#ifndef UIWindowController_cpp
#define UIWindowController_cpp

#include <grx20.h>
#include "AppUI.h"
#include "UIWindow.cpp"
#include "UIAppScreen.cpp"

class UIWindowController;
UIWindowController *_windowController = NULL;

class UIWindowController {
private:

protected:
	UIWindow *windows[255] = {NULL};
    int windowCount = 0;
	UIWindow *focusedWindow = NULL;

public:

    static UIWindowController *Get();

    UIAppScreen *screen = NULL;

    void Update(){
        int i;
        //check for window state changes
        for (i = windowCount-1; i >= 0; i--){
            if (windows[i]->closed && focusedWindow == windows[i]){
                //window was closed
                focusedWindow->Hide();
                if (windows[i]->destroy){
                    RemoveWindow(focusedWindow);
                    screen->RemoveChild(focusedWindow);
                    delete focusedWindow;
                    focusedWindow = NULL;
                }
                //focus on the first window (main)
                SetFocusedWindow(windows[0]);
            } else if (!(windows[i]->closed) && focusedWindow != windows[i]){
                //window was just opened
                focusedWindow->Show();
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

	void SetFocusedWindow(UIWindow *window){
        focusedWindow = window;
        focusedWindow->BringToFront();
	}

	UIWindowController(int screenWidth, int screenHeight, int bitDepth){

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