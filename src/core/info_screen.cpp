#include <WiFi.h>

#include "info_screen.h"


namespace reflow_esp {

InfoScreen::InfoScreen(TFT_eSPI* pTft, reflow_esp::Thermocouple* pTc1, reflow_esp::Thermocouple* pTc2): BaseScreen(SCREEN_ID_INFO) {
  mpTft = pTft;
  mpTc1 = pTc1;
  mpTc2 = pTc2;
}

void InfoScreen::loopImpl() {
  mpTft->setTextColor(TFT_BLACK, TFT_WHITE);
  mpTft->drawString("ReflowESP info", 10, 10);

  mpTft->drawString("Address: " + mInfo.addr.toString(), 10, 50);
  mpTft->drawString("SSID: " + mInfo.wifi_ssid, 10, 70);
  mpTft->drawString("Profile: " + mInfo.profile_title, 10, 90);
  
  mpTft->drawString("TC1: " + (mpTc1 ? mpTc1->getTempString() : "N/A"), 10, 110);
  mpTft->drawString("TC2: " + (mpTc2 ? mpTc2->getTempString() : "N/A"), 10, 130);
}

void InfoScreen::initScreenImpl() {
  mpTft->fillScreen(TFT_WHITE);

  mInfo.addr = WiFi.localIP();
  mInfo.wifi_ssid = WiFi.SSID();

  mInfo.profile_title = "N/A";
}


} // namespace reflow_esp
