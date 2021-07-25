#ifndef CharEditorApp_cpp
#define CharEditorApp_cpp

#include "../App.cpp"
#include "../keyboard.h"
#include <memory.h>
#include "../AppCtrls.cpp"
#include "CharEditUI.cpp"

class CharEditorApp : public CWApplication {
private:

    UITextArea* lbl_feetWidth = NULL;
    UITextArea* lbl_feetHeight = NULL;
    UITextBox* txt_feetWidth = NULL;
    UITextBox* txt_feetHeight = NULL;
    UIButton* button = NULL;
    UIWindow* mainWindow = NULL;
    UIStackedPanel* leftPanel = NULL;
    UIStackedPanel* midPanel = NULL;
    UIStackedPanel* rightPanel = NULL;
    UISpriteSheetNavigator* spriteNav = NULL;
    UISpritePreview* preview = NULL;
    UIModalWindow *openModal = NULL;
    UITextBox *modalText = NULL;
    SpriteSet* loadedSpriteset = NULL;
    int lastFeetWidth = 0;
    int lastFeetHeight = 0;

    void check_inputs(int *cancelInputPropagation){
        /*UINT16 ShiftState = Get_Shift_State();
        char* ctext = "";
        sprintf(ctext, "Shift State "PRINTF_BINARY_PATTERN_INT16, PRINTF_BYTE_TO_BINARY_INT16(ShiftState));
        debugOut(std::string(ctext));*/
    };

    void BuildOpenSpriteModal(){
        UIAppScreen* scr = UIAppScreen::Get();
        openModal = new UIModalWindow(scr->width * 0.8, scr->height * 0.8, "Open Sprite (name without extension)");
        openModal->x = (scr->width * 0.2) / 2;
        openModal->y = (scr->height * 0.2) / 2;
        openModal->BindEvent("Ok", this);
        openModal->BindEvent("Cancel", this);

        modalText = new UITextBox(200, 20, 8);
        modalText->tabstop = 1;
        modalText->x = 20;
        modalText->y = 20;        
        openModal->AddChild(modalText);
        
        UIWindowController::Get()->AddWindow(openModal,1);
        modalText->Focus();
    }

    void DestroyModalControls(){
        delete modalText;
    }

	void on_keyup(int ScanCode, int ShiftState, int Ascii, int *cancelInputPropagation){
        if (ScanCode == KEY_ESC){
            End();
        }

        if (ScanCode == KEY_O && (ShiftState & SHIFTSTATE_CTRL_LEFT || ShiftState & SHIFTSTATE_CTRL_RIGHT)){
            *cancelInputPropagation = 1;
            BuildOpenSpriteModal();
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
            //todo
            BuildOpenSpriteModal();
            return;
        }

        if (source == spriteNav && event == "SpriteChanged"){
            if (loadedSpriteset){
                preview->UpdateImage(loadedSpriteset, data.data1);
            }
            return;
        }

        if (source == txt_feetWidth && event == "Changed"){
            lastFeetWidth = 0;
            if (txt_feetWidth->GetText().size() > 0){
                lastFeetWidth = atoi(txt_feetWidth->GetText().c_str());
                preview->SetFeet(lastFeetWidth, lastFeetHeight);
            }
            return;
        }

        if (source == txt_feetHeight && event == "Changed"){
            lastFeetHeight = 0;
            if (txt_feetHeight->GetText().size() > 0){
                lastFeetHeight = atoi(txt_feetHeight->GetText().c_str());
                preview->SetFeet(lastFeetWidth, lastFeetHeight);
            }
        }

        if (source == openModal && event == "Ok"){
            std::string spriteName = std::string(modalText->GetText());
            loadedSpriteset = GameResources::GetSpriteSet(spriteName);
            spriteNav->SetSpriteSet(loadedSpriteset);
            preview->UpdateImage(loadedSpriteset, 0);
            DestroyModalControls();
            return;
        }
        if (source == openModal && event == "Cancel"){
            DestroyModalControls();
            return;
        }

        CWApplication::OnEvent(source, event, data);
    }

    CharEditorApp(int screenWidth, int screenHeight) : CWApplication(screenWidth,screenHeight,8,20){

        mainWindow = new UIWindow(screenWidth, screenHeight);

        
        UIWindowController::Get()->AddWindow(mainWindow, 1);

        leftPanel = new UIStackedPanel(GrAllocColor(0,0,0), screenWidth / 3, screenHeight);
        leftPanel->containertabstop = 1;
        midPanel = new UIStackedPanel(GrAllocColor(0,0,0), screenWidth / 3, screenHeight);
        midPanel->x = screenWidth/3;
        midPanel->containertabstop = 2;
        rightPanel = new UIStackedPanel(GrAllocColor(0,0,0), screenWidth / 3, screenHeight);
        rightPanel->x = 2* (screenWidth/3);
        rightPanel->containertabstop = 3;

        mainWindow->AddChild(leftPanel);
        mainWindow->AddChild(midPanel);
        mainWindow->AddChild(rightPanel);

        lbl_feetWidth = new UITextArea(100,10);
        lbl_feetWidth->SetText("Feet Clip Width:");
        leftPanel->AddChild(lbl_feetWidth);

        txt_feetWidth = new UITextBox(100,14,30);
        txt_feetWidth->SetText("1");
        txt_feetWidth->BindEvent("Changed", this);
        txt_feetWidth->SetIntMode(true);
        txt_feetWidth->tabstop = 1;
        leftPanel->AddChild(txt_feetWidth);

        lbl_feetHeight = new UITextArea(100,10);
        lbl_feetHeight->SetText("Feet Clip Height:");
        leftPanel->AddChild(lbl_feetHeight);

        txt_feetHeight = new UITextBox(100,14, 30);
        txt_feetHeight->SetText("1");
        txt_feetHeight->BindEvent("Changed", this);
        txt_feetHeight->SetIntMode(true);
        txt_feetHeight->tabstop = 2;
        leftPanel->AddChild(txt_feetHeight);

        spriteNav = new UISpriteSheetNavigator(midPanel->width, midPanel->height);
        spriteNav->BindEvent("SpriteChanged", this);
        spriteNav->tabstop = 4;
        
        midPanel->AddChild(spriteNav);

        preview = new UISpritePreview(100, 100);
        rightPanel->AddChild(preview);
        
        button = new UIButton(100, 20);
        button->SetText("Open Spriteset");
        button->BindEvent("Click", this);

        rightPanel->AddChild(button);
        
    }

    ~CharEditorApp(){
        delete lbl_feetWidth;
        delete txt_feetWidth;
        delete lbl_feetHeight;
        delete txt_feetHeight;
        delete leftPanel;
        delete midPanel;
        delete rightPanel;
        delete spriteNav;
        delete button;
        delete preview;
    }
};

#endif