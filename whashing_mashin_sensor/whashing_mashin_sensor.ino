
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
int ledPin = 13;
int EP =12; //d6
long lastmeasurement = 0;
int count = 0;
int counter = 0;
bool isWhashing = false;


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
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

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
      // ... and resubscribe
      client.subscribe("inTopic");
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
  pinMode(EP, INPUT); 
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

long measurement =TP_init();
  delay(50);
  Serial.println(measurement);
  Serial.println(count);
  if (lastmeasurement > 0 && measurement > 0){
    count ++;
    lastmeasurement = measurement;
  }
  else if (lastmeasurement == 0 && measurement == 0)  {
    count = 0;
    lastmeasurement = measurement;
  }
  lastmeasurement = measurement;

  if (count >= 15){
    Serial.print("washing washing");
    client.publish("Whashing", "start");
    isWhashing = true;
    
  }
  while(isWhashing == true){
   if (!client.connected()) {
    reconnect();
  }
  client.loop();
    long measurement =TP_init();
    delay(50);
    if (lastmeasurement == 0 && measurement == 0){
      counter++;
      Serial.println(measurement);
      Serial.println(counter);
      lastmeasurement = measurement;
    }
    else{
    counter = 0;
    lastmeasurement = measurement;
    }

    if (counter >=70){
      Serial.println("whasing is done");
      client.publish("Whashing", "stop");
      isWhashing = false;
    }
    
  }
  }
  long TP_init(){
  delay(95);
  long measurement=pulseIn (EP, HIGH);  //wait for the pin to get HIGH and returns measurement
  return measurement;
}
