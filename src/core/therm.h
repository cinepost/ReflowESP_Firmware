#pragma once
#include <Arduino.h>
#include "../config/Config.h"

#ifdef HAS_MAX6675
#include <max6675.h>
#else
#include <MAX31855.h>
#endif

namespace therm {

class Thermocouple {
 public:
  void setPins(uint8_t miso, uint8_t sck, uint8_t cs);
  void setPeriod(uint32_t period) { this->period = period; }
  float read() { return mCurrentTemp; }
  void poll() { loop(); }
  bool isValid();
  const String& getErrString() const { return mErrString; }
 protected:
  void loop();
  uint8_t pin_miso_;
  uint8_t pin_sck_;
  uint8_t pin_cs_;
  float mCurrentTemp;

#ifdef HAS_MAX6675
  MAX6675* pTc;
#else
  MAX31855* pTc;
#endif

  int mErrCode{MAX31855_NOT_INITIALIZED};
  uint32_t last_update{0};
  uint32_t period{500};
  String mErrString;
};

} // namespace therm
