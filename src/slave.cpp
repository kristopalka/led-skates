#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define AP_SSID "LED Skates"
#define AP_PASS "SomePassword"
#define UDP_PORT 4210


void setup() {
    pinMode(D4, OUTPUT);
}

void loop() {

}