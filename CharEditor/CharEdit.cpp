#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <grx20.h>
#include "../Common.cpp"
#include "../keyboard.c"
#include "CharEditorApp.cpp"

unsigned _stklen = 1048576;  /* need a 1MB stack */

CharEditorApp *app = NULL;

int main()
{
    int bitDepth = 8;
    int screenWidth = 640;
    int screenHeight = 480;
    GrSetMode(GR_width_height_bpp_graphics,screenWidth,screenHeight,bitDepth);
    app = new CharEditorApp(screenWidth, screenHeight);
	app->EnableMouse();
	app->Start();

    delete app;
	return 0;

}
