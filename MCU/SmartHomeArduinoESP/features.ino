

void relay_control(String relayStatus){ //control the relay
  if (relayStatus == "1") {
     digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
     //digitalWrite(relayPin, HIGH); // normally open
     digitalWrite(relayPin, LOW); //normally close
     client.publish(exitNode.c_str(), relayStatus.c_str());
  }
  else if (relayStatus == "0"){
     digitalWrite(BUILTIN_LED, HIGH);
     //digitalWrite(relayPin, LOW); // normally open
     digitalWrite(relayPin, HIGH);//normally close
     client.publish(exitNode.c_str(), relayStatus.c_str()); 
  }
}


void wallSwitch(int reading){ // detects changes in wallswitch
  if (reading != previous  && millis() - time1 > debounce) {
    if (relayStatus == "1"){ 
      relayStatus = "0";
      relay_control(relayStatus);
    }
    else if (relayStatus =="0") {
      relayStatus = "1";
      relay_control(relayStatus);
    }


    time1 = millis(); 
  }
  
  previous = reading;
  
}


void relayTimer (String statusUpdate){ // update node every 30 mins.
    long now = millis();
  if (now - lastMsg > 300000) { // change time in millis for update mins (30mins = 300000)
    lastMsg = now;
    statusUpdate += relayStatus.c_str();
    client.publish("RelayStatus", statusUpdate.c_str());
  }
}
