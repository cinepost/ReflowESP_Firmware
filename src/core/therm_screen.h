#ifndef CORE_THERM_SCREEN__H
#define CORE_THERM_SCREEN__H

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <TFT_eWidget.h>

#include <ClickEncoder.h>
#include <menuIO/clickEncoderIn.h>

#include "menu.h"
#include "therm.h"


namespace reflow_esp {

class ThermScreen {
  public:
    ThermScreen(TFT_eSPI* pTft, reflow_esp::Thermocouple* pTc1, reflow_esp::Thermocouple* pTc2);
    ~ThermScreen();
    void loop(int tc_num);
    void exitScreen();

  protected:
    void initScreen(int tc_num);
    reflow_esp::Thermocouple* getTC(int tc_num);

    TFT_eSPI* mpTft;
    MeterWidget* mpWidget;
    reflow_esp::Thermocouple* mpTc1;
    reflow_esp::Thermocouple* mpTc2;

    uint32_t mLastUpdate{0};
    uint32_t mPeriod{500};

    bool mScreenActive{false};
};

} // namespace reflow_esp

#endif  // CORE_THERM_SCREEN__H