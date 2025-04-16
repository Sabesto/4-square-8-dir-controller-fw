/**
 * ----------------------------------------------------------------------------
 * ESP32 Remote Control with WebSocket
 * ----------------------------------------------------------------------------
 * © 2020 Stéphane Calderoni
 * ----------------------------------------------------------------------------
 */

#include <Arduino.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include "pins.h"
#include "secrets.h"
#include "rotswitch.h"
#include "swrleds.h"
#include "tca9539.h"


// ----------------------------------------------------------------------------
// Definition of macros
// ----------------------------------------------------------------------------

#define LED_PIN   4
#define BTN_PIN   0
#define NEO_COUNT 1
#define HTTP_PORT 80
/*
#define SCL_PIN   47
#define SDA_PIN   21
#define IO_EXP_1_ADDR 0x74
*/
// ----------------------------------------------------------------------------
// Definition of global constants
// ----------------------------------------------------------------------------

// WiFi credentials
const char *WIFI_SSID = ssid_name;
const char *WIFI_PASS = ssid_password;

uint8_t wantedDirection = 0;
uint8_t actualDirection = 0;
uint16_t currentSWR, scaledSWR, previousSWR, relayCurr= 0;
bool currentPTT, previousPTT = 0;

unsigned long prevUpdateMS = 0;
const long minInterval = 250;

const uint8_t directionRelayMask[] = {DIR_N, DIR_NE, DIR_E, DIR_SE, DIR_S, DIR_SW, DIR_W, DIR_NW};
const uint8_t dirNumRelaysSet[] = {2, 0, 3, 6, 3, 2, 3, 4};
const uint8_t directionLEDPin[] = {DIR_LED_N, DIR_LED_NE, DIR_LED_E, DIR_LED_SE, DIR_LED_S, DIR_LED_SW, DIR_LED_W, DIR_LED_NW};

// Flags
volatile bool checkIOEX2inputs = false;

// ----------------------------------------------------------------------------
// Definition of global variables
// ----------------------------------------------------------------------------
TCA9539 ioex1;
TCA9539 ioex2;

AsyncWebServer server(HTTP_PORT);
AsyncWebSocket ws("/ws");

// ----------------------------------------------------------------------------
// SPIFFS initialization
// ----------------------------------------------------------------------------

void initSPIFFS() {
    if (!SPIFFS.begin()) {
        Serial.println("Cannot mount SPIFFS volume...");
    }
}

// ----------------------------------------------------------------------------
// Connecting to the WiFi network
// ----------------------------------------------------------------------------

void initWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.printf("Trying to connect [%s] ", WiFi.macAddress().c_str());
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.printf(" %s\n", WiFi.localIP().toString().c_str());
}

// ----------------------------------------------------------------------------
// Web server initialization
// ----------------------------------------------------------------------------

void onRootRequest(AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", "text/html", false, NULL);
}

void initWebServer() {
    server.on("/", onRootRequest);
    server.serveStatic("/", SPIFFS, "/");
    server.begin();
}

// ----------------------------------------------------------------------------
// WebSocket initialization
// ----------------------------------------------------------------------------

void notifyClients() {
    unsigned long currUpdateMS = millis();

    // Dont update faster then this
    if (currUpdateMS - prevUpdateMS >= minInterval) {
        JsonDocument json;
        json["adir"] = actualDirection;
        json["wdir"] = wantedDirection;
        json["ptt"] = currentPTT ? "on" : "off";
        json["swr"] = currentSWR;
        json["relcurr"] = relayCurr;
        json["nrelset"] = dirNumRelaysSet[actualDirection];

        char buffer[200];
        size_t len = serializeJson(json, buffer);
        ws.textAll(buffer, len);

        prevUpdateMS = currUpdateMS;
    }
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {

        JsonDocument json;
        DeserializationError err = deserializeJson(json, data);
        if (err) {
            Serial.print(F("deserializeJson() failed with code "));
            Serial.println(err.c_str());
            return;
        }

        const char *action = json["action"];
        if(action[0] >= '0' && action[0] <= '7') {
            wantedDirection = (uint8_t)(action[0] - '0');
            notifyClients();
        }
    }
}

void onEvent(AsyncWebSocket       *server,
             AsyncWebSocketClient *client,
             AwsEventType          type,
             void                 *arg,
             uint8_t              *data,
             size_t                len) {

    switch (type) {
        case WS_EVT_CONNECT:
            Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
            break;
        case WS_EVT_DISCONNECT:
            Serial.printf("WebSocket client #%u disconnected\n", client->id());
            break;
        case WS_EVT_DATA:
            handleWebSocketMessage(arg, data, len);
            break;
        case WS_EVT_PONG:
            Serial.printf("WS_EVT_PONG, client: #%u\n", client->id());
        case WS_EVT_ERROR:
            break;
    }
}

