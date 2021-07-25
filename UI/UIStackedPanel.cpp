#ifndef UIStackedPanel_cpp
#define UIStackedPanel_cpp

#include <grx20.h>
#include "AppUI.h"
#include "UIDrawable.cpp"

//Container for child elements stacked vertically. The container will update coordinates of the child elements.
class UIStackedPanel : public UIDrawable {
private:
	GrColor backgroundColor;
	int tailY = 0;
	void draw_internal(){
		GrClearContextC(ctx, backgroundColor);
	}
public:

	void AddChild(UIDrawable* subElement) override {
		subElement->y = tailY;
		subElement->x = 0;
		tailY += subElement->height;
		UIDrawable::AddChild(subElement);
	}

	void RemoveChild(UIDrawable* subElement) override {
		int thisY = subElement->y;
		int subY = subElement->height;
		for (int i = 0; i < childCount; i++){
			if (children[i]->y > thisY){
				children[i]->y -= subY;
			}
		}
		UIDrawable::RemoveChild(subElement);
	}

	UIStackedPanel(GrColor bgColor, int drawWidth, int drawHeight) : UIDrawable(drawWidth, drawHeight) {
		backgroundColor = bgColor;
	}
};

#endif