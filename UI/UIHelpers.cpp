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

void Draw3dButton(GrContext *ontoContext, BoxCoords coords, GrColor bgColor, bool invert){
    GrContext *prevCtx = GrCurrentContext();
    GrSetContext(ontoContext);
    int highlight[3][2] = {
        {coords.x1, coords.y2},
        {coords.x1, coords.y1},
        {coords.x2, coords.y1}
    };
    int shadow[3][2] = {
        {coords.x2, coords.y1},
        {coords.x2, coords.y2},
        {coords.x1, coords.y2}
    };

    GrPolyLine(3, invert ? shadow : highlight, THEME_3D_HIGHLIGHT);
    GrPolyLine(3, invert? highlight : shadow, THEME_3D_SHADOW);
    GrSetContext(prevCtx);
}

bool BoxesIntersect(BoxCoords coord1, BoxCoords coord2){
    return !(coord1.x2 < coord2.x1
    || coord1.y2 < coord2.y1
    || coord1.x1 > coord2.x2
    || coord1.y1 > coord2.y2);
}

BoxCoords BoxIntersection(BoxCoords coord1, BoxCoords coord2){
    if (!BoxesIntersect(coord1,coord2)){
        return BoxCoords {0,0,0,0};
    }

    return BoxCoords {
        coord1.x1 < coord2.x1 ? coord2.x1 : coord1.x1,
        coord1.y1 < coord2.y1 ? coord2.y1 : coord1.y1,
        coord1.x2 < coord2.x2 ? coord1.x2 : coord2.x2,
        coord1.y2 < coord2.y2 ? coord1.y2 : coord2.y2
    };
}


#endif