#ifndef CORE_PWM__H
#define CORE_PWM__H

#include <Arduino.h>

namespace reflow_esp {

class PWMOutput {
 public:
  void setPin(uint8_t pin) { mPin = pin; }
  void setPeriod(uint32_t period) { mPeriod= period; }
  void setLevel(float state) { mState = state; }
  float getLevel() { return mState; }
  void poll() { loop(); }
 protected:
  void loop();
  void setOutputState(bool state);
  uint8_t mPin;
  uint32_t mPeriod{500};
  uint32_t mPeriodStartTime{0};
  float mState{0};
  bool mCurrentState{false};
};

} // namespace reflow_esp

#endif // CORE_PWM__H