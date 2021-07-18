#ifndef CharEditorApp_cpp
#define CharEditorApp_cpp

#include "../App.cpp"
#include "../keyboard.h"
#include <memory.h>
#include "../AppCtrls.cpp"

class CharEditorApp : public CWApplication {
private:

    UITextBox* box1;
    UITextBox* box2;
    UITextBox* box3;
    UITextBox* box4;
    UIButton* button;
    UIWindow* mainWindow;
    UIPanel* leftPanel;
    UIPanel* midPanel;
    UIPanel* rightPanel;

    void check_inputs(int *cancelInputPropagation){
        /*UINT16 ShiftState = Get_Shift_State();
        char* ctext = "";
        sprintf(ctext, "Shift State "PRINTF_BINARY_PATTERN_INT16, PRINTF_BYTE_TO_BINARY_INT16(ShiftState));
        debugOut(std::string(ctext));*/
    };

	void on_keyup(int ScanCode, int ShiftState, int Ascii, int *cancelInputPropagation){
        if (ScanCode == KEY_ESC){
            End();
        }
    };

	void update(){
        
    };
	void render(){

    };
	void on_start(){

    };
public:

    void OnEvent(EventEmitter* source, std::string event, EventData data){
        if (source == button && event == "Click"){
            box3->SetText("Go!");
        }
    }

    CharEditorApp(int screenWidth, int screenHeight) : CWApplication(screenWidth,screenHeight,8,20){

        mainWindow = new UIWindow(screenWidth, screenHeight);

        
        UIWindowController::Get()->AddWindow(mainWindow, 1);

        leftPanel = new UIPanel(GrAllocColor(0,0,0), 320/3, 200);
        leftPanel->containertabstop = 1;
        midPanel = new UIPanel(GrAllocColor(0,0,0), 320/3, 200);
        midPanel->x = 320/3;
        midPanel->containertabstop = 2;
        rightPanel = new UIPanel(GrAllocColor(0,0,0), 320/3, 200);
        rightPanel->x = 2* (320/3);
        rightPanel->containertabstop = 3;
        mainWindow->AddChild(leftPanel);
        mainWindow->AddChild(midPanel);
        mainWindow->AddChild(rightPanel);

        box1 = new UITextBox(100,20,30);
        box1->tabstop = 1;
        box2 = new UITextBox(100,20, 30);
        box2->tabstop = 2;
        box2->y = 22;

        leftPanel->AddChild(box1);
        leftPanel->AddChild(box2);

        box3 = new UITextBox(100,20, 30);
        box3->tabstop = 4;
        box4 = new UITextBox(100,20,30);
        box4->tabstop = 5;
        box4->y = 22;
        
        midPanel->AddChild(box3);
        midPanel->AddChild(box4);

        button = new UIButton(100, 20);
        button->SetText("Click me");
        button->BindEvent("Click", this);

        rightPanel->AddChild(button);
        
    }

    ~CharEditorApp(){
        delete box1;
        delete box2;
        delete leftPanel;
        delete midPanel;
        delete rightPanel;
        delete box3;
        delete box4;
        delete button;
    }
};

#endif