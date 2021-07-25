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
		GrClearContextC(ctx, GrAllocColor(0,0,0));
		boxColors.fbx_intcolor = _bg;
		boxColors.fbx_topcolor = shadow;
		boxColors.fbx_leftcolor = shadow;
		boxColors.fbx_bottomcolor = highlight;
		boxColors.fbx_rightcolor = highlight;
		GrSetContext(ctx);
		GrFramedBox(_borderWidth, _borderWidth, width-(_borderWidth*2), height-(_borderWidth*2), _borderWidth, &boxColors);
		Freeze();
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
			Unfreeze();
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

		Unfreeze();
	}

public:

	void OnEvent(EventEmitter* source, std::string event, EventData data) override {
		if (event == "LeftMouseButtonUp"){
			Focus();
		}
		if (event == "GotFocus"){
			innerText->ShowCursor();
			Unfreeze();
		}
		if (event == "LostFocus"){
			innerText->HideCursor();
			Unfreeze();
		}
		UIDrawable::OnEvent(source, event, data);
	}

	void SetText(std::string text){
		innerText->SetText(text);
        EmitEvent("Changed");
		Unfreeze();
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
		_fg = GrAllocColor(255,255,255);
		_bg = GrAllocColor(10,10,10);
		highlight = GrAllocColor(230,230,230);
		shadow = GrAllocColor(100,100,100);
		innerText = new UITextArea(width - (_borderWidth * 4), height - (_borderWidth * 4));
		innerText->y = _borderWidth * 2;
		innerText->x = _borderWidth * 2;
		innerText->SetColor(_fg, GrNOCOLOR);
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