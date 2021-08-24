#ifndef SelectorApp_CPP
#define SelectorApp_CPP

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <pc.h>

#include "Common.h"
#include "AppUI.cpp"
#include "keyboard.h"
#include "App.cpp"
#include "SelApp.h"

class SelectorMainWindow : public UIWindow {
private:

    CWApplication* app = NULL;

    //main panels
    UIImagePanel* screenshot = NULL;
    UIPanel *titleBar = NULL;
    UIPanel *gameList = NULL;
    UIPanel *gameDetail = NULL;

    //fonts & dimensions
    GrFont* titleFont = NULL;
    GrFont* titleFontBold = NULL;
    int titleFontHeight = 0;
    GrFont* textFont = NULL;
    GrFont* textFontBold = NULL;
    int textFontHeight = 0;
    GrFont* smallFont = NULL;
    GrFont* smallFontBold = NULL;
    int smallFontHeight = 0;
    int defaultMargin = 0;
    int defaultButtonBorderWidth = 0;

    int leftPaneWidth = 0;
    int rightPaneWidth = 0;


    //items
    std::vector<DatabaseItem*> *dbItems;
    std::vector<UIButton*> buttons;
    int activeButton = 0;

    void _ActivateItem(int itemId){
        activeButton = itemId;
        buttons[itemId]->Focus();
        screenshot->SetImage((*dbItems)[itemId]->image);
    }

public:

    void OnEvent(EventEmitter *source, std::string event, EventData data){
        debugOut(event);
        for (int i = 0; i < buttons.size(); i++){
            if (source == buttons[i]){
                //launch game here
                _ActivateItem(i);
            }
            debugOut("couldn't find source");
        }
        UIWindow::OnEvent(source, event, data);
    }

    void CheckInputs(){
        UIWindow::CheckInputs();
    }

    void OnKeyUp(int ScanCode, int ShiftState, int Ascii){
        if (ScanCode == KEY_UP_ARROW && activeButton > 0){
            _ActivateItem(activeButton-1);
        } else if (ScanCode == KEY_DOWN_ARROW && activeButton < buttons.size()-1){
            _ActivateItem(activeButton+1);
        }
        UIWindow::OnKeyUp(ScanCode, ShiftState, Ascii);
    }

    void LoadItems(){
        dbItems = AppResources::GetDatabaseItems("games.db");
        char* buf = (char*) malloc(sizeof(char) * 1000);
        sprintf(buf, "dbItems count %d", dbItems->size());
        debugOut(std::string(buf));
        free(buf);
        for (int i = 0; i < dbItems->size(); i++){
            UIButton* button = new UIButton(leftPaneWidth-defaultMargin-defaultMargin,textFontHeight+defaultMargin+defaultMargin);
            button->SetFont(textFont);
            button->SetColor(GrAllocColor(10,10,10),GrWhite());
            button->SetText((char*) (*dbItems)[i]->name.c_str());
            button->x = defaultMargin;
            int margin = 0;
            if (i > 0){
                margin = defaultMargin*2;
            } else {
                margin = defaultMargin;
            }
            button->y = (i * button->height) + margin;
            button->BindEvent("Click", this);
            gameList->AddChild(button);
            buttons.push_back(button);

        }
    }

