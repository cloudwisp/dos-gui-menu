#ifndef UIWindowController_cpp
#define UIWindowController_cpp

#include <grx20.h>
#include "AppUI.h"
#include "UIWindow.cpp"
#include "UIAppScreen.cpp"
#include "stack"

class UIWindowController;
UIWindowController *_windowController = NULL;

class UIWindowController {
private:
    std::stack<UIWindow*> previousWindows;
protected:
	UIWindow *windows[255] = {NULL};
    int windowCount = 0;
	UIWindow *focusedWindow = NULL;
    //UIWindow *lastFocusedWindow = NULL;
public:
    virtual void OnEvent(EventEmitter *source, std::string event, EventData data) {
        if (event == "RequestOpen"){
            UIWindow* srcWindow = (UIWindow*)source;
            srcWindow->Show();
            SetFocusedWindow(srcWindow);
        }
    };
    static UIWindowController *Get();

    UIAppScreen *screen = NULL;

    void Update(){
        int i;
        //check for window state changes
        for (i = windowCount-1; i >= 0; i--){
            if (windows[i]->closed && focusedWindow == windows[i]){
                //window was closed
                focusedWindow->Hide();
                focusedWindow->EmitEvent("Closed");
                if (windows[i]->destroy){
                    RemoveWindow(focusedWindow);
                    screen->RemoveChild(focusedWindow);
                    delete focusedWindow;
                    focusedWindow = NULL;
                }
                //focus on the first window (main)
                if (previousWindows.size() > 0){
                    SetFocusedWindow(previousWindows.top(), true);
                    if (previousWindows.size() > 1){
                        //always leave the first window on the stack.
                        previousWindows.pop();
                    }
                }
            }
        }
        //call update on the UIDrawable tree
        screen->Update();
    }

	UIAppScreen *GetScreen(){
		return screen;
	}

	void AddWindow(UIWindow *window){
	    windows[windowCount] = window;
        windowCount++;
        screen->AddChild((UIDrawable*) window);
        if (!focusedWindow){
            focusedWindow = window;
        }
        window->BindEvent("RequestOpen", (EventConsumer*)this);
	}
	void AddWindow(UIWindow *window, int focused){
        AddWindow(window);
        if (focused){
            SetFocusedWindow(window);
        }
	}
	void RemoveWindow(UIWindow *window){
        int i, z, removed;
        removed = 0;
        for (i = windowCount-1; i >= 0; i--){
            if (windows[i] == window){
                for (z = i+1; z < windowCount; z++){
                    windows[z-1] = windows[z];
                    windows[z] = NULL;
                }
                windowCount--;
            }
        }
	}

	UIWindow *GetFocusedWindow(){
        return focusedWindow;
	}

	void SetFocusedWindow(UIWindow *window, bool stackOp = false){
        if (!stackOp && focusedWindow && window != focusedWindow){
            previousWindows.push(window);
        }
        focusedWindow = window;
        
        focusedWindow->BringToFront();
	}

	UIWindowController(int screenWidth, int screenHeight){

		//screen
		screen = new UIAppScreen(screenWidth, screenHeight);
		_windowController = this;
	}
	~UIWindowController(){
        delete screen;
	}
};

UIWindowController *UIWindowController::Get(){
    return _windowController;
}

#endif