void initWebSocket() {
    ws.onEvent(onEvent);
    server.addHandler(&ws);
}

void IRAM_ATTR handleIOEX2int() {
    checkIOEX2inputs = true;
}

void IRAM_ATTR handlePTTint() {
    currentPTT = digitalRead(PTT_SENSE);
    digitalWrite(PTT_LED, currentPTT ? LOW : HIGH);
}


uint8_t getRotarySwitchPosition() {
    uint8_t data = ioex2.input(TCA9539::Port::PORT0);
    // 0 means invalid position or in between positions
    if (data != 0){
        if (log2(data) <= 3) {
            return log2(data) + 4;
        } else {
            return log2(data) - 4;
        }
    } else {
        return -1;
    }
    
    
}
// ----------------------------------------------------------------------------
// Initialization
// ----------------------------------------------------------------------------

void setup() {
    Serial.begin(115200); delay(500);

    initSPIFFS();
    initWiFi();
    initWebSocket();
    initWebServer();
    initSWRLEDs();

    // PTT Input
    pinMode(PTT_SENSE, INPUT);
    pinMode(PTT_LED, OUTPUT);
    digitalWrite(PTT_LED, HIGH);

    pinMode(RELAY_CURRENT, INPUT);
   
    // Reset IO-Expanders
    pinMode(RESET, OUTPUT);
    digitalWrite(RESET, HIGH);
    delay(10);
    digitalWrite(RESET, LOW);
    delay(10);
    digitalWrite(RESET, HIGH);

    // Initialize IO Expanders
    Wire.begin(SDA, SCL);
    ioex1.attach(Wire);
    ioex1.setDeviceAddress(IOEX1_ADDR);
    ioex1.config(TCA9539::Port::PORT0, TCA9539::Config::OUT);
    ioex1.config(TCA9539::Port::PORT1, TCA9539::Config::OUT);
    ioex2.attach(Wire);
    ioex2.setDeviceAddress(IOEX2_ADDR);
    ioex2.config(TCA9539::Port::PORT0, TCA9539::Config::IN);
    ioex2.config(TCA9539::Port::PORT1, TCA9539::Config::OUT);
    
    // Interrupt from IO Expander 2, change on rotary switch
    attachInterrupt(INT, handleIOEX2int,FALLING);
    // Interrupt from PTT, change on PTT switch
    attachInterrupt(PTT_SENSE, handlePTTint, CHANGE);

    // We just booted, get position of rotswitch and set antenna to that
    wantedDirection = getRotarySwitchPosition();
}

// ----------------------------------------------------------------------------
// Main control loop
// ----------------------------------------------------------------------------
/*
TODO:
- Buzzer
- Show all LEDS on boot
- Measure relay current for each relay on boot and on command?
- Disable PTT passthrough on high SWR
*/

void loop() {
    ws.cleanupClients();
    notifyClients();
    
    // ioex2 interrupt has been triggered
    // check if rotary switch position has changed
    if (checkIOEX2inputs) {
        checkIOEX2inputs = false;
        int8_t value = getRotarySwitchPosition();
        if (value >= 0) {
            wantedDirection = value;
        }
    }

    // Read and scale SWR
    currentSWR = analogRead(SWR_FILT);
    scaledSWR = map(currentSWR, 0, 4095, 0, 10);
    setSWRLEDs(0);
    setSWRLEDs(scaledSWR);

    // Read external relay current
    relayCurr = analogRead(RELAY_CURRENT);
    // 663 = 222mA

    // Check if PTT has changed
    if (currentPTT != previousPTT) {
        notifyClients();
        previousPTT = currentPTT;
    }    

    // Check if wanted == actual direction
    if (wantedDirection != actualDirection && wantedDirection >= 0) {
        // Check if SWR is low and PTT is not active
        if (currentSWR < 1500 && currentPTT == LOW) {
            
            // Set relays
            ioex1.outputPort(TCA9539::Port::PORT0, ~directionRelayMask[wantedDirection]);
            
            // Set Direction LEDs
            ioex2.outputPort(TCA9539::Port::PORT1, 0xFF);
            ioex2.output(directionLEDPin[wantedDirection], 0);
            
            // Update actual direction
            actualDirection = wantedDirection;
            
            // Notify web clients
            notifyClients();
        }
    }
}