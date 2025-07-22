#include "menu.h"

#include "base_screen.h"


namespace reflow_esp {

BaseScreen::BaseScreen(int screen_num): mScreenNum(screen_num) {

}

void BaseScreen::loop() {
  if(gCurrentScreen != mScreenNum) return;

  uint32_t now = millis();
  if (now - mLastUpdate >= mPeriod) {
    if(buttonClicked()) {
      exitScreen();
      return;
    }

    initScreen();

    loopImpl();

    mLastUpdate = millis();
  }
}

void BaseScreen::initScreen() {
  if(mScreenActive) return;
  initScreenImpl();
  mScreenActive = true;
}

void BaseScreen::exitScreen() {
  if(gCurrentScreen != mScreenNum) return;
  exitScreenImpl();
  mScreenActive = false;
  restoreMenuScreen();
}

} // namespace reflow_esp
