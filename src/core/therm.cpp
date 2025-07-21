#include "therm.h"
//#include <max6675.h>

static const float sLowTempThresh = 10.0f;

namespace therm {

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
  if (now - this->last_update >= this->period) {
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
  }
  this->last_update = millis();
}

bool Thermocouple::isValid() {
  return mErrCode == MAX31855_ERROR_NONE;
}

} // namespace therm
