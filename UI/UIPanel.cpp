#ifndef UIPanel_cpp
#define UIPanel_cpp

#include <grx20.h>
#include "AppUI.h"
#include "UIDrawable.cpp"

class UIPanel : public UIDrawable {
private:
	GrColor backgroundColor;
	void draw_internal(){
		GrClearContextC(ctx, backgroundColor);
	}
public:

	UIPanel(GrColor bgColor, int drawWidth, int drawHeight) : UIDrawable(drawWidth, drawHeight) {
		backgroundColor = bgColor;
	}

	UIPanel(GrColor bgColor, int drawWidth, int drawHeight, int padding) : UIDrawable(drawWidth, drawHeight, padding){
		innerContextX = padding;
		innerContextY = padding;
		backgroundColor = bgColor;
	}
};

#endif