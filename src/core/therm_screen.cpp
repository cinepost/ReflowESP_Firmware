#include "therm_screen.h"


namespace reflow_esp {

ThermScreen::ThermScreen(TFT_eSPI* pTft, reflow_esp::Thermocouple* pTc1, reflow_esp::Thermocouple* pTc2) {
  mpTft = pTft;
  mpTc1 = pTc1;
  mpTc2 = pTc2;
  mpWidget = new MeterWidget(mpTft);
}

ThermScreen::~ThermScreen() {
  if(mpWidget) delete mpWidget;
}

void ThermScreen::loop(int tc_num) {
  if(gCurrentScreen != SCREEN_ID_TCMT) return;

  uint32_t now = millis();
  if (now - mLastUpdate >= mPeriod) {

    auto pTc = getTC(tc_num);
    if(!pTc || buttonClicked()) {
      exitScreen();
      return;
    }

    initScreen(tc_num);
    float temp = pTc->read();
    mpWidget->updateNeedle(temp, 0);
    //mpTft->drawFloat(temp, 10, 100, 100);
  
    mLastUpdate = millis();
  }
}

void ThermScreen::initScreen(int tc_num) {
  if(mScreenActive) return;
  mpTft->fillScreen(TFT_BLACK);
  mpWidget->setZones(85, 100, 70, 85, 35, 70, 0, 35);
  mpWidget->analogMeter(0, 0, 100.0, "C", "0", "75", "150", "225", "300"); // Draw analogue meter at 0, 0
  mScreenActive = true;
}

void ThermScreen::exitScreen() {
  if(gCurrentScreen != SCREEN_ID_TCMT) return;
  mScreenActive = false;
  restoreMenuScreen();
}

reflow_esp::Thermocouple* ThermScreen::getTC(int tc_num) {
  if(tc_num < 1 || tc_num > 2) return nullptr;

  return tc_num == 1 ? mpTc1 : mpTc2;
}

} // namespace reflow_esp
