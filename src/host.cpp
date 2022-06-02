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


// FastLED
#define LED_NUM 14
#define LED_PIN D3

CRGB leds[LED_NUM];


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


    FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds,  LED_NUM).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(255);
    fill_solid(leds, LED_NUM, CRGB::Green);
}

void loop() {
    UDP.beginPacket(client_IP, UDP_PORT);
    UDP.write(255);
    UDP.endPacket();
    FastLED.setBrightness(255);
    FastLED.show();
    digitalWrite(D4, LOW);
    Serial.println("On");
    delay(1000);

    UDP.beginPacket(client_IP, UDP_PORT);
    UDP.write(0);
    UDP.endPacket();
    FastLED.setBrightness(0);
    FastLED.show();
    digitalWrite(D4, HIGH);
    Serial.println("Off");
    delay(1000);
}