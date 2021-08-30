#ifndef UITextBox_cpp
#define UITextBox_cpp

#include <grx20.h>
#include "../keyboard.h"
#include "../AppEvent.h"
#include "UITextArea.cpp"

class UITextBox : public UIDrawable {
private:
	UITextArea *innerText = NULL;
	GrColor _bg;
	GrColor _fg;
	GrFBoxColors boxColors;
	GrColor highlight;
	GrColor shadow;
	int _borderWidth = 2;
	bool intMode = false;
	bool multiLine = false;

	void draw_internal(){
		//draw stuff
		GrClearContextC(ctx, THEME_COLOR_TRANSPARENT);
		boxColors.fbx_intcolor = _bg;
		boxColors.fbx_topcolor = shadow;
		boxColors.fbx_leftcolor = shadow;
		boxColors.fbx_bottomcolor = highlight;
		boxColors.fbx_rightcolor = highlight;
		GrSetContext(ctx);
		GrFramedBox(_borderWidth, _borderWidth, width-(_borderWidth*2), height-(_borderWidth*2), _borderWidth, &boxColors);
	}

	void OnKeyUp(int KeyCode, int ShiftState, int Ascii) override {
		// char* ctext = (char*) malloc(sizeof(char) * 1000);
		// sprintf(ctext, "KeyCode %x", KeyCode);
		// debugOut(std::string(ctext));
		// free(ctext);

		if (!multiLine && Ascii == 0x0A){
			//ignore line feed
			return;
		}

		if (KeyCode == KEY_BACKSPACE){
            innerText->CharBackspace();
			EmitEvent("Changed");
            return;
        }

        if (KeyCode == KEY_DELETE){
            innerText->CharDelete();
			EmitEvent("Changed");
        }

        if (KeyCode == KEY_RIGHT_ARROW){
            innerText->CursorRight();
        }

        if (KeyCode == KEY_LEFT_ARROW){
            innerText->CursorLeft();
        }

        if (KeyCode == KEY_DOWN_ARROW){
            innerText->CursorDown();
        }

        if (KeyCode == KEY_UP_ARROW){
            innerText->CursorUp();
        }
        
		if (intMode && Ascii > 0x0 && (Ascii > 0x39 || Ascii < 0x30)){
			//Outside of valid range
		} else if (Ascii > 0x0){
            innerText->CharAdd((char) Ascii);
			EmitEvent("Changed");
        }
	}

public:

	void OnEvent(EventEmitter* source, std::string event, EventData data) override {
		if (event == "LeftMouseButtonUp"){
			Focus();
		}
		if (event == "GotFocus"){
			innerText->ShowCursor();
		}
		if (event == "LostFocus"){
			innerText->HideCursor();
		}
		UIDrawable::OnEvent(source, event, data);
	}

	void SetText(std::string text){
		innerText->SetText(text);
        EmitEvent("Changed");
	}

	void SetIntMode(bool limitToInt){
		intMode = limitToInt;
	}

	void SetMultiline(bool isMultiline){
		multiLine = isMultiline;
	}

	std::string GetText(){
		return innerText->GetText();
	}

	UITextBox(int width, int height, int maxChars) : UIDrawable(width, height){
		_fg = THEME_CONTROL_TEXT;
		_bg = THEME_CONTROL_BACKGROUND;
		highlight = THEME_3D_HIGHLIGHT;
		shadow = THEME_3D_SHADOW;
		innerText = new UITextArea(width - (_borderWidth * 4), height - (_borderWidth * 4));
		innerText->y = _borderWidth * 2;
		innerText->x = _borderWidth * 2;
		innerText->SetColor(_fg, THEME_COLOR_TRANSPARENT);
		AddChild(innerText);
		BindEvent("GotFocus", this);
		BindEvent("LostFocus", this);
		BindEvent("LeftMouseButtonUp", this);
	}
	~UITextBox(){
		delete innerText;
	}
};

#endif