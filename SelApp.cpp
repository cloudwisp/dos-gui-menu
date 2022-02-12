#ifndef SelectorApp_CPP
#define SelectorApp_CPP

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <pc.h>
#include <algorithm>

#include "Common.h"
#include "AppUI.cpp"
#include "keyboard.h"
#include "App.cpp"
#include "SelApp.h"
#include "SelUI.cpp"
#include "AppOptions.cpp"

bool dbsort(DatabaseItem *item1, DatabaseItem *item2){
    return item1->name < item2->name;
}

class SelectorMainWindow : public UIWindow {
private:

    CWApplication* app = NULL;

    //main panels
    UIImagePanel* screenshot = NULL;
    UIImagePanel *titleBar = NULL;
    UIImagePanel *gameList = NULL;
    UIImagePanel *gameDetail = NULL;
    UIStackedPanel *detailRight = NULL;
    UIStackedPanel *detailLeft = NULL;
    UIScrollingListBox *gameListItems = NULL;
    UITextArea *gameTitle = NULL;
    UIScrollingText *gameDescription = NULL;
    UITextArea* gameDescriptionInner = NULL;
    UIButton *launch = NULL;
    UILabelAndText *gameGenre = NULL;
    UILabelAndText *gameNotes = NULL;
    UILabelAndText *publishedYear = NULL;
    UILabelAndText *developer = NULL;

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
    int titleBarHeight = 0;
    int detailLabelHeight = 0;
    int detailLabelPadding = 0;

    int leftPaneWidth = 0;
    int rightPaneWidth = 0;

    DatabaseItem* GetItem(int itemId){
        return (*dbItems)[itemId];
    }

    //items
    std::vector<DatabaseItem*> *dbItems;
    int activeItem = 0;

    void _ActivateItem(int itemId){
        activeItem = itemId;
        DatabaseItem* thisItem = (*dbItems)[itemId];
        gameTitle->SetText(thisItem->name);
        gameGenre->SetText(thisItem->genre);
        gameNotes->SetText(thisItem->notes);
        developer->SetText(thisItem->developer);
        publishedYear->SetText(thisItem->year);
        if (thisItem->readme != ""){
            gameDescriptionInner->SetText(AppResources::GetReadme(thisItem->readme));
        } else if (thisItem->inlineDescription) {
            gameDescriptionInner->SetText(AppResources::GetInlineDescription(thisItem->folder + "\\_menu.cfg", thisItem->name));
        } else {
            gameDescriptionInner->SetText("");
        }

        gameDescription->ScrollToTop();
        gameDescriptionInner->SizeHeightToContent();
        

        if (thisItem->image != ""){
            screenshot->SetImage(thisItem->image, 500);
        } else {
            screenshot->SetImage("noimage.png");
        }
    }

    void _LaunchItem(int itemId){
        DatabaseItem* thisItem = GetItem(itemId);
        AppResources::WriteLaunchBat(thisItem->path, thisItem->name);
        app->End();
    }

public:

    void OnEvent(EventEmitter *source, std::string event, EventData data){
        if (event == "SelectedItemChanged"){
            if (data.data1 > dbItems->size() - 1){
                return;
            }
            _ActivateItem(data.data1);
        }
        if (event == "Click" && source == launch){
            _LaunchItem(activeItem);
            return;
        }
        UIWindow::OnEvent(source, event, data);
    }

    void CheckInputs(){
        
        UIWindow::CheckInputs();
    }

    void LoadItems(){
        dbItems = AppResources::GetCachedMenuItems();
        if (dbItems->size() == 0){
            dbItems = AppResources::ScanMenuItemFiles();
        }
        
        string defaultItem = AppResources::GetDefaultItem();
        sort(dbItems->begin(), dbItems->end(), dbsort);
        for (int i = 0; i < dbItems->size(); i++){
            gameListItems->AddItem((*dbItems)[i]->name);
        }

        if (defaultItem != ""){
            for (int i = 0; i < dbItems->size(); i++){
                if (defaultItem == (*dbItems)[i]->name){
                    gameListItems->SetSelectedItem(i);
                    break;
                }
            }
        }
    }

    void OnKeyUp(int ScanCode, int ShiftState, int Ascii){
        if (ScanCode == KEY_ESC){
            app->End();
        }
        UIWindow::OnKeyUp(ScanCode, ShiftState, Ascii);
    }

