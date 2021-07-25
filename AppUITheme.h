#ifndef AppUITheme_h
#define AppUITheme_h

#include <grx20.h>

struct CWRGB {
    UINT8 r;
    UINT8 g;
    UINT8 b;
};

const int THEME_WINDOW_TITLE_HEIGHT = 20;
const int THEME_WINDOW_BORDER_WIDTH = 2;
CWRGB THEME_WINDOW_BORDER_COLOR = {0xFF,0xFF,0xFF};
CWRGB THEME_WINDOW_BACKGROUND_COLOR = {0xCC,0xCC,0xCC};
CWRGB THEME_WINDOW_TITLE_BACKGROUND_COLOR = {0x10,0x10,0x10};
CWRGB THEME_WINDOW_TITLE_TEXT_COLOR = {0xFF,0xFF,0xFF};
char* THEME_WINDOW_TITLE_FONT = "GrFont_PC6x8";

GrColor ColorFromRGB(CWRGB rgb){
    return GrAllocColor(rgb.r,rgb.g,rgb.b);
}

#endif