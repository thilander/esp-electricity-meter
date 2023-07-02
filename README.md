# electricity-meter

Made for esp8266/esp32 devices.
Reads various values from the P1 port on (swedish) electricity meters, in order to get current consumption and so on.
Publishes these values on an mqtt queue over WiFi.
The device is powered through the P1 port, so no battery/external power is needed except for plugging in the RJ12 connector.

Credentials for WiFi / MQTT broker should be put in Credentials.h file (include/Credentials.h)

```
#define WIFI_SSID "not-specified"
#define WIFI_PW "not-specified"
#define MQTT_HOST "192,168.xx.yy"
#define MQTT_PORT 1337
#define MQTT_USER "not-specified"
#define MQTT_PW "not-specified"
```

Uses PlatformIO for a nicer dev environment and ArduinoOTA in order to program the device over the air.
I've added a small jumper on the schematics/device so that the device can easily be programmed by wire too.

## Schematics
TODO: put schematics here
