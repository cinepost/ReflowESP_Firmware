#include <Arduino.h>

#include "main.h"

// Macros / config
#include "core/macros.h"
#include "config/Config.h"

// Persist
#include <Preferences.h>
#include <SPIFFS.h>

// Buzzer
#include <Buzzer.h>

// OTA
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ElegantOTA.h>

// Widget library
#include <TFT_eWidget.h>

// JSON
#include <ArduinoJson.h>

#include "core/info.h"
#include "core/pwm.h"
#include "core/therm.h"

const char* ssid = "TarNet";
const char* password = "kirienko";

AsyncWebServer server(80);
reflow_esp::SystemInfo sysinfo;

// Menu
#include "core/menu.h"

// Preferences
Preferences preferences;

void IRAM_ATTR onTimer(); // Start the timer to read the clickEncoder every 1 ms

float pwmState = 0;
int pwmPeriodSec = 5;

hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

pwm::PWMOutput pwmout;
therm::Thermocouple tc1, tc2;

Buzzer::Melody_t testMelody {
  .nbNotes = 3,
  .duration = {400, 400, 800},
  .frequency = {D4_NOTE_FREQ, E4_NOTE_FREQ, G4_NOTE_FREQ}
};

Buzzer myBuzzer;

MeterWidget   tcMeter1 = MeterWidget(&gfx);
MeterWidget   tcMeter2 = MeterWidget(&gfx);

/* -------------------------------------------------------------------------- */
/*                                    SETUP                                   */
/* -------------------------------------------------------------------------- */

bool initSPIFFS() {
  if(!SPIFFS.begin(true)) {
    Serial.println("Error mounting SPIFFS.");
    return false;
  }
  Serial.println("SPIFFS mounted");
  return true;
}

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void initPins() {
  pinMode(BUZZ, OUTPUT);
  pinMode(SSR1, OUTPUT);
  pinMode(SSR2, OUTPUT);
}

void initPWM() {
  pwmout.setPin(BUZZ);
  pwmout.setPeriod(PWM_PERIOD);
}

void initTherm() {
  tc1.setPins(THERM_MISO, THERM_SCK, THERM_CS1);
  tc1.setPeriod(THERM_POLL_PERIOD);

  tc2.setPins(THERM_MISO, THERM_SCK, THERM_CS2);
  tc2.setPeriod(THERM_POLL_PERIOD);
}

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void handleStatus(AsyncWebServerRequest *request) {
    StaticJsonDocument<JSON_BUF_SIZE> json;
  
    json["tc1"] = tc1.read();
    json["tc2"] = tc2.read();
    //json["heater1"] =
    //json["heater2"] = 
    //json["heater3"] = 
    //json["profile"] = 

    String output;
    serializeJson(json, output);
    request->send(200, "application/json", output);
}

void handleGetConfig(AsyncWebServerRequest *request){
    StaticJsonDocument<JSON_BUF_SIZE> json;

    String output;
    serializeJson(json, output);
    request->send(200, "application/json", output);
}


void setup() {
  Serial.begin(115200);
  Serial.println("Serial Start");

  if (!initSPIFFS()) {
    return;
  }

  initWiFi();

  //server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
  //  request->send(200, "text/plain", "Hi! I am ESP32.");
  //});

  server.on("/status", HTTP_GET, handleStatus);
  server.on("/config", HTTP_GET, handleGetConfig);

  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  server.onNotFound(notFound);

  ElegantOTA.begin(&server);
  server.begin();
  Serial.println("HTTP server started.");

  clickEncoder.setAccelerationEnabled(false);
  clickEncoder.setDoubleClickEnabled(false);

  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000, true);
  timerAlarmEnable(timer);

  preferences.begin("reflowesp", false);

  gfx.init();
  gfx.setRotation(1);
  gfx.fillScreen(TFT_BLACK);
  gfx.setTextSize(2);
  Serial.println("Done");

  initPins();
  initPWM();
  initTherm();

  myBuzzer.init(BUZZ);
  myBuzzer.setMelody(&testMelody);

  delay(500);
}

