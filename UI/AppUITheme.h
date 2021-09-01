#ifndef AppUITheme_h
#define AppUITheme_h

#include <grx20.h>
#include <string>
#include "AppUI.h"
#include "../Common.cpp"

int blitCount = 0;
void GrBitBltCount(GrContext* dst, int x, int y, GrContext* src, int x1, int y1, int x2, int y2, GrColor op){
    GrBitBlt(dst, x, y, src, x1, y1, x2, y2, op);
    blitCount++;
}

void LogBlitCount(){
    char* buffer = (char*) malloc(sizeof(char) * 1000);
    sprintf(buffer, "Blit Count: %d", blitCount);
    debugOut(std::string(buffer));
    free(buffer);
}

const int THEME_WINDOW_TITLE_HEIGHT = 20;
const int THEME_WINDOW_BORDER_WIDTH = 2;
char* THEME_WINDOW_TITLE_FONT = "helv11"; // "GrFont_PC6x8";
char* THEME_DEFAULT_FONT = "helv11";

/*
    Theme colors
*/
//defines for reference names
static GrColor themecolors[24];
#define THEME_WINDOW_BORDER_COLOR           themecolors[0]
#define THEME_WINDOW_BACKGROUND_COLOR       themecolors[1]
#define THEME_WINDOW_TEXT_COLOR             themecolors[2]
#define THEME_WINDOW_TITLE_BACKGROUND_COLOR themecolors[3]
#define THEME_WINDOW_TITLE_TEXT_COLOR       themecolors[4]
#define THEME_BUTTON_BACKGROUND_PRIMARY     themecolors[5]
#define THEME_BUTTON_TEXT_PRIMARY           themecolors[6]
#define THEME_BUTTON_BACKGROUND_SECONDARY   themecolors[7]
#define THEME_BUTTON_TEXT_SECONDARY         themecolors[8]
#define THEME_HIGHLIGHT_BORDER              themecolors[9]
#define THEME_PANEL_BACKGROUND_MENU         themecolors[10]
#define THEME_PANEL_TEXT_MENU               themecolors[11]
#define THEME_PANEL_BACKGROUND_PRIMARY      themecolors[12]
#define THEME_PANEL_TEXT_PRIMARY            themecolors[13]
#define THEME_PANEL_BACKGROUND_SECONDARY    themecolors[14]
#define THEME_PANEL_TEXT_SECONDARY          themecolors[15]
#define THEME_3D_HIGHLIGHT                  themecolors[16]
#define THEME_3D_SHADOW                     themecolors[17]
#define THEME_HIGHLIGHT_BACKGROUND          themecolors[18]
#define THEME_HIGHLIGHT_TEXT                themecolors[19]
#define THEME_CONTROL_BACKGROUND            themecolors[20]
#define THEME_CONTROL_TEXT                  themecolors[21]
#define THEME_COLOR_BLACK                   themecolors[22]
#define THEME_COLOR_TRANSPARENT             themecolors[23]

//actual colors - index must align with above list
CWRGB sourcecolors[24] = {
    //THEME_WINDOW_BORDER_COLOR
    {0xFF,0xFF,0xFF},
    //THEME_WINDOW_BACKGROUND_COLOR
    {0xCC,0xCC,0xCC},
    //THEME_WINDOW_TEXT_COLOR
    {0x10,0x10,0x10},
    //THEME_WINDOW_TITLE_BACKGROUND_COLOR
    {0x33,0x33,0x33},
    //THEME_WINDOW_TITLE_TEXT_COLOR
    {0xCA,0xCA,0xCA},
    //THEME_BUTTON_BACKGROUND_PRIMARY
    {0x10,0x10,0x10},
    //THEME_BUTTON_TEXT_PRIMARY
    {0xFF,0xFF,0xFF},
    //THEME_BUTTON_BACKGROUND_SECONDARY
    {0x10,0x10,0x10},
    //THEME_BUTTON_TEXT_SECONDARY
    {0xFF,0xFF,0xFF},
    //THEME_HIGHLIGHT_BORDER
    {0xFF,0xF7,0x00},
    //THEME_PANEL_BACKGROUND_MENU
    {0xDA,0xDA,0xDA},
    //THEME_PANEL_TEXT_MENU
    {0xFF,0xFF,0xFF},
    //THEME_PANEL_BACKGROUND_PRIMARY
    {0x10,0x10,0x10},
    //THEME_PANEL_TEXT_PRIMARY
    {0xFF,0xFF,0xFF},
    //THEME_PANEL_BACKGROUND_SECONDARY
    {0x10,0x10,0x10},
    //THEME_PANEL_TEXT_SECONDARY
    {0xFF,0xFF,0xFF},
    //THEME_BUTTON_3D_HIGHLIGHT
    {0xEB,0xEB,0xEB},
    //THEME_BUTTON_3D_SHADOW
    {0x64,0x64,0x64},
    //THEME_HIGHLIGHT_BACKGROUND
    {0xFF,0xFF,0xFF},
    //THEME_HIGHLIGHT_TEXT
    {0x10,0x10,0x10},
    //THEME_CONTROL_BACKGROUND
    {0xCC,0xCC,0xCC},
    //THEME_CONTROL_TEXT
    {0x10,0x10,0x10},
    //THEME_COLOR_BLACK
    {0x10,0x10,0x10}, //cannot be true black, since that's the transparency color
    //THEME_COLOR_TRANSPARENT
    {0x0,0x0,0x0}
};

//Actual GrColor values only be called after the graphics mode is set.
void SetupThemeColors(){
    int i;
    for (i=0; i < 24; ++i)
        themecolors[i] = GrAllocColor(sourcecolors[i].r,sourcecolors[i].g,sourcecolors[i].b);
}

void ResetColors(){
    GrResetColors();
    SetupThemeColors();
}

#endif