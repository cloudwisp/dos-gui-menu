#ifndef App_CPP
#define App_CPP

#include <pc.h>
#include <stdlib.h>
#include <dpmi.h>
#include <time.h>
#include "AppUI.cpp"
#include "Common.cpp"
#include "AppEvent.cpp"
#include "mouse.c"

#include "App.h"
//Base application class, encapsulating common application functionality
//Sreen and UI elements
//Update, render and input check cycle
class CWApplication : public EventEmitter, public EventConsumer, public UIWindowController {
private:

	UITextArea *diagnostic = NULL;

	int _end = 0;
	int _ms_per_update = 16;
	int _mouse_enabled = 0;

	void _loop(){

		clock_t deltaTime = 0;
		int frames = 0;
		double  frameRate = 30;
		double  averageFrameTimeMilliseconds = 33.333;
		double lastFps = 0;

		char *fpsDisplay = (char*) malloc(sizeof(char)*255);
		strcpy(fpsDisplay,"");

		double lag = 0;

		double previous = clock();

        while (!_end){
			clock_t beginFrame = clock();

			//game tick lag
			clock_t current = clock();
			double elapsed = current-previous;
			previous = current;
			lag += clockToMilliseconds(elapsed);

			//check inputs, non blocking
			_check_inputs();
            while (lag >= _ms_per_update){
				_update();
				lag -= _ms_per_update;
			}
            diagnostic->SetText(fpsDisplay);

			_render();

			clock_t endFrame = clock();
			deltaTime += endFrame - beginFrame;
			frames ++;

			if (clockToMilliseconds(deltaTime)>1000.0){
				frameRate = (double)frames*0.5 +  frameRate*0.5; //more stable
				frames = 0;
				deltaTime -= CLOCKS_PER_SEC;
				averageFrameTimeMilliseconds  = 1000.0/(frameRate==0?0.001:frameRate);

				sprintf(fpsDisplay,"FPS: %.2f \n lag: %.2f", frameRate, clockToMilliseconds(lag));
			}

		}
		free(fpsDisplay);

	}

	int mouseState[2] = {0,0};

	//UIDrawable *lastEventElements[255];
	UIDrawableCollection lastEventElements;
	int lastEventElementCount = 0;

	//UIDrawable *eventElements[255];
	UIDrawableCollection eventElements;
	int eventElementCount = 0;

	int _element_still_under_cursor(UIDrawable *element){
		int i;
		int found = 0;
		for (i = 0; i < eventElementCount; i++){
			if (eventElements[i] == element){
				found = 1;
				break;
			}
		}
		return found;
	}

	void _copy_last_event_elements(){
		int i;
		for (i = 0; i < eventElementCount; i++){
			lastEventElements[i] = eventElements[i];
		}
		lastEventElementCount = eventElementCount;
	}

	void _mousein_mouseout(){
		int i, j, found;
		for (i = 0; i < eventElementCount; i++){
			found = 0;
			for (j = 0; j < lastEventElementCount; j++){
				if (lastEventElements[j] == eventElements[i]){
					found = 1;
					break;
				}
			}
			if (!found){
				//MouseOver
				eventElements[i]->OnMouseOver();
			}
		}
		for (i = 0; i < lastEventElementCount; i++){
			found = 0;
			for (j = 0; j < eventElementCount; j++){
				if (lastEventElements[i] == eventElements[j]){
					found = 1;
					break;
				}
			}
			if (!found){
				//MouseOut
				lastEventElements[i]->OnMouseOut();
			}
		}
	}

