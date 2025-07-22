#ifndef CORE_INFO_SCREEN__H
#define CORE_INFO_SCREEN__H

#include <WiFi.h>

#include "menu.h"
#include "therm.h"


namespace reflow_esp {

class InfoScreen {
  public:
    InfoScreen(TFT_eSPI* pTft, reflow_esp::Thermocouple* pTc1, reflow_esp::Thermocouple* pTc2);
    void loop();
    void exitScreen();

  protected:
    struct SystemInfo {
        IPAddress addr;
        String wifi_ssid;
        String profile_title;
    };

    void initScreen();

    TFT_eSPI* mpTft;
    reflow_esp::Thermocouple* mpTc1;
    reflow_esp::Thermocouple* mpTc2;

    uint32_t mLastUpdate{0};
    uint32_t mPeriod{500};

    bool mScreenActive{false};

    SystemInfo mInfo;
};

} // namespace reflow_esp

#endif // CORE_INFO_SCREEN__H