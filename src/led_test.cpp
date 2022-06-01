#include <Arduino.h>
// #include <FastLED.h>


// #define LED_NUM    1
// #define LED_PIN    D3

//CRGB leds[LED_NUM];


void setup() {
    Serial.begin(9600);
    pinMode(2, OUTPUT);
    //pinMode(D3, INPUT_PULLUP);

    //FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds,  LED_NUM).setCorrection(TypicalLEDStrip);
    //FastLED.setBrightness(255);
}

void loop() {
    //fill_solid(leds, LED_NUM, CRGB::Blue);
    digitalWrite(2, 1);
    //FastLED.show();
    Serial.println("On");
    delay(1000);

    
    //fill_solid(leds, LED_NUM, CRGB::Black);
    digitalWrite(2, 0);
    //FastLED.show();
    Serial.println("Off");
    delay(1000);
}