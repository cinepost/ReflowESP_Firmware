#ifndef CORE_MENU__H
#define CORE_MENU__H

#include <TFT_eSPI.h>
#include <ClickEncoder.h>
#include <menuIO/clickEncoderIn.h>

extern TFT_eSPI gfx;
extern ClickEncoder gClickEncoder;
extern ClickEncoderStream gEncoderStream;

extern Menu::menu gMainMenu;
extern Menu::navRoot gNav;

extern int exitMenuOptions;

extern int gPrevScreen;
extern int gCurrentScreen;

namespace reflow_esp {
    void restoreMenuScreen();

    bool buttonClicked();
}

/* --------------------------------- Colours -------------------------------- */

#define Black RGB565(0,0,0)
#define Red RGB565(255,0,0)
#define Green RGB565(0,255,0)
#define Blue RGB565(0,0,255)
#define Gray RGB565(128,128,128)
#define LighterRed RGB565(255,150,150)
#define LighterGreen RGB565(150,255,150)
#define LighterBlue RGB565(150,150,255)
#define LighterGray RGB565(211,211,211)
#define DarkerRed RGB565(150,0,0)
#define DarkerGreen RGB565(0,150,0)
#define DarkerBlue RGB565(0,0,150)
#define Cyan RGB565(0,255,255)
#define Magenta RGB565(255,0,255)
#define Yellow RGB565(255,255,0)
#define White RGB565(255,255,255)
#define DarkerOrange RGB565(255,140,0)

/* --- Screens ---*/

#define SCREEN_ID_MENU 0    // main menu
#define SCREEN_ID_TCMT 1    // tc meter screen
#define SCREEN_ID_INFO 2    // system information screen
#define SCREEN_ID_PROC 10   // reflow process monitor screen

#endif // CORE_MENU__H