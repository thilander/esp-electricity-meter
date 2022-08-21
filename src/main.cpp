#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Credentials.h>

#ifndef CREDENTIALS_H
#define CREDENTIALS_H

#define WIFI_SSID "not-specified"
#define WIFI_PW "not-specified"
#define MQTT_HOST "not-specified"
#define MQTT_USER "not-specified"
#define MQTT_PW "not-specified"

#endif

String latestTelegram;
String latestDateTime;
String latestMeterReading;
String latestActivePower;
String latestActivePowerL1;
String latestActivePowerL2;
String latestActivePowerL3;

const char *mqtt_broker = MQTT_HOST;
const char *mqtt_user = MQTT_USER;
const char *mqtt_pass = MQTT_PW;
const char *mqtt_topic_ap = "electricity/activepower/total";
const char *mqtt_topic_ap_l1 = "electricity/activepower/l1";
const char *mqtt_topic_ap_l2 = "electricity/activepower/l2";
const char *mqtt_topic_ap_l3 = "electricity/activepower/l3";
const char *mqtt_topic_mr = "electricity/meterreading";
const char *restart_topic = "electricity/restart";

const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void handleRoot();
void handleRaw();
void handleMeterReading();
void handleActivePower();

String getTelegram();
String getDateTime(String telegram); // 0-0:1.0.0
String getMeterReading(String telegram); // 1-0:1.8.0
String getActualElectricityPowerDelivered(String telegram); // 1-0:1.7.0
String getActualElectricityPowerDeliveredL1(String telegram); // 1-0:21.7.0
String getActualElectricityPowerDeliveredL2(String telegram); // 1-0:41.7.0
String getActualElectricityPowerDeliveredL3(String telegram); // 1-0:61.7.0

const unsigned long millisUntilReset = 10*60*1000; // ten minutes
unsigned long millisSinceStart;

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

  // MQTT
  client.setServer(mqtt_broker, mqtt_port);
  while (!client.connected()) {
    String client_id = "esp8266-electricity-meter";
    if (client.connect(client_id.c_str(), mqtt_user, mqtt_pass)) {
      // connected
    } else {
      delay(2000);
    }
  }

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
    latestActivePowerL1 = getActualElectricityPowerDeliveredL1(telegram);
    latestActivePowerL2 = getActualElectricityPowerDeliveredL2(telegram);
    latestActivePowerL3 = getActualElectricityPowerDeliveredL3(telegram);

    client.publish(mqtt_topic_mr, latestMeterReading.c_str());
    client.publish(mqtt_topic_ap, latestActivePower.c_str());
    client.publish(mqtt_topic_ap_l1, latestActivePowerL1.c_str());
    client.publish(mqtt_topic_ap_l2, latestActivePowerL2.c_str());
    client.publish(mqtt_topic_ap_l3, latestActivePowerL3.c_str());
  }

  millisSinceStart = millis();
  if (millisSinceStart > millisUntilReset) {
    ESP.restart();
  }
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

String getActualElectricityPowerDeliveredL1(String telegram) {
  int sI = telegram.indexOf("1-0:21.7.0");
  if (sI < 0) {
    return "";
  }
  return telegram.substring(sI+11, sI+19);
}

String getActualElectricityPowerDeliveredL2(String telegram) {
  int sI = telegram.indexOf("1-0:41.7.0");
  if (sI < 0) {
    return "";
  }
  return telegram.substring(sI+11, sI+19);
}

String getActualElectricityPowerDeliveredL3(String telegram) {
  int sI = telegram.indexOf("1-0:61.7.0");
  if (sI < 0) {
    return "";
  }
  return telegram.substring(sI+11, sI+19);
}
