#ifndef CharEditorApp_cpp
#define CharEditorApp_cpp

#include "../App.cpp"
#include "../keyboard.h"
#include <string>
#include <std.h>
#include <memory.h>
#include "CharEditUI.cpp"

class CharEditorApp : public CWApplication {
private:

    UITextArea* lbl_feetWidth = NULL;
    UITextArea* lbl_feetHeight = NULL;
    UITextBox* txt_feetWidth = NULL;
    UITextBox* txt_feetHeight = NULL;
    UIButton* btn_spriteset = NULL;
    UIButton* btn_char = NULL;
    UIWindow* mainWindow = NULL;
    UIStackedPanel* leftPanel = NULL;
    UIStackedPanel* midPanel = NULL;
    UIStackedPanel* rightPanel = NULL;
    UISpriteSheetNavigator* spriteNav = NULL;
    UISpritePreview* preview = NULL;
    UISpritePreview* anim = NULL;
    UIModalWindow *openModal = NULL;
    UITextBox *modalText = NULL;
    SpriteSet* loadedSpriteset = NULL;
    GameCharModel* loadedChar = NULL;
    UIListBox* animList = NULL;
    int lastFeetWidth = 0;
    int lastFeetHeight = 0;
    int openModalMode = 0;
    int activeAnimDir = 1;
    int lastAnimFrame = 0;
    const int MODAL_MODE_CHAR = 0;
    const int MODAL_MODE_SPRITESET = 1;

    void check_inputs(int *cancelInputPropagation){
        /*UINT16 ShiftState = Get_Shift_State();
        char* ctext = "";
        sprintf(ctext, "Shift State "PRINTF_BINARY_PATTERN_INT16, PRINTF_BYTE_TO_BINARY_INT16(ShiftState));
        debugOut(std::string(ctext));*/
    };

    void BuildOpenModal(int mode){
        UIAppScreen* scr = UIAppScreen::Get();
        std::string title;
        if (mode == MODAL_MODE_CHAR){
            title = "Open character (name without extension)";
        } else {
            title = "Open spriteset (name without extension)";
        }
        openModal = new UIModalWindow(scr->width * 0.8, scr->height * 0.8, title);
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
            BuildOpenModal(MODAL_MODE_CHAR);
        }
    };

	void update(){

        if (loadedChar == NULL || loadedSpriteset == NULL){
            return;
        }
        int nextAnimFrame;
        if (lastAnimFrame == 19){
            nextAnimFrame = 0;
        } else {
            nextAnimFrame = lastAnimFrame+1;
        }
        lastAnimFrame = nextAnimFrame;
        int spriteId = loadedChar->sprites[activeAnimDir][nextAnimFrame];
        if (spriteId < 0){
            spriteId = 0;
            lastAnimFrame = 0;
        }
        anim->UpdateImage(loadedSpriteset, loadedChar->sprites[activeAnimDir][nextAnimFrame]);
    };
	void render(){

    };
	void on_start(){

    };

    void loadChar(std::string charName){
        loadedChar = GameResources::GetGameCharModel(charName);
        loadSpriteset(loadedChar->spriteset->id);
        char strbuf[20];
        itoa(loadedChar->groundClipWidth,strbuf,10);
        txt_feetWidth->SetText(std::string(strbuf));
        itoa(loadedChar->groundClipHeight,strbuf,10);
        txt_feetHeight->SetText(std::string(strbuf));
        
    }

    void loadSpriteset(std::string spritesetName){
        loadedSpriteset = GameResources::GetSpriteSet(spritesetName);
        spriteNav->SetSpriteSet(loadedSpriteset);
        preview->UpdateImage(loadedSpriteset, 0);
    }

public:

    void OnEvent(EventEmitter* source, std::string event, EventData data){
        if (source == btn_spriteset && event == "Click"){
            //todo
            BuildOpenModal(MODAL_MODE_SPRITESET);
            return;
        }

        if (source == btn_char && event == "Click"){
            BuildOpenModal(MODAL_MODE_CHAR);
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
            std::string typedName = std::string(modalText->GetText());
            if (openModalMode == MODAL_MODE_SPRITESET){
                loadSpriteset(typedName);
            } else if (openModalMode == MODAL_MODE_CHAR) {
                loadChar(typedName);
            }
            
            DestroyModalControls();
            return;
        }
        if (source == openModal && event == "Cancel"){
            DestroyModalControls();
            return;
        }

        if (source == animList && event == "SelectedItemChanged"){
            activeAnimDir = data.data1;
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

        btn_char = new UIButton(100, 20);
        btn_char->SetText("Open Character");
        btn_char->BindEvent("Click", this);
        btn_char->tabstop = 10;

        leftPanel->AddChild(btn_char);
        

        lbl_feetWidth = new UITextArea(100,10);
        lbl_feetWidth->SetText("Feet Clip Width:");
        leftPanel->AddChild(lbl_feetWidth);

        txt_feetWidth = new UITextBox(100,20,30);
        txt_feetWidth->SetText("1");
        txt_feetWidth->BindEvent("Changed", this);
        txt_feetWidth->SetIntMode(true);
        txt_feetWidth->tabstop = 20;
        leftPanel->AddChild(txt_feetWidth);

        lbl_feetHeight = new UITextArea(100,10);
        lbl_feetHeight->SetText("Feet Clip Height:");
        leftPanel->AddChild(lbl_feetHeight);

        txt_feetHeight = new UITextBox(100,20, 30);
        txt_feetHeight->SetText("1");
        txt_feetHeight->BindEvent("Changed", this);
        txt_feetHeight->SetIntMode(true);
        txt_feetHeight->tabstop = 30;
        leftPanel->AddChild(txt_feetHeight);

        animList = new UIListBox(100, leftPanel->height - txt_feetHeight->height - lbl_feetHeight->height - txt_feetWidth->height - lbl_feetWidth->height);
        animList->AddItem("Idle");
        animList->AddItem("North");
        animList->AddItem("Northeast");
        animList->AddItem("East");
        animList->AddItem("Southeast");
        animList->AddItem("South");
        animList->AddItem("Southwest");
        animList->AddItem("West");
        animList->AddItem("Northwest");
        animList->AddItem("Dead");
        animList->tabstop = 35;
        animList->BindEvent("SelectedItemChanged", this);
        leftPanel->AddChild(animList);

        spriteNav = new UISpriteSheetNavigator(midPanel->width, midPanel->height);
        spriteNav->BindEvent("SpriteChanged", this);
        spriteNav->tabstop = 40;
        
        midPanel->AddChild(spriteNav);

        preview = new UISpritePreview(100, 100);
        rightPanel->AddChild(preview);

        anim = new UISpritePreview(20,20);
        rightPanel->AddChild(anim);
        
        btn_spriteset = new UIButton(100, 20);
        btn_spriteset->SetText("Change Spriteset");
        btn_spriteset->tabstop = 50;
        btn_spriteset->BindEvent("Click", this);

        rightPanel->AddChild(btn_spriteset);        
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
        delete btn_char;
        delete btn_spriteset;
        delete preview;
        delete anim;
    }
};

#endif