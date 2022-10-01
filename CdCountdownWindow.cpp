#ifndef CdCountdownWindow_CPP
#define CdCountdownWindow_CPP

#include "AppUI.cpp"

class CdCountdownWindow : public UIWindow {
private:
    UITextArea *statusMessage = NULL;
    bool pendingCountdown = false;
    clock_t countdownStart;
    int lastDisplayedSecond = 0;
    std::string secondDisplay;

    void draw_internal() override{
		GrClearContextC(ctx, THEME_WINDOW_BACKGROUND_COLOR);
		GrSetContext(ctx);
		Draw3dButton(ctx, BoxCoords {1,1,width-2,height-2},THEME_WINDOW_BACKGROUND_COLOR,false);
	}

    void UpdateSecondsDisplay(int seconds){
        lastDisplayedSecond = seconds;
        secondDisplay = "";
        if (seconds > 0){
            secondDisplay.append("\r\nTime Until Launch: ").append(std::to_string(seconds));
        } else {
            secondDisplay = std::string("This item requires a CD-ROM.\r\nInsert the CD now, or hit ESC to cancel.");
            secondDisplay.append("\r\nPress ENTER to Launch...");
        }
        
        statusMessage->SetText(secondDisplay);
        needsRedraw = true;
    }

public:

    void OnKeyUp(int ScanCode, int ShiftState, int Ascii){
        if (ScanCode == KEY_ESC){
            pendingCountdown = false;
            Close();
        } else if (ScanCode == KEY_ENTER || ScanCode == KEY_NUMPAD_ENTER){
            LaunchCountdown();
        }
    }

    void OnEvent(EventEmitter *source, std::string event, EventData data){
        if (event == "Closed" && source == this){
            pendingCountdown = false;
        } else if (event == "Opened" && source == this){
            pendingCountdown = false;
            UpdateSecondsDisplay(0);
        }
        UIWindow::OnEvent(source, event, data);
    }

    int countdownSeconds = 5;

    void Update(){
        if (pendingCountdown){
            clock_t now = clock();
            int diff = clockToMilliseconds(now - countdownStart);
            if (diff > countdownSeconds * 1000){
                pendingCountdown = false;
                EmitEvent("CountdownEnd");
            } else if (diff > (countdownSeconds - lastDisplayedSecond + 1) * 1000) {
                UpdateSecondsDisplay(lastDisplayedSecond - 1);
            }
        }
    }

    void LaunchCountdown(){
        if (pendingCountdown){
            return;
        }
        countdownStart = clock();
        lastDisplayedSecond = countdownSeconds + 1;
        pendingCountdown = true;
    }

    CdCountdownWindow(int width, int height) : UIWindow(width, height){
        statusMessage = new UITextArea(width, height);
        statusMessage->SetAlign(GR_ALIGN_CENTER, GR_ALIGN_CENTER);
        statusMessage->SetText("");
        AddChild(statusMessage);
        BindEvent("Closed", this);
        BindEvent("Opened", this);
    }
    ~CdCountdownWindow(){
        delete statusMessage;
    }

};

#endif