#ifndef UIScrollingText_cpp
#define UIScrollingText_cpp

#include <grx20.h>
#include "AppUI.h"
#include "UIDrawable.cpp"
#include "UITextArea.cpp"

class UIScrollingText : public UIDrawable {
private:
	UITextArea* _innerText;
	// void draw_internal(){
	// 	GrClearContextC(ctx, THEME_COLOR_TRANSPARENT);
	// 	_innerText->y = 0-ScrollTop;
	// }
	
    BoxCoords yBar;
    BoxCoords yBarButtonUp;
    BoxCoords yBarButtonDown;
    BoxCoords yBarScrollPos;
	BoxCoords yBarSpaceAbove;
	BoxCoords yBarSpaceBelow;
    int scrollArrowPad = 4;
	bool scrollBarVisible = false;

    void draw_internal(){
		
        GrClearContextC(ctx, THEME_COLOR_TRANSPARENT);
        GrClearContextC(innerContext, THEME_COLOR_TRANSPARENT);
        GrSetContext(ctx);
		if (scrollBarVisible){
			//whole scrollbar bg
            Draw3dButton(ctx, yBar, THEME_COLOR_SCROLLBAR_BACKGROUND, true);

            //up button
            Draw3dButton(ctx, yBarButtonUp, THEME_COLOR_SCROLLBAR_BUTTON_BG, false);
            
            //up arrow shape
            int poly[3][2] = {
                {yBarButtonUp.x1 + (THEME_SCROLLBAR_WIDTH / 2), yBarButtonUp.y1 + scrollArrowPad},
                {yBarButtonUp.x1 + scrollArrowPad, yBarButtonUp.y2 - scrollArrowPad},
                {yBarButtonUp.x2 - scrollArrowPad, yBarButtonUp.y2 - scrollArrowPad}
            };
            GrFilledPolygon(3,poly, THEME_COLOR_SCROLLBAR_BUTTON_FG);

            //down button
            Draw3dButton(ctx, yBarButtonDown, THEME_COLOR_SCROLLBAR_BUTTON_BG, false);

            //down arrow shape
            int dnpoly[3][2] = {
                {yBarButtonDown.x1 + scrollArrowPad, yBarButtonDown.y1 + scrollArrowPad},
                {yBarButtonDown.x1 + (THEME_SCROLLBAR_WIDTH / 2), yBarButtonDown.y2 - scrollArrowPad},
                {yBarButtonDown.x2 - scrollArrowPad, yBarButtonDown.y1 + scrollArrowPad}                
            };
            GrFilledPolygon(3,dnpoly, THEME_COLOR_SCROLLBAR_BUTTON_FG);

            //scroll position block.
            Draw3dButton(ctx, yBarScrollPos, THEME_COLOR_SCROLLBAR_BUTTON_BG, false);
		} 
    }

	double yScrollPercent = 0;
	int innerScrollBarHeight = 0;
	int yBarGrabHeight = 0;
    void SetBoxCoords(){
		yScrollPercent = (double)ScrollTop / (_innerText->height - innerHeight);
        int minPosWidth = THEME_SCROLLBAR_WIDTH;
        int maxY = height - 1;
		int yBarLeft = width - THEME_SCROLLBAR_WIDTH;
		int yBarRight = width - 1;
		innerScrollBarHeight = maxY - (THEME_SCROLLBAR_WIDTH * 2);
		
		yBarGrabHeight = innerScrollBarHeight - (_innerText->height - innerHeight);
		if (yBarGrabHeight < minPosWidth){ yBarGrabHeight = minPosWidth; }
		int innerPosTop = THEME_SCROLLBAR_WIDTH + ((double)(innerScrollBarHeight - yBarGrabHeight) * yScrollPercent);
		
		//scrollbar outer
		yBar = {yBarLeft, 0, yBarRight, maxY};
		yBarButtonUp = {yBarLeft, 0, yBarRight, THEME_SCROLLBAR_WIDTH};
		yBarButtonDown = {yBarLeft, maxY - THEME_SCROLLBAR_WIDTH, yBarRight, maxY};
		yBarScrollPos = {yBarLeft, innerPosTop, yBarRight, innerPosTop + yBarGrabHeight};
		yBarSpaceAbove = {yBarLeft, THEME_SCROLLBAR_WIDTH, yBarRight, innerPosTop};
		yBarSpaceBelow = {yBarLeft, innerPosTop + yBarGrabHeight, yBarRight, maxY - THEME_SCROLLBAR_WIDTH};
		scrollBarVisible = _innerText->height > innerHeight;
    }

