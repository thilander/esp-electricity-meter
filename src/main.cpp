#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <Credentials.h>

#ifndef CREDENTIALS_H
#define CREDENTIALS_H

#define WIFI_SSID "not-specified"
#define WIFI_PW "not-specified"

#endif

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PW);
  uint32_t notConnectedCounter = 0;
  while (WiFi.status() != WL_CONNECTED) { 
      delay(100);
      Serial.println("WiFi connecting...");
      notConnectedCounter++;
      if(notConnectedCounter > 150) {
          Serial.println("Resetting due to Wifi not connecting...");
          ESP.restart();
      }
  }
  Serial.print("Wifi connected, IP address: ");
  Serial.println(WiFi.localIP());
  ArduinoOTA.begin();
}

void loop() {
  ArduinoOTA.handle();
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
}