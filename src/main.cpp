#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Credentials.h>

#ifndef CREDENTIALS_H
#define CREDENTIALS_H

#define WIFI_SSID "not-specified"
#define WIFI_PW "not-specified"

#endif

String latestTelegram;
String latestDateTime;
String latestMeterReading;
String latestActivePower;

ESP8266WebServer server(80);

void handleRoot();
void handleRaw();
void handleMeterReading();
void handleActivePower();

String getTelegram();
String getDateTime(String telegram); // 0-0:1.0.0
String getMeterReading(String telegram); // 1-0:1.8.0
String getActualElectricityPowerDelivered(String telegram); // 1-0:1.7.0

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PW);
  uint32_t notConnectedCounter = 0;
  while (WiFi.status() != WL_CONNECTED) {
      delay(100);
      notConnectedCounter++;
      if(notConnectedCounter > 150) {
          ESP.restart();
      }
  }

  server.on("/", handleRoot);
  server.on("/raw", handleRaw);
  server.on("/ap", handleActivePower);
  server.begin();

  ArduinoOTA.begin();
}

void loop() {
  ArduinoOTA.handle();

  String telegram = getTelegram();
  latestTelegram = telegram;
  String dateTime = getDateTime(telegram);

  if (dateTime != "" && dateTime != latestDateTime) {
    latestDateTime = dateTime;
    latestMeterReading = getMeterReading(telegram);
    latestActivePower = getActualElectricityPowerDelivered(telegram);
  }
  server.handleClient();
}

void handleRoot() {
  // String s = "Tidpunkt: " + latestDateTime + "\nMätarställning: " + latestMeterReading + "kWh\nAktiv effekt: " + latestActivePower + "kW";
  // server.send(200, "text/plain", s);

  String result = "{\"time\":" + latestDateTime + ",\"meterReading\":" + latestMeterReading + ",\"activePower\":" + latestActivePower + "}";
  server.send(200, "application/json", result);
}

void handleRaw() {
  server.send(200, "text/plain", latestTelegram);
}

void handleMeterReading() {
}

void handleActivePower() {
  server.send(200, "text/plain", latestActivePower + "kW");
}

String getTelegram() {
  String s;
  while(Serial.available()) {
    s = Serial.readString();
  }
  return s;
}

String getDateTime(String telegram) {
  // YYMMDDhhmmssX
  // Swedish normal time, not adjusted for summer time , (X=W on normal time)
  int sI = telegram.indexOf("0-0:1.0.0");
  if (sI < 0) {
    return "";
  }
  return telegram.substring(sI+10, sI+23);
}

String getMeterReading(String telegram) {
  int sI = telegram.indexOf("1-0:1.8.0");
  if (sI < 0) {
    return "";
  }
  return telegram.substring(sI+10, sI+22);
}

String getActualElectricityPowerDelivered(String telegram) {
  int sI = telegram.indexOf("1-0:1.7.0");
  if (sI < 0) {
    return "";
  }
  return telegram.substring(sI+10, sI+18);
}

