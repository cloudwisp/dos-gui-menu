#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <grx20.h>
#include "keyboard.c"
#include "SelApp.cpp"

unsigned _stklen = 1048576;  /* need a 1MB stack */

SelectorApplication *app = NULL;

int main(int argc, char *argv[])
{   
    memCounterStart();
    MenuConfig* config = AppResources::GetMenuConfig();
    std::string resolution = config->resolution;

    if (argc > 1){
        resolution = std::string(argv[1]);
    }
    int screenWidth = 320;
    int screenHeight = 200;
    if (resolution == "high"){
        screenWidth = 640;
        screenHeight = 480;
    }
    int bitDepth = 8;
    GrSetMode(GR_width_height_bpp_graphics,screenWidth,screenHeight,bitDepth);
    std::string fontDir = std::string(currentDir());
    
    if (fontDir.at(fontDir.size() - 1) != 0x5C){
        fontDir.append("\\");
    }
    fontDir.append("fonts");
    
    GrSetFontPath((char*)fontDir.c_str());
    SetupThemeColors();
    app = new SelectorApplication(screenWidth, screenHeight);
	app->EnableMouse(false);
    app->ToggleDiagnostics(config->diagnosticOverlay);
	app->Start();
    AppResources::Destroy();

    delete app;
	return 0;

}
