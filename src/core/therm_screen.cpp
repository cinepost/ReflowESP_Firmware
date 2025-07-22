#include "therm_screen.h"


namespace reflow_esp {

ThermScreen::ThermScreen(TFT_eSPI* pTft, reflow_esp::Thermocouple* pTc1, reflow_esp::Thermocouple* pTc2): BaseScreen(SCREEN_ID_TCMT) {
  mpTft = pTft;
  mpTc1 = pTc1;
  mpTc2 = pTc2;
  mpWidget = new MeterWidget(mpTft);
  mActiveTcNum = 0;
}

ThermScreen::~ThermScreen() {
  if(mpWidget) delete mpWidget;
}

void ThermScreen::loopImpl() {
  auto pTc = getActiveTC();
  float temp = pTc->read();
  mpWidget->updateNeedle(temp, 0);
  //mpTft->drawFloat(temp, 10, 100, 100);
}

void ThermScreen::initScreenImpl() {
  mpTft->fillScreen(TFT_BLACK);
  mpTft->setTextSize(1);
  mpWidget->setZones(85, 100, 70, 85, 35, 70, 0, 35);
  mpWidget->analogMeter(0, 0, 100.0, "C", "0", "75", "150", "225", "300"); // Draw analogue meter at 0, 0
}

reflow_esp::Thermocouple* ThermScreen::getActiveTC() {
  return mActiveTcNum == 1 ? mpTc1 : mpTc2;
}

void ThermScreen::setActiveTC(int tc_num) {
  if(mActiveTcNum == tc_num) return;

  mScreenActive = false; // re initialize screen
  if(tc_num < 2) {
    mActiveTcNum = 1;
  } else {
    mActiveTcNum = 2;
  } 
}

} // namespace reflow_esp