void initMeter(int tc) {
  if(changeScreen == 0) return;

  gfx.fillScreen(TFT_BLACK);
  switch (tc) {
  case 1:
    tcMeter1.setZones(85, 100, 50, 85, 25, 50, 0, 25); // Example here red starts at 75% and ends at 100% of full scale
    tcMeter1.analogMeter(0, 0, 100.0, "C", "0", "75", "150", "225", "300"); // Draw analogue meter at 0, 0
    break;
  case 2:
    tcMeter2.setZones(85, 100, 50, 85, 25, 50, 0, 25); // Example here red starts at 75% and ends at 100% of full scale
    tcMeter2.analogMeter(0, 0, 100.0, "C", "0", "75", "150", "225", "300"); // Draw analogue meter at 0, 0
    break;
  default:
    break;
  }
  changeScreen = 0;
}

void initInfo() {
  if(changeScreen == 0) return;
  gfx.fillScreen(TFT_WHITE);

  sysinfo.addr = WiFi.localIP();
  sysinfo.wifi_ssid = WiFi.SSID();

  changeScreen = 0;
}

void runTCTest(int tc);
void runInfo();

/* -------------------------------------------------------------------------- */
/*                                  MAIN LOOP                                 */
/* -------------------------------------------------------------------------- */

void loop() {
  // Always poll thermocouples 
  tc1.poll();
  tc2.poll();

  constexpr int menuFPS = 1000 / 30;
  static unsigned long lastMenuFrame = -menuFPS;
  unsigned long now = millis();

  switch(exitMenuOptions) {
    // case 1: {
    //   delay (500);
    //   digitalWrite(BUZZ, HIGH);
    //   delay(buzzTimeOn * 1000);
    //   digitalWrite(BUZZ, LOW);
    //   exitMenuOptions = 0;
    // }
    case 1: 
      {
        runTCTest(1);
        break;
      }
    case 2: 
      {
        runTCTest(2);
        break;
      }
    case 3:
      {
        runInfo();
        break;
      }
    default:
      // Menu poll
      if (now - lastMenuFrame >= menuFPS) {
        lastMenuFrame = millis();
        nav.poll();
      }
  }

  pwmout.setLevel(pwmState);
  pwmout.poll();
}

/* -------------------------------------------------------------------------- */
/*                                 INFO SCREEN                                */
/* -------------------------------------------------------------------------- */

void runInfo() {
  delay(500);
  initInfo();

  gfx.setTextColor(TFT_BLACK, TFT_WHITE);
  gfx.drawString("ReflowESP info", 10, 10);

  //String str;
  //str = "IP address: " + sysinfo.addr;
  gfx.drawString("Address: " + sysinfo.addr.toString(), 10, 50);
  gfx.drawString("SSID: " + sysinfo.wifi_ssid, 10, 70);
  gfx.drawString("Profile: " + sysinfo.profile_title, 10, 90);

  switch (clickEncoder.getButton()) {
    case ClickEncoder::Button_e::Pressed:
    case ClickEncoder::Button_e::Clicked:
    case ClickEncoder::Button_e::DoubleClicked:
      {
        gfx.fillScreen(TFT_BLACK);
        exitMenuOptions = 0;   // Return to the menu
        mainMenu.dirty = true; // Force the main menu to redraw itself
        nav.refresh();
      }
    default:
        break;
  }

}

/* -------------------------------------------------------------------------- */
/*                                 TC TESTS                                   */
/* -------------------------------------------------------------------------- */

void runTCTest(int tc) {
  delay(500); // Pause to allow the button to come up

  initMeter(tc);

  float _temp = 0.0f;

  switch(tc) {
    case 1:
      {
        _temp = tc1.read();
        //tcMeter1.updateNeedle(_temp, 0);
        break;
      }
    case 2:
      {
        _temp = tc2.read();
        //tcMeter2.updateNeedle(_temp, 0);
        break;
      }
    default:
      return;
  }

  //gfx.drawFloat(_temp, 10, 100, 100);

  switch (clickEncoder.getButton()) {
    case ClickEncoder::Button_e::Pressed:
    case ClickEncoder::Button_e::Clicked:
    case ClickEncoder::Button_e::DoubleClicked:
      {
        gfx.fillScreen(TFT_BLACK);
        exitMenuOptions = 0;   // Return to the menu
        mainMenu.dirty = true; // Force the main menu to redraw itself
        nav.refresh();
      }
    default:
        break;
  }
}

/* -------------------------------------------------------------------------- */
/*                                 INTERRUPTS                                 */
/* -------------------------------------------------------------------------- */

void IRAM_ATTR onTimer() {
  clickEncoder.service();
}
