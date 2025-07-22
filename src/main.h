#pragma once

#include "core/pwm.h"
#include "core/therm.h"

#define JSON_BUF_SIZE 1024

extern int pwmPeriodSec;

extern float pwmState;

extern pwm::PWMOutput pwmout;
extern reflow_esp::Thermocouple tc1;
extern reflow_esp::Thermocouple tc2;
