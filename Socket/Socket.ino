
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.
const char* ssid = "M&M's";
const char* password = "mia12345";
const char* mqtt_server = "192.168.1.103";


WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
int message = 0;

String relayStatus = "ֿֿOFF";
const String roomGroup = "LivingRoom/Socket";


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
     client.publish(roomGroup.c_str(), relayStatus.c_str());
  }
  else if (relayStatus == "OFF"){
     digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
     client.publish(roomGroup.c_str(), relayStatus.c_str()); 
  }
}

void check_for_status (char message) {
  if (message == '3')
  client.publish("WiFi_IoT_Status", roomGroup.c_str());
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
      client.subscribe("WiFi_IoT_Status");
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
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
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
  long now = millis();
  if (now - lastMsg > 1800000) {
    lastMsg = now;
    client.publish(roomGroup.c_str(), "Living Room Socket: ");
    client.publish(roomGroup.c_str(), relayStatus.c_str());
  }
}
