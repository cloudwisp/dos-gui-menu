#ifndef UIButton_cpp
#define UIButton_cpp

#include <memory.h>
#include <string>
#include <stdio.h>
#include <time.h>
#include "../Common.cpp"
#include "../AppEvent.cpp"
#include "../keyboard.h"
#include "UITextArea.cpp"

/* Command Button */
#define BUTTONSTATE_NORMAL 1
#define BUTTONSTATE_PRESSED 2
#define BUTTONSTATE_SELECTED 3

class UIButton : public UIDrawable {
private:
	UITextArea *textArea = NULL;
	GrColor _bg;
	GrColor _fg;
	int _changed = 0;
	const char *_text;
	GrFBoxColors boxColors;
	GrColor highlight;
	GrColor shadow;
	int _state = BUTTONSTATE_NORMAL;
	int _borderWidth = 2;

	void draw_internal(){
		//draw stuff
		GrClearContextC(ctx, THEME_COLOR_TRANSPARENT);
		boxColors.fbx_intcolor = _bg;

		boxColors.fbx_topcolor = highlight;
		boxColors.fbx_leftcolor = highlight;
		boxColors.fbx_bottomcolor = shadow;
		boxColors.fbx_rightcolor = shadow;
		textArea->SetColor(_fg, THEME_COLOR_TRANSPARENT);

		if (_state == BUTTONSTATE_PRESSED){
			//flip highlight border direction
			boxColors.fbx_topcolor = shadow;
			boxColors.fbx_leftcolor = shadow;
			boxColors.fbx_bottomcolor = highlight;
			boxColors.fbx_rightcolor = highlight;
		} else if (_state == BUTTONSTATE_SELECTED){
			//solid border highlight
			boxColors.fbx_bottomcolor = highlight;
			boxColors.fbx_rightcolor = highlight;
		}
		GrSetContext(ctx);
		GrFramedBox(_borderWidth, _borderWidth, width-(_borderWidth*2), height-(_borderWidth*2), _borderWidth, &boxColors);
	}


	int _inMouseOver = 0;
	clock_t lastClick = clock();
public:

	void Update(){
		
		if (_state == BUTTONSTATE_PRESSED && !_inMouseOver){
			//flip back to un-pressed state after button is pressed using keyboard.
			clock_t now = clock();
			if (clockToMilliseconds(now-lastClick) > 250){
				SetState(BUTTONSTATE_NORMAL);
				needsRedraw = true;
			}
		}
	}

	void SetColor(GrColor backColor, GrColor textColor) {
		_bg = backColor;
		_fg = textColor;
		textArea->SetColor(textColor, GrNOCOLOR);
		needsRedraw = true;
	}

	void SetText(char *text){
		_text = text;
		textArea->SetText(text);
		needsRedraw = true;
	}

	void SetFont(GrFont *font){
		textArea->SetFont(font);
	}

	void SetState(int state){
		_state = state;
		needsRedraw = true;
	}

	void OnKeyUp(int ScanCode, int ShiftState, int Ascii){
		if (ScanCode == KEY_ENTER || ScanCode == KEY_NUMPAD_ENTER){
			SetState(BUTTONSTATE_PRESSED);
			Click();
		}
	}

	void Click(){
		lastClick = clock();
		EmitEvent("Click");
	}

	void OnEvent(EventEmitter *source, std::string event, EventData data){
		if (source == this){

			if (event == "MouseOver"){
				SetState(BUTTONSTATE_SELECTED);
				_inMouseOver = 1;
			} else if (event == "MouseOut"){
				SetState(BUTTONSTATE_NORMAL);
				_inMouseOver = 0;
			} else if (event == "LeftMouseButtonDown"){
				SetState(BUTTONSTATE_PRESSED);
			} else if (event == "LeftMouseButtonUp"){
				if (_inMouseOver){
					//return to selected state
					SetState(BUTTONSTATE_SELECTED);
				} else {
					//return to normal state
					SetState(BUTTONSTATE_NORMAL);
				}
				Click();
			}

		} else {
			//handle events from other objects
		}
		//pass it on to parent
		UIDrawable::OnEvent(source,event,data);
	}

	UIButton (int width, int height) : UIDrawable(width,height) {
		textArea = new UITextArea(width-(_borderWidth*2), height-(_borderWidth*2));
		textArea->x = _borderWidth;
		textArea->y = _borderWidth;
		textArea->SetAlign(GR_ALIGN_CENTER,GR_ALIGN_CENTER);
		AddChild(textArea);
		BindEvent("MouseOver", (EventConsumer*) this);
		BindEvent("MouseOut", (EventConsumer*) this);
		BindEvent("LeftMouseButtonDown", (EventConsumer*) this);
		BindEvent("LeftMouseButtonUp", (EventConsumer*) this);
		_fg = THEME_BUTTON_TEXT_PRIMARY;
		_bg = THEME_BUTTON_BACKGROUND_PRIMARY;
		highlight = THEME_3D_HIGHLIGHT;
		shadow = THEME_3D_SHADOW;
		tabstop = 1;
	}
	~UIButton(){
        delete textArea;
	}
};

#endif