	//scrollBarPos relative to the top of the container.
	int ScrollBarPosToScrollTop(int scrollBarPos){
		double travelHeight = innerScrollBarHeight - yBarGrabHeight;
		double offsetPos = scrollBarPos - THEME_SCROLLBAR_WIDTH;
		double newPercent = offsetPos / travelHeight;
		return newPercent * (double)(_innerText->height - innerHeight);
	}

	int ScrollTop = 0;

	void Update(){
		if (inButtonHold && buttonHoldDir == -1){
			ScrollUp();
		} else if (inButtonHold){
			ScrollDown();
		} 
		_innerText->y = 0-ScrollTop;
		SetBoxCoords();
		UIDrawable::Update();
	}

	bool inButtonHold = false;
	int buttonHoldDir = 0;
	bool inScrollDrag = false;
	int lastScrollDragY = 0;
	int lastScrollPosY = 0;
	void OnEvent(EventEmitter *source, std::string event, EventData data){
		if (scrollBarVisible && event == "LeftMouseButtonDown"){
			int localX = data.data1;
			int localY = data.data2;
			if (CoordsIntersectBox(yBarButtonUp, localX, localY)){
				inButtonHold = true;
				buttonHoldDir = -1;
			} else if (CoordsIntersectBox(yBarButtonDown, localX, localY)){
				inButtonHold = true;
				buttonHoldDir = 1;
			} else if (CoordsIntersectBox(yBarScrollPos, localX, localY)){
				inScrollDrag = true;
				lastScrollDragY = localY;
				lastScrollPosY = yBarScrollPos.y1;
			}
		} else if (scrollBarVisible && event == "LeftMouseButtonUp"){
			if (!inButtonHold && !inScrollDrag){
				//check if empty space on either side of scrollbar is clicked
				if (CoordsIntersectBox(yBarSpaceAbove, data.data1, data.data2)){
					ScrollPageUp();
				} else if (CoordsIntersectBox(yBarSpaceBelow, data.data1, data.data2)){
					ScrollPageDown();
				}
			}
			inButtonHold = false;
			inScrollDrag = false;
			buttonHoldDir = 0;
		} else if (scrollBarVisible && event == "MouseMove"){
			if (inScrollDrag){
				int diff = data.data2 - lastScrollDragY;
				int newScrollTop = ScrollBarPosToScrollTop(lastScrollPosY + diff);
				ScrollTo(newScrollTop);
				lastScrollDragY = data.data2;
				lastScrollPosY = yBarScrollPos.y1;
			}
		}
	}

	void OnKeyUp(int KeyCode, int ShiftState, int Ascii) {
		if (KeyCode == KEY_UP_ARROW){
			ScrollTo(ScrollTop-10);
		} else if (KeyCode == KEY_DOWN_ARROW){
			ScrollTo(ScrollTop + 10);
		} else if (KeyCode == KEY_PAGE_DOWN){
			ScrollPageDown();
		} else if (KeyCode == KEY_PAGE_UP){
			ScrollPageUp();
		}
	}

public:

	UITextArea* GetText(){
		return _innerText;
	}

	bool ScrollToTop(){
		ScrollTop = 0;
		SetBoxCoords();
		needsRedraw = true;
		return true;
	}

	bool ScrollDown(){
		int newScroll = ScrollTop+1;
		if (newScroll > _innerText->height - height){
		 	return false;
		}
		ScrollTop++;
		SetBoxCoords();
		needsRedraw = true;
		return true;
	}

	void ScrollPageDown(){
		ScrollTo(ScrollTop + innerHeight);
	}

	bool ScrollUp(){
		int newScroll = ScrollTop - 1;
		if (newScroll < 0){
			return false;
		}
		ScrollTop--;
		SetBoxCoords();
		needsRedraw = true;
		return true;
	}

	void ScrollPageUp(){
		ScrollTo(ScrollTop - innerHeight);
	}

	void ScrollTo(int top){
		if (top < 0){
			top = 0;
		}
		if (top > _innerText->height - height){
			top = _innerText->height - height;
		}
		ScrollTop = top;
		SetBoxCoords();
		needsRedraw = true;
	}

	UIScrollingText(int width, int height) : UIDrawable(width, height, width - THEME_SCROLLBAR_WIDTH, height, 0, 0, false){
		_innerText = new UITextArea(width - THEME_SCROLLBAR_WIDTH - 2, height);
		AddChild(_innerText);
		BindEvent("MouseMove", this);
		BindEvent("LeftMouseButtonDown", this);
		BindEvent("LeftMouseButtonUp", this);
	}
};

#endif