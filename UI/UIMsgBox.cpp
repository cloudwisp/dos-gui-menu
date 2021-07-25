#ifndef UIMsgBox_cpp
#define UIMsgBox_cpp

#include <grx20.h>
#include "../keyboard.h"
#include "AppUI.h"
#include "UIWindow.cpp"
#include "UIWindowController.cpp"
#include "UITextArea.cpp"

class UIMsgBox : public UIWindow {
private:
    UITextArea *textArea = NULL;
public:
    static UIMsgBox* Create(char *text, int drawWidth, int drawHeight);
    void OnKeyUp(int ScanCode, int ShiftState, int Ascii){
        UIWindow::OnKeyUp(ScanCode, ShiftState, Ascii);
        if (ScanCode == KEY_ENTER){
            CloseAndDestroy();
        }
    }
    UIMsgBox(char *text, int drawWidth, int drawHeight) : UIWindow(drawWidth, drawHeight){
        textArea = new UITextArea(drawWidth, drawHeight);
        textArea->SetText(text);
        textArea->SetAlign(GR_ALIGN_CENTER,GR_ALIGN_CENTER);
        AddChild((UIDrawable*) textArea);
    }
    ~UIMsgBox(){
        delete textArea;
    }
};

UIMsgBox* UIMsgBox::Create(char *text, int drawWidth, int drawHeight){
    UIMsgBox *msgBox = new UIMsgBox(text, drawWidth, drawHeight);
    msgBox->x = (UIWindowController::Get()->GetScreen()->width / 2) - (drawWidth/2);
    msgBox->y = (UIWindowController::Get()->GetScreen()->height / 2) - (drawHeight/2);
    UIWindowController::Get()->AddWindow((UIWindow*) msgBox, 1);
    return msgBox;
}

#endif