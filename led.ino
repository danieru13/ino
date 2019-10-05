#include <ESP8266WiFi.h>
#include <PubSubClient.h> 

#define LED 2 //connect LED to digital pin2

// Update these with values suitable for your network.
const char* ssid = "WIFI-LIS";                   
//const char* password = "SSID_PASSWORD";          
const char* mqtt_server = "192.168.193.101";            

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  // initialize the digital pin2 as an output.
    pinMode(LED, OUTPUT);
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
  if (command == "ON") {
    // Turn On the lamp  
    digitalWrite(LED, HIGH); 
  }
  else {
    // Turn Off the lamp
    digitalWrite(LED, LOW);   
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
      client.subscribe("hospital/room/lamp");
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
}