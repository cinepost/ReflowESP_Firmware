#ifndef CORE_THERM_SCREEN__H
#define CORE_THERM_SCREEN__H

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <TFT_eWidget.h>

#include <ClickEncoder.h>
#include <menuIO/clickEncoderIn.h>

#include "menu.h"
#include "therm.h"
#include "base_screen.h"


namespace reflow_esp {

class ThermScreen: public BaseScreen {
  public:
    ThermScreen(TFT_eSPI* pTft, reflow_esp::Thermocouple* pTc1, reflow_esp::Thermocouple* pTc2);
    ~ThermScreen();

    void setActiveTC(int tc_num);
    void loopImpl();

  protected:
    void initScreenImpl();
    reflow_esp::Thermocouple* getActiveTC();

    TFT_eSPI* mpTft;
    MeterWidget* mpWidget;
    reflow_esp::Thermocouple* mpTc1;
    reflow_esp::Thermocouple* mpTc2;

    int mActiveTcNum{0};
};

} // namespace reflow_esp

#endif  // CORE_THERM_SCREEN__H