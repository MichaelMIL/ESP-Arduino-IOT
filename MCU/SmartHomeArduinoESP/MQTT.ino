


void callback(char* topic, byte* payload, unsigned int length) {
  message = (char)payload[0];
  relay_from_MQTT(message);
  check_for_status(message);
  ResetMQTT(message);
}



void check_for_status (char message) {
  if (message == '3') // change for incoming status msg.
  client.publish("WiFi_IoT_Status", roomGroup.c_str());
}


void ResetMQTT (char message) {
  if (message == '4'){ //change for incoming restart cmd.
  client.publish("WiFi_IoT_Status", "Restarting");
  delay (400);
  ESP.restart();
  }
}


void relay_from_MQTT(int message) {
  if (message == '1'){ // change for on signal
    relayStatus = "1";
    relay_control(relayStatus);
  }
  else if (message == '0'){ //change for off signal.
    relayStatus = "0";
    relay_control(relayStatus);
  }
  
}
