#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define AP_SSID "LED Skates"
#define AP_PASS "SomePassword"
#define UDP_PORT 4210

WiFiUDP UDP;
IPAddress local_IP(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];


void setup() {
    pinMode(D4, OUTPUT);

    Serial.begin(9600);
    Serial.println();

    Serial.println("Starting access point...");
    WiFi.softAPConfig(local_IP, gateway, subnet);
    WiFi.softAP(AP_SSID, AP_PASS);
    Serial.println(WiFi.localIP());

    UDP.begin(UDP_PORT);
    Serial.print("Listening on UDP port ");
    Serial.println(UDP_PORT);
}

void loop() {
    UDP.parsePacket();
    UDP.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);

    if (packetBuffer[0]) digitalWrite(2, HIGH);
    else digitalWrite(2, LOW);
}