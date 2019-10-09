#include <ESP8266WiFi.h>
#include <PubSubClient.h> 
#include <Servo.h>

#define PIR_MOTION_SENSOR 2//Use pin 2 to receive the signal from the module


Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position
// Update these with values suitable for your network.
const char* ssid = "WIFI-LIS";                   
//const char* password = "SSID_PASSWORD";          
const char* mqtt_server = "192.168.192.7";            

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;

void setup() {
    myservo.attach(5);  // attaches the servo on pin 5 to the servo object
    pinMode(PIR_MOTION_SENSOR, INPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.disconnect();
  delay(100);
  WiFi.begin(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    Serial.print(WiFi.status());
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String command;
  for (int i = 0; i < length; i++) {
    command += (char)payload[i];
  }
  Serial.print(command);  
  Serial.println(); 
  if (command == "ON" || command == "OPEN") {
    for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  }
  else {
    for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
    }  
  }   
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // ... and resubscribe
      client.subscribe("hospital/room/window");
      client.subscribe("hospital/room/fan");
    } 
    else {
      Serial.print("failed connection, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if(digitalRead(PIR_MOTION_SENSOR)){//if it detects the moving people?
        Serial.println("Movement");
        client.publish("hotel/room/door", "ON");
  }else{
        Serial.println("Watching");
        client.publish("hotel/room/door", "OFF");
  }
    delay(200);

  
}