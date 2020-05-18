void DHTmessure(){
  long now = millis();
  if (now - dhttimer > 30000){
    dhttimer = now;
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    float hic = dht.computeHeatIndex(t, h, false);
    static char temperatureTemp[7];
    dtostrf(hic, 6, 2, temperatureTemp);
    static char humidityTemp[7];
    dtostrf(h, 6, 2, humidityTemp);
    client.publish("Hum" , humidityTemp);
    delay(100);
    client.publish("Temp" , temperatureTemp);
}
}
