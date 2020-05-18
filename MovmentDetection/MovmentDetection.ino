
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.
const char* ssid = "change"; // change Wifi SSID
const char* password = "pass"; //  change WiFi passwrd
const char* mqtt_server = "localhost"; // MQTT server IP


WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
int message = 0;

String relayStatus = "ֿֿOFF";
const String roomGroup = "LivingRoom/Lamp"; //chnge to the name of the device//chnge to the name of the device

//motiondetection parameters
const int pirSensor = 5; //D1
int motion;
int previousPIR = 0;    // the previous reading from the input pin
unsigned long timePIR = 0;           // the last time the output pin was toggled
unsigned long debouncePIR = 200UL;   // the debounce time, increase if the output flickers



//wallswitch parameters
const int wallSwitchPin = 13; //D7
int reading;           // the current reading from the input pin
int previous = LOW;    // the previous reading from the input pin
unsigned long time1 = 0;           // the last time the output pin was toggled
unsigned long debounce = 200UL;   // the debounce time, increase if the output flickers


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
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  message = (char)payload[0];
  relay_from_MQTT(message);
  check_for_status(message);
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void relay_from_MQTT(int message) {
  if (message == '1'){
    relayStatus = "ON";
    relay_control(relayStatus);
  }
  else if (message == '0'){
    relayStatus = "OFF";
    relay_control(relayStatus);
  }
  
}


void relay_control(String relayStatus){
  if (relayStatus == "ON") {
     digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
     // but actually the LED is on; this is because
     // it is active low on the ESP-01)
     client.publish(roomGroup.c_str(), "B.relay: On");
     Serial.println("S.relay: ON");
     relayStatus = "The light is ON";
     client.publish(roomGroup.c_str(), relayStatus.c_str());
  }
  else if (relayStatus == "OFF"){
     digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
     client.publish(roomGroup.c_str(), "B.relay: Off");
     Serial.println("S.relay: OFF");
     relayStatus = "The light is OFF";
     client.publish(roomGroup.c_str(), relayStatus.c_str()); 
  }
}

void check_for_status (char message) {
  if (message == '3')
  client.publish(roomGroup.c_str(), relayStatus.c_str());
}

void wallSwitch(int reading){
  if (reading != previous  && millis() - time1 > debounce) {
    if (relayStatus == "ON"){
      relayStatus = "OFF";
      relay_control(relayStatus);
    }
    else if(relayStatus == "OFF") {
      relayStatus = "ON";
      relay_control(relayStatus);
    }


    time1 = millis(); 
  }
  
  previous = reading;
  
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += roomGroup.c_str();
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(roomGroup.c_str(), "I'm Connected, My name is: ");
      client.publish(roomGroup.c_str(), clientId.c_str());
      // ... and resubscribe
      client.subscribe(roomGroup.c_str());
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void MotionDetection(int motion = digitalRead(pirSensor)){
  if (motion != previousPIR  && millis() - timePIR > debouncePIR) {
    Serial.print("afterIF");
    Serial.print(motion);
    if (relayStatus == "ON"){
      Serial.println("Motion detected!");
      relayStatus = "OFF";
      relay_control(relayStatus);
    }
    else if(relayStatus == "OFF") {
      Serial.println("Motion detected!");
      relayStatus = "ON";
      relay_control(relayStatus);
    }


    timePIR = millis(); 
  }
  
  previousPIR = motion;
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(wallSwitchPin, INPUT);
  pinMode(pirSensor, INPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  reading = digitalRead(wallSwitchPin);
  wallSwitch(reading);
  //motion = digitalRead(pirSensor);
  MotionDetection(motion);
  long now = millis();
  if (now - lastMsg > 1800000) {
    lastMsg = now;
    client.publish(roomGroup.c_str(), "Living Room Lamp: ");
    client.publish(roomGroup.c_str(), relayStatus.c_str());
  }
}
