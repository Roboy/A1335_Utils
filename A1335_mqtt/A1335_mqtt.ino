#include <Wire.h>
#include "Definitions.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>


const char* ssid = "roboy";
const char* password = "wiihackroboy";
const char* mqtt_server = "192.168.0.134";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
double msg = 0.0;
//char msg[50];
int value = 0;

A1335State state;

Servo myservo;


void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  myservo.attach(13);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String str;
  for (int i = 0; i < length; i++) {
    str += (char)payload[i];
    Serial.print((char)payload[i]);
  }
  Serial.println();

  myservo.write(str.toInt());              // tell servo to go to position in variable 'pos'
  delay(100);  


}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
//      readDeviceState(0xF, &state);
//      client.publish("angle", String(state.angle).c_str());
      // ... and resubscribe
      client.subscribe("servo");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  
  Wire.begin();
  Wire.setClock(400000);
  Serial.begin(115200);
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

}


void loop() {
  int pos;

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000) {
    
    readDeviceState(0xF, &state);
    Serial.print(F("    Angle:  "));
    Serial.println(state.angle);
    lastMsg = now;
    ++value;
    msg = state.angle;
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("angle", String(msg).c_str());
    

  
  }
  

}
