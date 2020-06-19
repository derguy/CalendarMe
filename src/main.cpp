#include <credentials.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <Wire.h>
#include <DebugMacros.h>
#include <HTTPSRedirect.h>
#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSerifBold9pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>

// Adafruit TPL5100. Put Diode from GND to DONE
#define TPL5110_DONE_PIN D1

// E-Paper + NodeMCU
// CLK -> GPIO14 (D5), DIN -> GPIO13 (D7), GND -> GND, 3.3V -> 3.3V
#define E_PAPER_CS_PIN D8
#define E_PAPER_DC_PIN D4
#define E_PAPER_RST_PIN D0
#define E_PAPER_BUSY_PIN D2

// 1.54" BWR - for other displays choose other Constructor
GxEPD2_3C<GxEPD2_154c, GxEPD2_154c::HEIGHT> display(GxEPD2_154c(E_PAPER_CS_PIN, E_PAPER_DC_PIN, E_PAPER_RST_PIN, E_PAPER_BUSY_PIN));

const char ssid[] = WIFI_SSID;
const char password[] = WIFI_PASSWD;

const char* host = "script.google.com";
const int httpsPort = 443;
const char* GScriptId = GSCRIPT_ID;
String gScriptUrl = String("/macros/s/") + GScriptId + "/exec?clientId=" + DEVICE_ID;
HTTPSRedirect* client = nullptr;

void connectWifi() {
    Serial.print("Connecting to wifi: ");
    Serial.println(ssid);
    Serial.flush();
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    client = new HTTPSRedirect(httpsPort);
    client->setInsecure();
    client->setPrintResponseBody(true);
    client->setContentTypeHeader("application/json");
}

String getDataFromCalendar() {
    Serial.print("Connecting to ");
    Serial.println(host);

    // Try to connect for a maximum of 5 times
    bool flag = false;
    for (int i = 0; i < 5; i++) {
        int retval = client->connect(host, httpsPort);
        if (retval == 1) {
            flag = true;
            break;
        } else
            Serial.println("Connection failed. Retrying...");
    }

    if (!flag) {
        Serial.print("Could not connect to server: ");
        Serial.println(host);
        Serial.println("Exiting...");
        return "";
    }

    client->GET(gScriptUrl, host);
    String response = client->getResponseBody();
    return response;
}

void printEPaper(StaticJsonDocument<500> json)
{
  display.setRotation(1);
  display.setTextColor(GxEPD_BLACK);
  int16_t tbx, tby; uint16_t tbw, tbh;
  uint16_t x = 0;
  uint16_t y = 40;
  display.setFullWindow();
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    
    // print out today
    display.setCursor(0, 15);
    display.setFont(&FreeSansBold12pt7b);
    display.print(json["header1"].as<char*>());
    display.setCursor(0, display.getCursorY() + 20);
    display.setFont(&FreeSansBold9pt7b);
    display.print(json["body1"].as<char*>());

    // print out tomorrow
    display.setFont(&FreeSansBold12pt7b);
    display.setCursor(0, display.getCursorY() + 5);
    display.print(json["header2"].as<char*>());
    display.setCursor(0, display.getCursorY() + 20);
    display.setFont(&FreeSansBold9pt7b);
    display.print(json["body2"].as<char*>());
  }
  while (display.nextPage());
}

void setup() {
    pinMode(TPL5110_DONE_PIN, OUTPUT);
    digitalWrite(TPL5110_DONE_PIN, LOW);

    Serial.begin(115200);
    Serial.flush();
    Serial.println("setup");
 
    connectWifi();
    
    // get and parse data
    String dataString = getDataFromCalendar();
    const char *dataChar = dataString.c_str();
    StaticJsonDocument<500> json;
    DeserializationError error = deserializeJson(json, dataChar);
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return;
    }

    // print data to epaper
    if (json["isNewData"]) {
        Serial.println("Updating Epaper");
        display.init(115200);
        printEPaper(json);
    } else {
        Serial.println("Skip updating");
    }

    // cleanup
    delete client;
    client = nullptr;
}

void loop() {
    // put arduino to sleep with TP tpl5110
    digitalWrite(TPL5110_DONE_PIN, HIGH);
    delay(2000);
    digitalWrite(TPL5110_DONE_PIN, LOW);
    delay(1000);
}