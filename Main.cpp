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
    int bitDepth = 8;
    int screenWidth = 640;
    int screenHeight = 480;
    GrSetMode(GR_width_height_bpp_graphics,screenWidth,screenHeight,bitDepth);
    SetupThemeColors();
    app = new SelectorApplication(screenWidth, screenHeight);
	app->EnableMouse(false);
	app->Start();

    AppResources::Destroy();

    delete app;
	return 0;

}
