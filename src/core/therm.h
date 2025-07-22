#pragma once
#include <Arduino.h>
#include "../config/Config.h"

#ifdef HAS_MAX6675
#include <max6675.h>
#else
#include <MAX31855.h>
#endif

namespace reflow_esp {

class Thermocouple {
 public:
  void setPins(uint8_t miso, uint8_t sck, uint8_t cs);
  void setPeriod(uint32_t period) { mPeriod = period; }
  float read() { return mCurrentTemp; }
  String getTempString();
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
  uint32_t mLastUpdate{0};
  uint32_t mPeriod{500};
  String mErrString;
  String mTempString;  // variable to avoid memory fragmentation
};

} // namespace reflow_esp
