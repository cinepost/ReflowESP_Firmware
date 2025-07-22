#include <FloatToAscii.h>

#include "therm.h"


namespace reflow_esp {

void Thermocouple::setPins(uint8_t miso, uint8_t sck, uint8_t cs) {
  #ifdef HAS_MAX6675
  pTc = new MAX6675(sck, cs, miso);
  #else
  pTc = new MAX31855(sck, cs, miso);
  pTc->begin();
  #endif
}

void Thermocouple::loop() {
  uint32_t now = millis();
  if (now - mLastUpdate >= mPeriod) {
    #ifdef HAS_MAX6675
    mCurrentTemp = pTc->readCelsius();
    #else
    mErrCode = pTc->getErrorCode();

    if(mErrCode != MAX31855_ERROR_NONE) {
      mCurrentTemp = 0.0f;
      mErrString = pTc->getErrorString();
    } else {
      mCurrentTemp = pTc->getTemperature();
    }
    #endif
  
    mLastUpdate = millis();
  }
}

String Thermocouple::getTempString() {
  if(!isValid()) return pTc->getErrorString();

  return FloatToAscii(mTempString, mCurrentTemp, 2);
}

bool Thermocouple::isValid() {
  return mErrCode == MAX31855_ERROR_NONE;
}

} // namespace reflow_esp
