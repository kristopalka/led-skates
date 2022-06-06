#include <ESP8266WiFi.h>
#include <FastLED.h>
#include <WiFiUdp.h>

// WIFI config
#define AP_SSID "LedSkates"
#define AP_PASS "ASDH#$F*@JF@!"
#define UDP_PORT 8080

WiFiUDP UDP;
IPAddress client_IP(192, 168, 100, 2);
IPAddress gateway(192, 168, 100, 1);
IPAddress subnet(255, 255, 255, 0);

char packetBuffer[UDP_TX_PACKET_MAX_SIZE];

// FastLED
#define LED_NUM 14
#define LED_PIN D3

CRGB leds[LED_NUM];

void setup() {
    Serial.begin(74880);
    Serial.println("-------------------------");

    WiFi.begin(AP_SSID, AP_PASS);
    WiFi.mode(WIFI_STA);

    if (!WiFi.config(client_IP, gateway, subnet)) {
        Serial.println("STA Failed to configure");
    }

    Serial.print("Connecting to AP");
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        Serial.print(".");
    }

    Serial.print("\nConnected! IP address: ");
    Serial.println(WiFi.localIP());

    UDP.begin(UDP_PORT);
    Serial.print("Opening UDP port: ");
    Serial.println(UDP_PORT);


    FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds,  LED_NUM).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(255);
    fill_solid(leds, LED_NUM, CRGB::Green);
}

void loop() {
    UDP.parsePacket();
    UDP.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);

    FastLED.setBrightness((int)packetBuffer[0]);
    FastLED.show();
    Serial.println((int)packetBuffer[0]);

    delay(10);
}