#ifndef CORE_BASE_SCREEN__H
#define CORE_BASE_SCREEN__H

#include <WiFi.h>

#include "menu.h"
#include "therm.h"


namespace reflow_esp {

class BaseScreen {
  public:
    BaseScreen(int screen_num);
    void loop();
    virtual void loopImpl() = 0;

    void exitScreen();
    void setPeriod(uint32_t period) { mPeriod = period; }

  protected:
    void initScreen();
    virtual void initScreenImpl() = 0;

    uint32_t mPeriod{500};
    uint32_t mLastUpdate{0};

    bool mScreenActive{false};

  private:
    int mScreenNum{-1};
};

} // namespace reflow_esp

#endif // CORE_BASE_SCREEN__H