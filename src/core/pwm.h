#pragma once
#include <Arduino.h>

namespace pwm {

class PWMOutput {
 public:
  void setPin(uint8_t pin) { pin_ = pin; }
  void setPeriod(uint32_t period) { period_ = period; }
  void setLevel(float state) { state_ = state; }
  void poll() { loop(); }
 protected:
  void loop();
  void setOutputState(bool state);
  uint8_t pin_;
  uint32_t period_;
  uint32_t period_start_time_{0};
  float state_{0};
  bool current_state_{false};
};

} // namespace pwm
