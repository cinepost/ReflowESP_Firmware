#include <FloatToAscii.h>

#include "proc_screen.h"


namespace reflow_esp {

ProcScreen::ProcScreen(TFT_eSPI* pTft, reflow_esp::Thermocouple* pTc1, reflow_esp::Thermocouple* pTc2): BaseScreen(SCREEN_ID_INFO) {
  mpTft = pTft;
  mpTc1 = pTc1;
  mpTc2 = pTc2;

  mpPWM0 = NULL;
  mpPWM1 = NULL;
  mpPWM2 = NULL;
}

void ProcScreen::loopImpl() {
  uint32_t now = millis();
  if(now - mStartTime >= mTaskDuration) {
    shutdownHeaters();
    return;
  }

  if(isHeating()) {
    mpTft->fillSmoothCircle(310, 10, 10, TFT_RED, TFT_BLACK);
  } else {
    mpTft->drawRect(300, 0, 20, 20, TFT_BLACK);
  }

  mpTft->setTextColor(TFT_BLACK, TFT_WHITE);
  mpTft->drawString("ReflowESP info", 10, 10);

  float temp_avg = getTempAvg();
  
  mpTft->drawString(FloatToAscii(mTempAvgString, temp_avg, 1), 10, 90);
  mpTft->drawString(mpTc1 ? mpTc1->getTempString() : "N/A", 10, 110);
  mpTft->drawString(mpTc2 ? mpTc2->getTempString() : "N/A", 10, 130);
}

void ProcScreen::initScreenImpl() {
  mpTft->fillScreen(TFT_BLACK);
  mpTft->setTextSize(2);
  mpTft->setTextColor(TFT_WHITE, TFT_BLACK);
  mpTft->drawString("ReflowESP Oven Controller", 4, 4);

  mpTft->setTextSize(1);
  mpTft->drawString("Alloy:", 4, 44);
  mpTft->drawString("Profile:", 4, 64);
  mpTft->drawString("Time:", 4, 84);
  mpTft->drawString("Temp(C):", 4, 104);
  mpTft->drawString("TC1(C):", 4, 124);
  mpTft->drawString("TC2(C):", 4, 144);

  // turn on heaters

  // start elapsed time counting
  mStartTime = millis();
}

void ProcScreen::exitScreenImpl() {
    shutdownHeaters();
}

float ProcScreen::getTempAvg() {
  float div = 0.f;
  float temp = 0.f;
  if(mpTc1) {
    temp+=mpTc1->read();
    div+=1.0;
  }
  if(mpTc2) {
    temp+=mpTc2->read();
    div+=1.0;
  }
  return temp / div;
}

bool ProcScreen::isReady() {
    if(!mpTc1 && !mpTc2) return false;
    if(!mpPWM0 && !mpPWM1 && !mpPWM2) return false;
    //if(!mpProifile) return false;

    return true;
}

void ProcScreen::shutdownHeaters() {
    if(mpPWM0) mpPWM0->setLevel(0);
    if(mpPWM1) mpPWM1->setLevel(0);
    if(mpPWM2) mpPWM2->setLevel(0);
}

bool ProcScreen::isHeating() {
    if(mpPWM0 && mpPWM0->getLevel() > 0.0) return true;
    if(mpPWM1 && mpPWM2->getLevel() > 0.0) return true;
    if(mpPWM1 && mpPWM2->getLevel() > 0.0) return true;
    return false;
}

void ProcScreen::openDoor() {

}

} // namespace reflow_esp
