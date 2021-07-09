#ifndef APPCTRLS_CPP
#define APPCTRLS_CPP

#include "AppUI.cpp"
#include "AppEvent.cpp"

class UIControl {
private:

public:
	int TabStop;
	UIControl(){
		TabStop = 1;
	}
};

/* Command Button */
#define BUTTONSTATE_NORMAL 1
#define BUTTONSTATE_PRESSED 2
#define BUTTONSTATE_SELECTED 3

class UIButton : public UIDrawable, public UIControl {
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
		GrClearContextC(ctx, GrAllocColor(0,0,0));
		boxColors.fbx_intcolor = _bg;

		boxColors.fbx_topcolor = highlight;
		boxColors.fbx_leftcolor = highlight;
		boxColors.fbx_bottomcolor = shadow;
		boxColors.fbx_rightcolor = shadow;
		textArea->SetColor(_fg, GrNOCOLOR);

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
		Freeze();
	}

	int _inMouseOver = 0;
public:

	void SetColor(GrColor backColor, GrColor textColor) {
		_bg = backColor;
		_fg = textColor;
		textArea->SetColor(textColor, GrNOCOLOR);
		Unfreeze();
	}

	void SetText(char *text){
		_text = text;
		textArea->SetText(text);
		Unfreeze();
	}

	void SetFont(GrFont *font){
		textArea->SetFont(font);
	}

	void SetState(int state){
		_state = state;
		Unfreeze();
	}

	void OnEvent(EventEmitter *source, const char *event, EventData data){
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
				EmitEvent("Click");
			}

		} else {
			//handle events from other objects
		}
		//pass it on to parent
		UIDrawable::OnEvent(source,event,data);
	}

	UIButton (int width, int height) : UIDrawable(width,height) {
		textArea = new UITextArea(width-_borderWidth, height-_borderWidth);
		textArea->x = _borderWidth;
		textArea->y = _borderWidth;
		textArea->SetAlign(GR_ALIGN_CENTER,GR_ALIGN_CENTER);
		AddChild(textArea);
		BindEvent("MouseOver", (EventConsumer*) this);
		BindEvent("MouseOut", (EventConsumer*) this);
		BindEvent("LeftMouseButtonDown", (EventConsumer*) this);
		BindEvent("LeftMouseButtonUp", (EventConsumer*) this);
		_fg = GrAllocColor(255,255,255);
		_bg = GrAllocColor(10,10,10);
		highlight = GrAllocColor(230,230,230);
		shadow = GrAllocColor(100,100,100);
		tabstop = 1;
	}
	~UIButton(){
        delete textArea;
	}
};

#endif
