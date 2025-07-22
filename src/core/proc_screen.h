#ifndef CORE_PROC_SCREEN__H
#define CORE_PROC_SCREEN__H

#include <WiFi.h>

#include "menu.h"
#include "therm.h"
#include "pwm.h"
#include "base_screen.h"

namespace reflow_esp {

class ProcScreen: public BaseScreen {
  public:
    ProcScreen(TFT_eSPI* pTft, reflow_esp::Thermocouple* pTc1, reflow_esp::Thermocouple* pTc2);
    virtual void loopImpl();

    void attachPWM(uint32_t id, PWMOutput* pPWM);

  protected:
    virtual void initScreenImpl();
    virtual void exitScreenImpl() override;

    float getTempAvg();

    void shutdownHeaters();
    void openDoor();
    bool isReady();
    bool isHeating();

    TFT_eSPI* mpTft;
    reflow_esp::Thermocouple* mpTc1;
    reflow_esp::Thermocouple* mpTc2;

    uint32_t mStartTime;
    uint32_t mTaskDuration{5000};

    PWMOutput* mpPWM0;
    PWMOutput* mpPWM1;
    PWMOutput* mpPWM2;

    String mTempAvgString;  // variable to avoid memory fragmentation
};

} // namespace reflow_esp

#endif // CORE_PROC_SCREEN__H