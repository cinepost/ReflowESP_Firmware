#pragma once

#include "core/pwm.h"
#include "core/therm.h"

#define JSON_BUF_SIZE 1024

extern int pwmPeriodSec;

extern float pwmState;

extern reflow_esp::PWMOutput pwmout;
