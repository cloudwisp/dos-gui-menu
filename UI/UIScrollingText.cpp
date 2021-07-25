#ifndef UIScrollingText_cpp
#define UIScrollingText_cpp

#include <grx20.h>
#include "AppUI.h"
#include "UIDrawable.cpp"
#include "UITextArea.cpp"

class UIScrollingText : public UIDrawable {
private:
	UITextArea* _innerText;
	void draw_internal(){
		GrClearContextC(ctx, GrAllocColor(0,0,0));
		_innerText->y = 0-ScrollTop;
	}
	
	int ScrollTop = 0;
public:

	bool ScrollDown(){
		int newScroll = ScrollTop+1;
		if (newScroll > _innerText->height - height){
			return false;
		}
		ScrollTop++;
		return true;
	}

	bool ScrollUp(){
		int newScroll = ScrollTop - 1;
		if (newScroll < 0){
			return false;
		}
		ScrollTop--;
		return true;
	}

	UIScrollingText(UITextArea* innerText, int height) : UIDrawable(innerText->width, height){
		_innerText = innerText;
		AddChild(_innerText);
	}
};

#endif