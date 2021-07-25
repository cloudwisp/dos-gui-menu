#ifndef UIModalWindow_cpp
#define UIModalWindow_cpp

#include <grx20.h>
#include <string>
#include <cstring>
#include "../AppEvent.h"
#include "UITitledWindow.cpp"
#include "UIButton.cpp"

class UIModalWindow : public UITitledWindow {
private:
	UIButton* cancelButton;
	UIButton* okButton;
	std::string _prompt;
public:

	void OnEvent(EventEmitter* source, std::string event, EventData data){
		if (source == cancelButton && event == "Click"){
			EmitEvent("Cancel");
			CloseAndDestroy();
		} else if (source == okButton && event == "Click"){
			EmitEvent("Ok");  //emit for whoever launched the dialog to inspect and fetch values from controls added to the window.
			CloseAndDestroy();
		}
	}

	UIModalWindow(int width, int height, std::string prompt) : UITitledWindow(width, height, prompt){
		_prompt = prompt;

		cancelButton = new UIButton(width/2, 20);
		cancelButton->y = innerHeight - 20;
		cancelButton->x = 0;
		cancelButton->tabstop = 100;
		cancelButton->SetText("Cancel");
		cancelButton->BindEvent("Click", this);

		AddChild(cancelButton);

		okButton = new UIButton(width / 2, 20);
		okButton->y = innerHeight - 20;
		okButton->x = width / 2;
		okButton->tabstop = 101;
		okButton->SetText("OK");
		okButton->BindEvent("Click", this);
		AddChild(okButton);
	}

	~UIModalWindow(){
		delete cancelButton;
		delete okButton;
	}
};

#endif