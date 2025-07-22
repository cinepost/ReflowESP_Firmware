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

// JSON
#include <ArduinoJson.h>

#include "core/pwm.h"
#include "core/therm.h"

#include "core/therm_screen.h"
#include "core/info_screen.h"
#include "core/proc_screen.h"

const char* ssid = "TarNet";
const char* password = "kirienko";

AsyncWebServer server(80);

// Menu
#include "core/menu.h"

// Preferences
Preferences preferences;

void IRAM_ATTR onTimer(); // Start the timer to read the clickEncoder every 1 ms

float pwmState = 0;
int pwmPeriodSec = 5;

hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

reflow_esp::PWMOutput pHeater1, pHeater2, pHeater3;
reflow_esp::PWMOutput pwmout;
reflow_esp::Thermocouple tc1, tc2;

Buzzer::Melody_t testMelody {
  .nbNotes = 3,
  .duration = {400, 400, 800},
  .frequency = {D4_NOTE_FREQ, E4_NOTE_FREQ, G4_NOTE_FREQ}
};

Buzzer myBuzzer;

reflow_esp::ThermScreen thermScreen(&gfx, &tc1, &tc2);
reflow_esp::InfoScreen infoScreen(&gfx, &tc1, &tc2);
reflow_esp::ProcScreen procScreen(&gfx, &tc1, &tc2);

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

  server.on("/status", HTTP_GET, handleStatus);
  server.on("/config", HTTP_GET, handleGetConfig);

  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  server.onNotFound(notFound);

  ElegantOTA.begin(&server);
  server.begin();
  Serial.println("HTTP server started.");

  gClickEncoder.setAccelerationEnabled(false);
  gClickEncoder.setDoubleClickEnabled(false);

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

  switch(gCurrentScreen) {
    case SCREEN_ID_TCMT: 
      {
        thermScreen.setActiveTC(exitMenuOptions);
        thermScreen.loop();
        break;
      }
    case SCREEN_ID_INFO:
      {
        infoScreen.loop();
        break;
      }
    case SCREEN_ID_MENU:
    default:
      // Menu poll
      if (now - lastMenuFrame >= menuFPS) {
        lastMenuFrame = millis();
        gNav.poll();
      }
  }

  pwmout.setLevel(pwmState);
  pwmout.poll();
}

/* -------------------------------------------------------------------------- */
/*                                 INTERRUPTS                                 */
/* -------------------------------------------------------------------------- */

void IRAM_ATTR onTimer() {
  gClickEncoder.service();
}