	void _check_mouse_event(){
		if (!_mouse_enabled){ return; }
		int button, x, y;

		getmousepos(&button, &x, &y);

		screen->IdentifyVisibleElementsAtPosition(x, y, eventElements, &eventElementCount);
		
		screen->UnHighlightAllChildren();
		for (int i = 0; i < eventElementCount; i++){
			if (eventElements[i] == screen || eventElements[i]->window == eventElements[i]){
				continue;
			}
			eventElements[i]->Highlight();
		}

		if (x != mousePointer->x || y != mousePointer->y){
			//moved
			EmitEvent("MouseMove", x, y);
			mousePointer->x = x;
			mousePointer->y = y;
			_mousein_mouseout();
			screen->PropagateMouseEvent(x, y, "MouseMove");
		}
		if (button == 1){
			if (!mouseState[0]){
				//first click of left
				mouseState[0] = 1;
				EmitEvent("LeftMouseButtonDown", x, y);
				screen->PropagateMouseEvent(x, y, "LeftMouseButtonDown");
			}
			//clear right if marked down
			if (mouseState[1]){
				mouseState[1] = 0;
				EmitEvent("RightMouseButtonUp", x, y);
				EmitEvent("RightMouseButtonClick", x, y);
				screen->PropagateMouseEvent(x, y, "RightMouseButtonUp");
				screen->PropagateMouseEvent(x, y, "RightMouseButtonClick");
			}
		} else if (button == 2){
			if (!mouseState[1]){
				//first click of right
				mouseState[1] = 1;
				EmitEvent("RightMouseButtonDown", x, y);
				screen->PropagateMouseEvent(x, y, "RightMouseButtonDown");
			}
			//check left if marked down
			if (mouseState[0]){
				mouseState[0] = 0;
				EmitEvent("LeftMouseButtonUp", x, y);
				EmitEvent("LeftMouseButtonClick", x, y);
				screen->PropagateMouseEvent(x, y, "LeftMouseButtonUp");
				screen->PropagateMouseEvent(x, y, "LeftMouseButtonClick");
			}
		} else {
			if (mouseState[0]){
				//clear left - now up
				mouseState[0] = 0;
				EmitEvent("LeftMouseButtonUp", x, y);
				EmitEvent("LeftMouseButtonClick", x, y);
				screen->PropagateMouseEvent(x, y, "LeftMouseButtonUp");
				screen->PropagateMouseEvent(x, y, "LeftMouseButtonClick");
			}
			if (mouseState[1]){
				//clear right - now up
				mouseState[1] = 0;
				EmitEvent("RightMouseButtonUp", x, y);
				EmitEvent("RightMouseButtonClick", x, y);
				screen->PropagateMouseEvent(x, y, "RightMouseButtonUp");
				screen->PropagateMouseEvent(x, y, "RightMouseButtonClick");
			}
		}
		_copy_last_event_elements();
		eventElementCount = 0; //reset counter for next run
	}

	void _check_inputs(){
		int cancelInputPropagation = 0;
		check_inputs(&cancelInputPropagation);
		if (!cancelInputPropagation){
            focusedWindow->CheckInputs();
            EmitEvent("CheckInputs");
		}
        //flush the KeyUp buffer
        while (KeyUpWaiting()){
            cancelInputPropagation = 0;
			KeyUpInfo keyInfo = GetNextKeyUpCode();
            on_keyup(keyInfo.ScanCode, keyInfo.ShiftState, keyInfo.AsciiCharacter, &cancelInputPropagation);
            if (!cancelInputPropagation){
                focusedWindow->OnKeyUp(keyInfo.ScanCode, keyInfo.ShiftState, keyInfo.AsciiCharacter);
                EmitEvent("KeyUp", keyInfo.ScanCode, keyInfo.ShiftState);
            }

        }
        _check_mouse_event();
	}

	void _update(){
		EventQueue::EmitAllQueued();
		update();
		UIWindowController::Update();
		EmitEvent("Update");
	}

	void _render(){
		render();
		EmitEvent("Render");
		diagnostic->BringToFront(); //ensure diagnostics are at top
		mousePointer->BringToFront();
		screen->render();
	}

	virtual void check_inputs(int *cancelInputPropagation) = 0;
	virtual void on_keyup(int ScanCode, int ShiftState, int Ascii, int *cancelInputPropagation) = 0;
	virtual void update() = 0;
	virtual void render() = 0;
	virtual void on_start() = 0;
	static CWApplication* _app;

protected:

	UIPointer *mousePointer = NULL;

public:

    static CWApplication* GetApplication(){
        return _app;
    }

	void Start(){
	    on_start();
	    _loop();
	}

	void End(){
		_end = 1;
		GrSetMode(GR_default_text);
	}

	void EnableMouse(){
		initmouse();
		mousePointer = new UIPointer();
		mousePointer->x = 50;
		mousePointer->y = 50;
		screen->AddChild(mousePointer);
		_mouse_enabled = 1;
		screen->SetMouseEnabled();
	}

	void ToggleDiagnostics(int on){
		if (on){
			diagnostic->Show();
		} else {
			diagnostic->Hide();
		}
	}


	CWApplication(int screenWidth, int screenHeight, int bitDepth, int msPerUpdate) : UIWindowController(screenWidth, screenHeight, bitDepth) {
		SetKb();


		//diagnostics overlay
		diagnostic = new UITextArea(100, 40);
		diagnostic->x = 4;
		diagnostic->y = screenHeight-40;

		screen->AddChild((UIDrawable *) diagnostic);

		_ms_per_update = msPerUpdate;
		_app = this;
	}
	~CWApplication(){
		if (_mouse_enabled){
			GrMouseUnInit();
			delete mousePointer;
		}
		delete diagnostic;
		ResetKb();
	}
};

CWApplication* CWApplication::_app = NULL;


#endif