    SelectorMainWindow(int drawWidth, int drawHeight, CWApplication* mainApp) : UIWindow(drawWidth, drawHeight){
        app = mainApp;

        leftPaneWidth = drawWidth * 0.3;
        rightPaneWidth = drawWidth-leftPaneWidth;

        GrFont* titleFont;

        if (drawHeight == 600){
            titleFont = GrLoadFont("fonts/helv22.fnt");
            titleFontBold = GrLoadFont("fonts/helv22b.fnt");
            titleFontHeight = 22;
            textFont = GrLoadFont("fonts/helv15.fnt");
            textFontBold = GrLoadFont("fonts/helv15b.fnt");
            textFontHeight = 15;
            smallFont = GrLoadFont("fonts/helv13.fnt");
            smallFontBold = GrLoadFont("fonts/helv13b.fnt");
            smallFontHeight = 13;
            defaultMargin = 10;
            defaultButtonBorderWidth = 4;
        } else if (drawHeight == 480) {
            titleFont = GrLoadFont("fonts/helv15.fnt");
            titleFontBold = GrLoadFont("fonts/helv15b.fnt");
            titleFontHeight = 15;
            textFont = GrLoadFont("fonts/helv13.fnt");
            textFontBold = GrLoadFont("fonts/helv13b.fnt");
            textFontHeight = 13;
            smallFont = GrLoadFont("fonts/helv11.fnt");
            smallFontBold = GrLoadFont("fonts/helv11b.fnt");
            smallFontHeight = 11;
            defaultMargin = 8;
            defaultButtonBorderWidth = 2;
        } else {
            titleFont = GrLoadFont("fonts/helv13.fnt");
            titleFontBold = GrLoadFont("fonts/helv13b.fnt");
            titleFontHeight = 13;
            textFont = GrLoadFont("fonts/helv11.fnt");
            textFontBold = GrLoadFont("fonts/helv11b.fnt");
            textFontHeight = 11;
            smallFont = GrLoadFont("fonts/helv11.fnt");
            smallFontBold = GrLoadFont("fonts/helv11b.fnt");
            smallFontHeight = 11;
            defaultMargin = 8;
            defaultButtonBorderWidth = 1;
        }

        int titleHeight = titleFontHeight + defaultMargin + defaultMargin;

        gameDetail = new UIPanel(GrAllocColor(10,10,10),rightPaneWidth,drawHeight-titleHeight);
        gameDetail->x = leftPaneWidth;
        gameDetail->y = titleHeight;
        AddChild(gameDetail);


        screenshot = new UIImagePanel(rightPaneWidth/2,rightPaneWidth*0.666);
        screenshot->x = defaultMargin;
        screenshot->y = defaultMargin;
        gameDetail->AddChild(screenshot);
        screenshot->SendToBack();

        //title bar
        titleBar = new UIPanel(GrAllocColor(50,50,50),drawWidth,titleHeight);
        UITextArea *title = new UITextArea(leftPaneWidth,titleHeight-defaultMargin-defaultMargin);
        title->SetFont(titleFontBold);
        title->SetColor(GrWhite(), GrNOCOLOR);
        title->SetText("Game Selector");
        title->x = defaultMargin;
        title->y = defaultMargin;
        titleBar->AddChild(title);
        AddChild(titleBar);

        //game list
        gameList = new UIPanel(GrAllocColor(120,120,120),leftPaneWidth,drawHeight-titleHeight);
        gameList->x = 0;
        gameList->y = titleHeight;
        AddChild(gameList);

    }

};


class SelectorApplication : public CWApplication {
private:

    UIWindow* mainWindow = NULL;

	void on_start(){
        ((SelectorMainWindow*) mainWindow)->LoadItems();
	}

	void render(){

	}

	void update(){


	}

	void check_inputs(int *cancelInputPropagation){

	}

	void on_keyup(int ScanCode, int ShiftState, int Ascii, int *cancelInputPropagation){
        if (ScanCode == KEY_ESC){
            End();
        }
	}


public:

	void OnEvent(EventEmitter *source, std::string event, EventData data){
		CWApplication::OnEvent(source,event,data);
	}

	SelectorApplication(int screenWidth, int screenHeight): CWApplication (screenWidth, screenHeight, 16, MS_PER_UPDATE) {
        mainWindow = new SelectorMainWindow(screenWidth, screenHeight, this);
		AddWindow(mainWindow,1);
        mainWindow->BringToFront();

	}
	~SelectorApplication(){

	    if (mainWindow){ delete mainWindow; }
	}

};

#endif
