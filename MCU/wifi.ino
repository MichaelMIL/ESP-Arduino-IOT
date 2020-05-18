

void reconnect() {
  // Loop until we're reconnected
  int counter = 0;
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
      client.publish("WiFi_IoT_Status", roomGroup.c_str());
      // ... and resubscribe
      client.subscribe(roomGroup.c_str());
      client.subscribe("WiFi_IoT_Status");
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
      counter++;
      if (counter == 6){
          Serial.println("Restarting");
          delay (400);
          ESP.restart();
      }
      
    }
  }
}
