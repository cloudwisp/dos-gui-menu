#ifndef UIHelpers_cpp
#define UIHelpers_cpp

#include <grx20.h>
#include "AppUI.h"

class UIHelpers {
public:
	static GrColor ColorFromRGB(CWRGB rgb);
    static GrFont* ResolveFont(char* fontName);
    static int CharHeight(char* fontName);
};

GrColor UIHelpers::ColorFromRGB(CWRGB rgb){
	return GrAllocColor(rgb.r,rgb.g,rgb.b);
}


GrFont* UIHelpers::ResolveFont(char* fontName){
    if (fontName == "GrFont_PC6x8"){
        return &GrFont_PC6x8;
    }
    if (fontName == "GrFont_PC8x8"){
        return &GrFont_PC8x8;
    }
    if (fontName == "GrFont_PC8x14"){
        return &GrFont_PC8x14;
    }
    if (fontName == "GrFont_PC8x16"){
        return &GrFont_PC8x16;
    }
    GrFont* resolved = GrLoadFont(fontName);
    if (resolved == NULL){
        return &GrDefaultFont;
    }
}

int UIHelpers::CharHeight(char* fontName){
    return GrFontCharHeight(ResolveFont(fontName), "A");
}

#endif