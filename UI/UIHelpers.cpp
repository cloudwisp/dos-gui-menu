#ifndef UIHelpers_cpp
#define UIHelpers_cpp

#include <grx20.h>
#include "AppUI.h"

class UIHelpers {
public:
	static GrColor ColorFromRGB(CWRGB rgb);
};

GrColor UIHelpers::ColorFromRGB(CWRGB rgb){
	return GrAllocColor(rgb.r,rgb.g,rgb.b);
}

#endif