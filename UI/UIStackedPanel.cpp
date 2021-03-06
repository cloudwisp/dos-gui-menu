#ifndef UIStackedPanel_cpp
#define UIStackedPanel_cpp

#include <grx20.h>
#include "AppUI.h"
#include "UIDrawable.cpp"
#include "UIPanel.cpp"

//Container for child elements stacked vertically. The container will update coordinates of the child elements.
class UIStackedPanel : public UIDrawable {
private:
	GrColor backgroundColor;
	int tailY = 0;
	int innerPadding = 0;
	void draw_internal(){
		GrClearContextC(ctx, backgroundColor);
		if (innerContext){
			GrClearContextC(innerContext, THEME_COLOR_TRANSPARENT);
		}
	}
public:

	void OnEvent(EventEmitter *source, std::string event, EventData data){
		if (event == "Shown" || event == "Hidden"){
			ReFlow();
		}
		UIDrawable::OnEvent(source, event, data);
	}

	int GetTailY(){
		return tailY;
	}

	void AddChild(UIDrawable* subElement) override {
		UIDrawable::AddChild(subElement);
		subElement->BindEvent("Shown", this);
		subElement->BindEvent("Hidden", this);
		ReFlow();
		needsRedraw = true;
	}

	void RemoveChild(UIDrawable* subElement) override {
		UIDrawable::RemoveChild(subElement);
		subElement->UnbindAllEventsForConsumer(this);
		ReFlow();
		needsRedraw = true;
	}

	void ReFlow(){
		int thisY = 0;
		for (int i = 0; i < childCount; i++){
			UIDrawable* thisChild = children[i];
			if (!thisChild->visible){
				continue;
			}
			thisChild->SetPosition(0, thisY);
			thisY+=thisChild->height + innerPadding;
		}
		tailY = thisY;
		needsRedraw = true;
	}

	UIStackedPanel(GrColor bgColor, int drawWidth, int drawHeight) : UIDrawable(drawWidth, drawHeight) {
		backgroundColor = bgColor;
	}
	UIStackedPanel(GrColor bgColor, int drawWidth, int drawHeight, int padding) : UIDrawable(drawWidth, drawHeight, padding){
		backgroundColor = bgColor;
	}
	UIStackedPanel(GrColor bgColor, int drawWidth, int drawHeight, int padding, int inlinePadding): UIDrawable(drawWidth, drawHeight, padding){
		backgroundColor = bgColor;
		innerPadding = inlinePadding;
	}
};

#endif