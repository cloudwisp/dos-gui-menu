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
    UIWindow* mainWindow;

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
    CharEditorApp(int screenWidth, int screenHeight) : CWApplication(screenWidth,screenHeight,8,20){

        mainWindow = new UIWindow(screenWidth, screenHeight);
        UIWindowController::Get()->AddWindow(mainWindow, 1);
        box1 = new UITextBox(100,20,30);
        box1->tabstop = 1;
        box2 = new UITextBox(100,20, 30);
        box2->tabstop = 2;
        box2->y = 22;
        
        mainWindow->AddChild(box1);
        mainWindow->AddChild(box2);
        
    }

    ~CharEditorApp(){
        delete box1;
        delete box2;
    }
};

#endif