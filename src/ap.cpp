#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <FastLED.h>

// WIFI config
#define AP_SSID "LedSkates"
#define AP_PASS "ASDH#$F*@JF@!"
#define UDP_PORT 8080

WiFiUDP UDP;
IPAddress local_IP(192, 168, 4, 1);
IPAddress client_IP(192, 168, 4, 2);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

char packetBuffer[UDP_TX_PACKET_MAX_SIZE];

void setup() {
    Serial.begin(74880);
    Serial.println("-------------------------");

    pinMode(D4, OUTPUT);

    Serial.println("Starting access point...");
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(local_IP, gateway, subnet);
    WiFi.softAP(AP_SSID, AP_PASS);
    Serial.println(WiFi.softAPIP());

    UDP.begin(UDP_PORT);
    Serial.print("Opening UDP port: ");
    Serial.println(UDP_PORT);
}

void loop() {
    UDP.beginPacket(client_IP, UDP_PORT);
    UDP.write(255);
    UDP.endPacket();
    digitalWrite(D4, HIGH);
    Serial.println("On");
    delay(1000);

    UDP.beginPacket(client_IP, UDP_PORT);
    UDP.write(0);
    UDP.endPacket();
    digitalWrite(D4, LOW);
    Serial.println("Off");
    delay(1000);
}