    SelectorMainWindow(int drawWidth, int drawHeight, CWApplication* mainApp) : UIWindow(drawWidth, drawHeight){
        app = mainApp;

        leftPaneWidth = drawWidth * 0.3;
        rightPaneWidth = drawWidth-leftPaneWidth;

        GrFont* titleFont;
        bool scaleHeaderHeight = false;
        bool shortLabels = false;
        if (drawHeight == 600){
            titleFont = GrLoadFont("helv22.fnt");
            titleFontBold = GrLoadFont("helv22b.fnt");
            titleBarHeight = 32;
            titleFontHeight = 22;
            textFont = GrLoadFont("helv15.fnt");
            textFontBold = GrLoadFont("helv15b.fnt");
            textFontHeight = 15;
            smallFont = GrLoadFont("helv13.fnt");
            smallFontBold = GrLoadFont("helv13b.fnt");
            smallFontHeight = 13;
            defaultMargin = 10;
            detailLabelHeight = 20;
            defaultButtonBorderWidth = 4;
            detailLabelPadding = 2;
        } else if (drawHeight == 480) {
            titleFont = GrLoadFont("helv15.fnt");
            titleFontBold = GrLoadFont("helv15b.fnt");
            titleFontHeight = 15;
            titleBarHeight = 22;
            textFont = GrLoadFont("helv13.fnt");
            textFontBold = GrLoadFont("helv13b.fnt");
            textFontHeight = 13;
            smallFont = GrLoadFont("helv11.fnt");
            smallFontBold = GrLoadFont("helv11b.fnt");
            smallFontHeight = 11;
            defaultMargin = 8;
            detailLabelHeight = 20;
            defaultButtonBorderWidth = 2;
            detailLabelPadding = 2;
        } else {
            scaleHeaderHeight = true;
            titleFont = GrLoadFont("helv13.fnt");
            titleFontBold = GrLoadFont("helv13b.fnt");
            titleFontHeight = 13;
            titleBarHeight = 16;
            textFont = GrLoadFont("helv11.fnt");
            textFontBold = GrLoadFont("helv11b.fnt");
            textFontHeight = 11;
            shortLabels = true;
            smallFont = GrLoadFont("helv11.fnt");
            smallFontBold = GrLoadFont("helv11b.fnt");
            smallFontHeight = GrFontCharHeight(smallFont,"Y");
            detailLabelPadding = 0;
            defaultMargin = 2;
            detailLabelHeight = smallFontHeight;
            defaultButtonBorderWidth = 1;
        }

        int gameListHeight = drawHeight - titleBarHeight;
        int titleHeight = titleFontHeight + defaultMargin + defaultMargin;
        int rightPaneHeight = drawHeight - titleBarHeight;
        int gameTitleHeight = titleHeight;
        int rightPaneLowerHeight = rightPaneHeight - gameTitleHeight;
        int detailLeftWidth = rightPaneWidth * 0.66;
        int detailLeftWidthInner = detailLeftWidth - (defaultMargin * 2);
        int detailRightWidth = rightPaneWidth - detailLeftWidth;
        int detailRightWidthInner = detailRightWidth - (defaultMargin * 2);
        
        
        //int detailRightWidthInner = detailRightWidth - (defaultMargin * 2);
        double labelPercent = 30;
        gameDetail = new UIImagePanel(rightPaneWidth,rightPaneHeight);
        gameDetail->scaleToWidth = true;
        gameDetail->scaleToHeight = true;
        gameDetail->SetImage("darkbg3.png");
        gameDetail->x = leftPaneWidth;
        gameDetail->y = titleBarHeight;
        AddChild(gameDetail);

        detailLeft = new UIStackedPanel(THEME_COLOR_TRANSPARENT, detailLeftWidth, rightPaneLowerHeight, defaultMargin);
        detailLeft->y = gameTitleHeight;
        detailLeft->containertabstop = 2;
        gameDetail->AddChild(detailLeft);

        detailRight = new UIStackedPanel(THEME_COLOR_TRANSPARENT, detailRightWidth, rightPaneLowerHeight, defaultMargin, defaultMargin);
        detailRight->y = gameTitleHeight;
        detailRight->x = detailLeft->width;
        detailRight->containertabstop = 3;
        gameDetail->AddChild(detailRight);

        screenshot = new UIImagePanel(detailRightWidthInner,detailRightWidthInner*1.4);
        screenshot->preserveColors = false;
        screenshot->progressive = true;
        screenshot->SetLoadingFont(textFont);
        detailRight->AddChild(screenshot);
        screenshot->SendToBack();

        //title bar
        //titleBar = new UIPanel(THEME_WINDOW_TITLE_BACKGROUND_COLOR,drawWidth,titleHeight);
        titleBar = new UIImagePanel(drawWidth, titleBarHeight);
        titleBar->scaleToHeight = scaleHeaderHeight;
        titleBar->scaleToWidth = !scaleHeaderHeight;
        titleBar->SetImage("header.png");
        AddChild(titleBar);

        //game list
        gameList = new UIImagePanel(leftPaneWidth,gameListHeight);
        gameList->scaleToHeight = true;
        gameList->scaleToWidth = true;
        gameList->SetImage("lightbg2.png");
        gameList->x = 0;
        gameList->y = titleBarHeight;
        gameList->containertabstop = 1;
        AddChild(gameList);

        gameListItems = new UIScrollingListBox(leftPaneWidth-(defaultMargin*2), gameListHeight-(defaultMargin*2), smallFont);
        gameListItems->BindEvent("SelectedItemChanged", this);
        gameListItems->SetColor(THEME_CONTROL_TEXT, THEME_COLOR_TRANSPARENT);
        gameListItems->x = defaultMargin;
        gameListItems->y = defaultMargin;
        gameListItems->tabstop = 1;
        gameList->AddChild(gameListItems);
        gameListItems->Focus();

        gameTitle = new UITextArea(rightPaneWidth, gameTitleHeight, defaultMargin);
        gameTitle->SetFont(titleFontBold);
        gameTitle->SetColor(THEME_PANEL_TEXT_PRIMARY, THEME_COLOR_TRANSPARENT);
        gameTitle->SetAlign(GR_ALIGN_LEFT, GR_ALIGN_CENTER);
        gameTitle->x = 0;
        gameTitle->y = 0;

        gameDetail->AddChild(gameTitle);

        gameGenre = new UILabelAndText(detailLeftWidthInner, detailLabelHeight, detailLabelHeight, labelPercent, detailLabelPadding);
        gameGenre->SetLabelFont(smallFontBold);
        gameGenre->SetTextFont(smallFont);
        gameGenre->SetLabel("Genre");
        detailLeft->AddChild(gameGenre);

        gameNotes = new UILabelAndText(detailLeftWidthInner, detailLabelHeight, detailLabelHeight * 2, labelPercent, detailLabelPadding);
        gameNotes->SetLabelFont(smallFontBold);
        gameNotes->SetTextFont(smallFont);
        gameNotes->SetLabel("Notes");
        detailLeft->AddChild(gameNotes);

        developer = new UILabelAndText(detailLeftWidthInner, detailLabelHeight, detailLabelHeight, labelPercent, detailLabelPadding);
        developer->SetLabelFont(smallFontBold);
        developer->SetTextFont(smallFont);
        if (shortLabels){
            developer->SetLabel("Dev.");
        } else {
            developer->SetLabel("Developer");
        }
        
        detailLeft->AddChild(developer);

        publishedYear = new UILabelAndText(detailLeftWidthInner, detailLabelHeight, detailLabelHeight, labelPercent, detailLabelPadding);
        publishedYear->SetLabelFont(smallFontBold);
        publishedYear->SetTextFont(smallFont);
        publishedYear->SetLabel("Year");
        detailLeft->AddChild(publishedYear);

        

        int usedHeight = detailLeft->GetTailY();
        gameDescription = new UIScrollingText(detailLeftWidthInner, rightPaneLowerHeight - usedHeight - (2*defaultMargin));
        gameDescription->tabstop = 1;
        gameDescription->x = 0;

        gameDescriptionInner = gameDescription->GetText();
        gameDescriptionInner->SetFont(smallFont);
        gameDescriptionInner->SetColor(THEME_PANEL_TEXT_PRIMARY, THEME_COLOR_TRANSPARENT);

        detailLeft->AddChild(gameDescription);
        
        launch = new UIButton(detailRightWidthInner, detailLabelHeight);
        launch->SetText("Launch!");
        launch->x = 0;
        launch->BindEvent("Click", this);
        launch->tabstop = 1;
        launch->SetFont(smallFont);
        detailRight->AddChild(launch);
    }

};


