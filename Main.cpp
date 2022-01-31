#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <grx20.h>
#include "keyboard.c"
#include "SelApp.cpp"

unsigned _stklen = 1048576;  /* need a 1MB stack */

SelectorApplication *app = NULL;

int main()
{
    MenuConfig* config = AppResources::GetMenuConfig();
    int screenWidth = 320;
    int screenHeight = 200;
    if (config->resolution == "high"){
        screenWidth = 640;
        screenHeight = 480;
    }
    int bitDepth = 8;
    GrSetMode(GR_width_height_bpp_graphics,screenWidth,screenHeight,bitDepth);
    SetupThemeColors();
    app = new SelectorApplication(screenWidth, screenHeight);
	app->EnableMouse(false);
    app->ToggleDiagnostics(false);
	app->Start();
    AppResources::Destroy();

    delete app;
	return 0;

}
