#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <allegro.h>
#include <grx20.h>
#include "keyboard.c"
#include "SelApp.cpp"

unsigned _stklen = 1048576;  /* need a 1MB stack */

SelectorApplication *app = NULL;

int main()
{

    allegro_init();
    int bitDepth = 16;
    int screenWidth = 640;
    int screenHeight = 480;
    GrSetMode(GR_width_height_bpp_graphics,screenWidth,screenHeight,bitDepth);

    install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, "");
    app = new SelectorApplication(screenWidth, screenHeight);
	app->EnableMouse();
	app->Start();
	allegro_exit();

    AppResources::Destroy();

    delete app;
	return 0;

}
END_OF_MAIN()
