#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WS2812FX.h>
#include <WiFiUdp.h>

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

#include "web/index.html.cpp"
#include "web/main.js.cpp"

#define WIFI_SSID "LedSkates"
#define WIFI_PASSWORD "1234567890"
#define HTTP_PORT 80
#define UDP_PORT 8080

IPAddress host_ip(192, 168, 100, 1);
IPAddress client_ip(192, 168, 100, 2);
IPAddress gateway(192, 168, 100, 1);
IPAddress subnet(255, 255, 255, 0);

#define LED_PIN D3
#define LED_COUNT 14

unsigned long auto_last_change = 0;
unsigned long last_wifi_check_time = 0;
String modes_html = "";
uint8_t myModes[] = {};  // optionally create a custom list of effect/mode numbers
bool auto_cycle = false;

WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
ESP8266WebServer server(HTTP_PORT);

void modes_setup();
void leds_setup();
void wifi_setup();
void server_setup();
void handle_auto_mode();
void srv_handle_not_found();
void srv_handle_index_html();
void srv_handle_main_js();
void srv_handle_modes();
void srv_handle_set();

////////////////////////////////////////////// SETUP //////////////////////////////////////////////
void setup() {
    Serial.begin(74880);
    delay(500);

    modes_setup();
    leds_setup();
    wifi_setup();
    server_setup();
}

void modes_setup() {
    modes_html.reserve(5000);

    modes_html = "";
    uint8_t num_modes = sizeof(myModes) > 0 ? sizeof(myModes) : ws2812fx.getModeCount();
    for (uint8_t i = 0; i < num_modes; i++) {
        uint8_t m = sizeof(myModes) > 0 ? myModes[i] : i;
        modes_html += "<li><a href='#'>";
        modes_html += ws2812fx.getModeName(m);
        modes_html += "</a></li>";
    }
}

void leds_setup() {
    Serial.println("WS2812FX setup");

    ws2812fx.init();
    ws2812fx.setMode(FX_MODE_STATIC);
    ws2812fx.setColor(0xFF5900);
    ws2812fx.setSpeed(1000);
    ws2812fx.setBrightness(128);
    ws2812fx.start();
}

void wifi_setup() {
    Serial.println("Wifi setup");
    Serial.println("Starting access point...");
    WiFi.softAPConfig(host_ip, gateway, subnet);

    bool success = WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
    if (success) {
        Serial.println("Success!");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("Failed. Resetting...");
        delay(1000);
        ESP.reset();
    }
}

void server_setup() {
    Serial.println("HTTP server setup");

    server.on("/", srv_handle_index_html);
    server.on("/main.js", srv_handle_main_js);
    server.on("/modes", srv_handle_modes);
    server.on("/set", srv_handle_set);
    server.onNotFound(srv_handle_not_found);

    server.begin();

    Serial.println("HTTP server started.");
}

////////////////////////////////////////////// LOOP //////////////////////////////////////////////

void loop() {
    server.handleClient();
    ws2812fx.service();

    handle_auto_mode();
}

void handle_auto_mode() {
    unsigned long now = millis();

    if (auto_cycle && (now - auto_last_change > 10000)) {  // cycle effect mode every 10 seconds
        uint8_t next_mode = (ws2812fx.getMode() + 1) % ws2812fx.getModeCount();

        if (sizeof(myModes) > 0) {  // if custom list of modes exists
            for (uint8_t i = 0; i < sizeof(myModes); i++) {
                if (myModes[i] == ws2812fx.getMode()) {
                    next_mode = ((i + 1) < sizeof(myModes)) ? myModes[i + 1] : myModes[0];
                    break;
                }
            }
        }

        ws2812fx.setMode(next_mode);
        Serial.print("mode is ");
        Serial.println(ws2812fx.getModeName(ws2812fx.getMode()));
        auto_last_change = now;
    }
}

////////////////////////////////////////////// Webserver Functions //////////////////////////////////////////////

void srv_handle_not_found() {
    server.send(404, "text/plain", "File Not Found");
}

void srv_handle_index_html() {
    server.send_P(200, "text/html", index_html);
}

void srv_handle_main_js() {
    server.send_P(200, "application/javascript", main_js);
}

void srv_handle_modes() {
    server.send(200, "text/plain", modes_html);
}

void srv_handle_set() {
    Serial.println(server.uri());

    for (uint8_t i = 0; i < server.args(); i++) {
        if (server.argName(i) == "c") {
            uint32_t tmp = (uint32_t)strtol(server.arg(i).c_str(), NULL, 10);
            if (tmp <= 0xFFFFFF) {
                ws2812fx.setColor(tmp);
            }
        }

        if (server.argName(i) == "m") {
            uint8_t tmp = (uint8_t)strtol(server.arg(i).c_str(), NULL, 10);
            uint8_t new_mode = sizeof(myModes) > 0
                                   ? myModes[tmp % sizeof(myModes)]
                                   : tmp % ws2812fx.getModeCount();
            ws2812fx.setMode(new_mode);
            auto_cycle = false;
            Serial.print("mode is ");
            Serial.println(ws2812fx.getModeName(ws2812fx.getMode()));
        }

        if (server.argName(i) == "b") {
            if (server.arg(i)[0] == '-') {
                ws2812fx.setBrightness(ws2812fx.getBrightness() * 0.8);
            } else if (server.arg(i)[0] == ' ') {
                ws2812fx.setBrightness(
                    min(max(ws2812fx.getBrightness(), 5) * 1.2, 255));
            } else {  // set brightness directly
                uint8_t tmp = (uint8_t)strtol(server.arg(i).c_str(), NULL, 10);
                ws2812fx.setBrightness(tmp);
            }
            Serial.print("brightness is ");
            Serial.println(ws2812fx.getBrightness());
        }

        if (server.argName(i) == "s") {
            if (server.arg(i)[0] == '-') {
                ws2812fx.setSpeed(max(ws2812fx.getSpeed(), 5) * 1.2);
            } else if (server.arg(i)[0] == ' ') {
                ws2812fx.setSpeed(ws2812fx.getSpeed() * 0.8);
            } else {
                uint16_t tmp = (uint16_t)strtol(server.arg(i).c_str(), NULL, 10);
                ws2812fx.setSpeed(tmp);
            }
            Serial.print("speed is ");
            Serial.println(ws2812fx.getSpeed());
        }

        if (server.argName(i) == "a") {
            if (server.arg(i)[0] == '-') {
                auto_cycle = false;
            } else {
                auto_cycle = true;
                auto_last_change = 0;
            }
        }
    }
    server.send(200, "text/plain", "OK");
}