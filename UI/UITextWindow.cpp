#ifndef UITextWindow_cpp
#define UITextWindow_cpp

#include <grx20.h>
#include "AppUI.h"
#include "UIWindow.cpp"
#include "UIWindowController.cpp"
#include "UITextArea.cpp"

class UITextWindow : public UIWindow {
private:
    UITextArea *textArea = NULL;
public:

	void SetText(char* text){
		textArea->SetText(text);
	}

    UITextWindow(char *text, int drawWidth, int drawHeight) : UIWindow(drawWidth, drawHeight){
        textArea = new UITextArea(drawWidth, drawHeight);
        textArea->SetText(text);
        textArea->SetAlign(GR_ALIGN_CENTER,GR_ALIGN_CENTER);
        AddChild((UIDrawable*) textArea);
    }

	UITextWindow(char *text) : UIWindow(UIWindowController::Get()->GetScreen()->width, UIWindowController::Get()->GetScreen()->height) {
		UITextWindow(text, UIWindowController::Get()->GetScreen()->width, UIWindowController::Get()->GetScreen()->height);
	}

    ~UITextWindow(){
        delete textArea;
    }
};

#endif