class SelectorApplication : public CWApplication {
private:

    UIWindow* mainWindow = NULL;
    UITitledWindow *options = NULL;
	void on_start(){
        ((SelectorMainWindow*) mainWindow)->LoadItems();
	}

	void render(){

	}

	void update(){


	}

	void check_inputs(int *cancelInputPropagation){
        if ((KeyState(KEY_LEFT_CONTROL) || KeyState(KEY_RIGHT_CONTROL)) && KeyState(KEY_O)){
            options->Open();
        }
	}

	void on_keyup(int ScanCode, int ShiftState, int Ascii, int *cancelInputPropagation){
        // if (ScanCode == KEY_ESC){
        //     End();
        // }
	}


public:

	void OnEvent(EventEmitter *source, std::string event, EventData data){
		CWApplication::OnEvent(source,event,data);
	}

	SelectorApplication(int screenWidth, int screenHeight): CWApplication (screenWidth, screenHeight, MS_PER_UPDATE) {
        if (screenHeight == 600){
            THEME_WINDOW_TITLE_FONT = "helv14.fnt";
            THEME_WINDOW_TITLE_HEIGHT = 18;
        } else if (screenHeight == 480){
            THEME_WINDOW_TITLE_FONT = "helv13.fnt";
            THEME_WINDOW_TITLE_HEIGHT = 16;
        } else {
            THEME_WINDOW_TITLE_FONT = "helv11.fnt";
            THEME_WINDOW_TITLE_HEIGHT = 14;
        }
        int optWidth = screenWidth / 2;
        int optHeight = screenHeight / 2;
        options = new AppOptionsWindow(optWidth, optHeight);
        options->SetPosition(optWidth / 2, optHeight / 2);
        AddWindow(options, 0);
        mainWindow = new SelectorMainWindow(screenWidth, screenHeight, this);
		AddWindow(mainWindow,1);
	}
	~SelectorApplication(){

	    if (mainWindow){ delete mainWindow; }
        if (options){ delete options; }
	}

};

#endif
