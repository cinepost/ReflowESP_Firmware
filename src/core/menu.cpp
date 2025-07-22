#include "menu.h"
#include "../main.h"
#include "../config/Config.h"

// Menu library
#include <menu.h>

// Serial IO
#include <menuIO/chainStream.h>
#include <menuIO/serialIO.h>
#include <menuIO/serialIn.h>
#include <menuIO/serialOut.h>

// TFT
#include <TFT_eSPI.h>
#include <menuIO/TFT_eSPIOut.h>

// Encoder
#include <ClickEncoder.h>
#include <menuIO/clickEncoderIn.h>

using namespace Menu;

// TFT
TFT_eSPI gfx = TFT_eSPI();

// Encoder
ClickEncoder gClickEncoder = ClickEncoder(ENCA, ENCB, ENCBTN, ENCSTEPS, 1, INPUT, INPUT_PULLDOWN);
ClickEncoderStream gEncoderStream(gClickEncoder, 1);

int exitMenuOptions = 0;
int gCurrentScreen = SCREEN_ID_MENU;
int gPrevScreen = gCurrentScreen;

static result go_pwm() {
  delay(500);
  Serial.println("go_pwm");
  pwmout.setPeriod(pwmPeriodSec * 1000);
  return proceed;
}

static result go_tc1_test() {
  delay(500);
  gPrevScreen = gCurrentScreen;
  gCurrentScreen = SCREEN_ID_TCMT;
  exitMenuOptions = 1;
  return proceed;
}

static result go_tc2_test() {
  delay(500);
  gPrevScreen = gCurrentScreen;
  gCurrentScreen = SCREEN_ID_TCMT;
  exitMenuOptions = 2;
  return proceed;
}

static result go_info() {
  delay(500);
  gPrevScreen = gCurrentScreen;
  gCurrentScreen = SCREEN_ID_INFO;
  return proceed;
}

namespace reflow_esp {
  void restoreMenuScreen() {
    gCurrentScreen = SCREEN_ID_MENU;
    exitMenuOptions = 0;   // Return to the menu
    gMainMenu.dirty = true; // Force the main menu to redraw itself
    gfx.fillScreen(TFT_BLACK);
    gNav.refresh();
  }

  bool buttonClicked() {
    switch (gClickEncoder.getButton()) {
      case ClickEncoder::Button_e::Pressed:
      case ClickEncoder::Button_e::Clicked:
      case ClickEncoder::Button_e::DoubleClicked:
        return true;
  
      default:
        return false;
    }
  }
}

const colorDef<uint16_t> colors[6] MEMMODE = {
  //{{disabled normal,disabled selected},{enabled normal,enabled selected, enabled editing}}
  {{(uint16_t)Black, (uint16_t)Black}, {(uint16_t)Black, (uint16_t)Blue,   (uint16_t)Blue}}, //bgColor
  {{(uint16_t)White, (uint16_t)White},  {(uint16_t)White, (uint16_t)White, (uint16_t)White}},//fgColor
  {{(uint16_t)Red, (uint16_t)Red}, {(uint16_t)Yellow, (uint16_t)Yellow, (uint16_t)Yellow}}, //valColor
  {{(uint16_t)White, (uint16_t)White}, {(uint16_t)White, (uint16_t)White, (uint16_t)White}}, //unitColor
  {{(uint16_t)White, (uint16_t)Gray},  {(uint16_t)Black, (uint16_t)Blue,  (uint16_t)White}}, //cursorColor
  {{(uint16_t)White, (uint16_t)Yellow}, {(uint16_t)Black,  (uint16_t)Blue,   (uint16_t)Blue}}, //titleColor
};

MENU(tempMenu, "Temperature", doNothing, noEvent, wrapStyle,
  FIELD(pwmPeriodSec, "PWM Period", "s", 1, 60, 1, 1, go_pwm, exitEvent, noStyle),
  EXIT("< BACK")
)

MENU(settingsMenu, "Settings", doNothing, noEvent, wrapStyle,
  SUBMENU(tempMenu),
  EXIT("< Back")
)

MENU(testsMenu, "Tests", doNothing, noEvent, wrapStyle,
  OP("Test TC1", go_tc1_test, enterEvent),
  OP("Test TC2", go_tc2_test, enterEvent),
  EXIT("< Back")
)

MENU(gMainMenu, MACHINE_NAME, doNothing, noEvent, wrapStyle,
  OP("Info", go_info, enterEvent),
  SUBMENU(settingsMenu),
  SUBMENU(testsMenu),
  FIELD(pwmState, "PWM", "", 0, 1, 0.1, 0.1, doNothing, noEvent, noStyle)
);

serialIn serial(Serial);
MENU_INPUTS(in, &gEncoderStream, &serial);
MENU_OUTPUTS(out, MAX_DEPTH,
  TFT_eSPI_OUT(gfx, colors, FONT_WIDTH, FONT_HEIGHT, {0, 0, GFX_WIDTH / FONT_WIDTH, GFX_HEIGHT / FONT_HEIGHT}),
  SERIAL_OUT(Serial),
  NONE
);

NAVROOT(gNav, gMainMenu, MAX_DEPTH, in, out);
