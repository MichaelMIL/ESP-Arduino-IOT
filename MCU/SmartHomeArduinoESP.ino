#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include <DHT.h>


#define DHTPIN  4  //D2
#define DHTTYPE DHT22   // DHT 22  (AM2302)

//settings
// Update these with values suitable for your network.
String outNode, out, exitNode;
const char* ssid = "change"; // change Wifi SSID
const char* password = "pass"; //  change WiFi passwrd
const char* mqtt_server = "localhost"; // MQTT server IP

WiFiClient espClient;
PubSubClient client(espClient);

//relay timer
long lastMsg = 0;
char msg[50];
int message = 0;
int relayPin = 14; //D5


//wallswitch parameters
const int wallSwitchPin = 5; //D1
int reading;           // the current reading from the input pin
int previous = LOW;    // the previous reading from the input pin
unsigned long time1 = 0;           // the last time the output pin was toggled
unsigned long debounce = 200UL;   // the debounce time, increase if the output flickers

//DHT

DHT dht(DHTPIN, DHTTYPE);
int dhttimer = 0; 


// RELAY INFO
String relayStatus = "ֿֿ0"; // normally off =0, on=1
const String roomGroup = "LivingRoom/TV_Lamp"; //chnge to the name of the device
String statusUpdate = roomGroup.c_str();





void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(relayPin, OUTPUT);        // Initialize the relayPin pin as an output
  pinMode(wallSwitchPin, INPUT);    // Initialize the wallSwitchPin pin as an output
  Serial.begin(115200);       
  init_OTA();
  dht.begin();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  outNode = String(roomGroup);
  out = String("/out");
  exitNode =outNode+out;
  client.publish("WiFi_IoT_Status", roomGroup.c_str());
}

void loop() {
  ArduinoOTA.handle();
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  reading = digitalRead(wallSwitchPin);
  wallSwitch(reading);
  relayTimer(statusUpdate);
  DHTmessure();
  }
