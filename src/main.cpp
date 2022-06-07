#define MODE SERVER  // mode san be SERVER or CLIENT

// #define min(a, b) ((a) < (b) ? (a) : (b))
// #define max(a, b) ((a) > (b) ? (a) : (b))

#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WS2812FX.h>
#include <WiFiUdp.h>

#include "web/arg.cpp"
#include "web/index.html.cpp"
#include "web/main.js.cpp"

#define WIFI_SSID "LedSkates"
#define WIFI_PASSWORD "1234567890"
#define HTTP_PORT 80
#define UDP_PORT 8080

#define LED_PIN D3
#define LED_COUNT 14

IPAddress host_ip(192, 168, 100, 1);
IPAddress client_ip(192, 168, 100, 2);
IPAddress gateway(192, 168, 100, 1);
IPAddress subnet(255, 255, 255, 0);

unsigned long auto_last_change = 0;
unsigned long last_wifi_check_time = 0;
String html_modes = "";
bool auto_cycle = false;

WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
ESP8266WebServer server(HTTP_PORT);
WiFiUDP UDP;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];

void html_modes_setup();
void leds_setup();
void wifi_AP_setup();
void web_server_setup();
void wifi_server_setup();
void wifi_client_setup();
void receive_packet();
void handle_auto_mode();
void change_mode(Arg arg);
void srv_handle_not_found();
void srv_handle_index_html();
void srv_handle_main_js();
void srv_handle_modes();
void srv_handle_set();

////////////////////////////////////////////// SETUP //////////////////////////////////////////////
void setup() {
    Serial.begin(74880);
    delay(500);

    leds_setup();
#if MODE == SERVER
    html_modes_setup();
    web_server_setup();
    wifi_server_setup();
#elif
    wifi_client_setup();
#endif

    UDP.begin(UDP_PORT);
}

void html_modes_setup() {
    html_modes.reserve(5000);

    html_modes = "";
    uint8_t num_modes = ws2812fx.getModeCount();
    for (uint8_t i = 0; i < num_modes; i++) {
        html_modes += "<li><a href='#'>";
        html_modes += ws2812fx.getModeName(i);
        html_modes += "</a></li>";
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

void wifi_server_setup() {
    Serial.println("Wifi setup");
    Serial.println("Starting access point...");
    WiFi.softAPConfig(host_ip, gateway, subnet);

    bool success = WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
    if (success) {
        Serial.println("Success!");
        Serial.println(WiFi.softAPIP());
    } else {
        Serial.println("Failed. Resetting...");
        delay(1000);
        ESP.reset();
    }
}

void wifi_client_setup() {
    Serial.println("Wifi setup");
    Serial.print("Connecting to ");
    Serial.print(WIFI_SSID);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.print("\nConnected! IP address: ");
    Serial.println(WiFi.localIP());
}

void web_server_setup() {
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
#if MODE == SERVER
    server.handleClient();
    ws2812fx.service();
#elif
    receive_packet();
#endif
    handle_auto_mode();
}

void receive_packet() {
    UDP.parsePacket();
    UDP.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);

    Arg arg(packetBuffer);
    change_mode(arg);
}

void handle_auto_mode() {
    unsigned long now = millis();

    if (auto_cycle && (now - auto_last_change > 10000)) {  // cycle effect mode every 10 seconds
        uint8_t next_mode = (ws2812fx.getMode() + 1) % ws2812fx.getModeCount();

        ws2812fx.setMode(next_mode);
        Serial.print("mode is ");
        Serial.println(ws2812fx.getModeName(ws2812fx.getMode()));
        auto_last_change = now;
    }
}

void change_mode(Arg arg) {
    if (arg.key == "c") {
        uint32_t tmp = (uint32_t)strtol(arg.value.c_str(), NULL, 10);
        if (tmp <= 0xFFFFFF) {
            ws2812fx.setColor(tmp);
        }
    }

    if (arg.key == "m") {
        uint8_t tmp = (uint8_t)strtol(arg.value.c_str(), NULL, 10);
        uint8_t new_mode = tmp % ws2812fx.getModeCount();
        ws2812fx.setMode(new_mode);
        auto_cycle = false;
        Serial.print("mode is ");
        Serial.println(ws2812fx.getModeName(ws2812fx.getMode()));
    }

    if (arg.key == "b") {
        if (arg.value.c_str()[0] == '-') {
            ws2812fx.setBrightness(ws2812fx.getBrightness() * 0.8);
        } else if (arg.value.c_str()[0] == ' ') {
            ws2812fx.setBrightness(min((int)(max(ws2812fx.getBrightness(), (uint8_t)5) * 1.2), 255));
        } else {  // set brightness directly
            uint8_t tmp = (uint8_t)strtol(arg.value.c_str(), NULL, 10);
            ws2812fx.setBrightness(tmp);
        }
        Serial.print("brightness is ");
        Serial.println(ws2812fx.getBrightness());
    }

    if (arg.key == "s") {
        if (arg.value.c_str()[0] == '-') {
            ws2812fx.setSpeed(max(ws2812fx.getSpeed(), (uint16_t)5) * 1.2);
        } else if (arg.value.c_str()[0] == ' ') {
            ws2812fx.setSpeed(ws2812fx.getSpeed() * 0.8);
        } else {
            uint16_t tmp = (uint16_t)strtol(arg.value.c_str(), NULL, 10);
            ws2812fx.setSpeed(tmp);
        }
        Serial.print("speed is ");
        Serial.println(ws2812fx.getSpeed());
    }

    if (arg.key == "a") {
        if (arg.value.c_str()[0] == '-') {
            auto_cycle = false;
        } else {
            auto_cycle = true;
            auto_last_change = 0;
        }
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
    server.send(200, "text/plain", html_modes);
}

void srv_handle_set() {
    for (uint8_t i = 0; i < server.args(); i++) {
        Arg arg(server.argName(i), server.arg(i));

        UDP.beginPacket(client_ip, UDP_PORT);
        UDP.write(arg.toString().c_str());
        UDP.endPacket();

        delay(10);

        change_mode(arg);
    }
    server.send(200, "text/plain", "OK");
}
