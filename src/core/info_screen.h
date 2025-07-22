#ifndef CORE_INFO_SCREEN__H
#define CORE_INFO_SCREEN__H

#include <WiFi.h>

#include "menu.h"
#include "therm.h"
#include "base_screen.h"

namespace reflow_esp {

class InfoScreen: public BaseScreen {
  public:
    InfoScreen(TFT_eSPI* pTft, reflow_esp::Thermocouple* pTc1, reflow_esp::Thermocouple* pTc2);
    virtual void loopImpl();

  protected:
    struct SystemInfo {
        IPAddress addr;
        String wifi_ssid;
        String profile_title;
    };

    virtual void initScreenImpl();

    TFT_eSPI* mpTft;
    reflow_esp::Thermocouple* mpTc1;
    reflow_esp::Thermocouple* mpTc2;

    SystemInfo mInfo;
};

} // namespace reflow_esp

#endif // CORE_INFO_SCREEN__H