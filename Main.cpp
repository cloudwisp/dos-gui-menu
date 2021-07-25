#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <allegro.h>
#include <grx20.h>
#include "keyboard.c"
#include "GameApp.cpp"
#include "GameChar.cpp"
#include "GameWrld.h"

unsigned _stklen = 1048576;  /* need a 1MB stack */

GameApplication *app = NULL;

int main()
{

    allegro_init();
    int bitDepth = 8;
    int screenWidth = 320;
    int screenHeight = 200;
    GrSetMode(GR_width_height_bpp_graphics,screenWidth,screenHeight,bitDepth);

    install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, "");
    app = new GameApplication(screenWidth, screenHeight, bitDepth, 320, 200, 0, 0, WORLD_LEVEL1);
	app->EnableMouse();
	app->Start();
	allegro_exit();

    GameResources::Destroy();

    delete app;
	return 0;

}
END_OF_MAIN()
