#include "pwm.h"

#include <Arduino.h>

namespace reflow_esp {

void PWMOutput::setOutputState(bool new_state) {
  if (new_state == this->mCurrentState) return;
  
  mCurrentState = new_state;
  digitalWrite(mPin, new_state);
  Serial.printf("Setting state %s\n", new_state ? "on" : "off");
}

void PWMOutput::loop() {
  uint32_t now = millis();
  float scaled_state = mState * mPeriod;

  setOutputState(scaled_state > now - mPeriodStartTime);

  if (now - mPeriodStartTime >= mPeriod) {
    mPeriodStartTime += mPeriod;
  }
}

} // namespace reflow_esp
