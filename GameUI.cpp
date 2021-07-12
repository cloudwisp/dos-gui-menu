#ifndef GameUI_cpp
#define GameUI_cpp

#include <grx20.h>
#include "AppUI.h"
#include "AppEvent.h"
#include <time.h>

class HealthWidget : public UIDrawable {
private:
    GrColor mainColor;
    void draw_internal(){
        GrContext *curCtx = GrCurrentContext();
        GrSetContext(ctx);
        GrClearContextC(ctx,GrAllocColor(0,0,0));
        if (totalHP == 0){
            GrSetContext(curCtx);
            return;
        }
        GrBox(0,0,width-1,height-1,mainColor);
        double widthRatio = (double)currentHP/(double)totalHP;
        double hpWidth = (double)width*widthRatio;
        if (hpWidth == 0){
            GrSetContext(curCtx);
            return;
        }
        GrColor hbColor = mainColor;
        if (widthRatio <= .3){
            hbColor = GrAllocColor(255,0,0);
        }
        GrFilledBox(0,0,(int) hpWidth,height-1,hbColor);
        GrSetContext(curCtx);
    }
public:
    int totalHP = 100;
    int currentHP = 90;
    HealthWidget(GrColor color) : UIDrawable(20,6){
        mainColor = color;
    }
};

class TitleScreen: public UIWindow {

private:
    GrContext *titleImg = NULL;
    int imgLoaded = 0;
    char *titleImgPath = NULL;
    EventConsumer* _onDoneListener = NULL;

    void draw_internal(){
        if (!imgLoaded){
            titleImg = GameResources::LoadImage(titleImgPath);
            imgLoaded = 1;
        }
        GrBitBlt(ctx,0,0,titleImg,0,0,width-1,height-1,GrIMAGE);
    }

public:

    void OnKeyUp(int ScanCode){
        if (ScanCode == KEY_ENTER){
            CloseAndDestroy();
            _onDoneListener->OnEvent(this,"TitleScreenClosed",CreateEventData(0,0));
        }
        UIWindow::OnKeyUp(ScanCode);
    }

    TitleScreen(int screenWidth, int screenHeight, char *imgPath, EventConsumer* onDoneListener) : UIWindow(screenWidth, screenHeight) {
        titleImgPath = imgPath;
        _onDoneListener = onDoneListener;
    }
    ~TitleScreen(){

    }

};


class StoryScreen: public UIWindow {

private:
    GrContext *titleImg = NULL;
    int imgLoaded = 0;
    char *titleImgPath = NULL;
    EventConsumer* _onDoneListener = NULL;
    UIScrollingText* scrollContainer = NULL;
    UITextArea* scrollingText = NULL;
    int scrollSpeed = 100; //ms per pixel
    clock_t lastTick = clock();
    clock_t firstTick = clock();
    int delay = 2000;

    void draw_internal(){
        if (!imgLoaded){
            titleImg = GameResources::LoadImage(titleImgPath);
            imgLoaded = 1;
        }
        GrBitBlt(ctx,0,0,titleImg,0,0,width-1,height-1,GrIMAGE);
    }

public:

    void Update(){
        clock_t now = clock();
        int msSinceFirst = clockToMilliseconds(now-firstTick);
        if (msSinceFirst < delay){
            return;
        }

        int msSince = clockToMilliseconds(now-lastTick);
        
        if (msSince >= scrollSpeed){
            scrollContainer->ScrollDown();
            lastTick = now;
        }
    }

    void OnKeyUp(int ScanCode){
        if (ScanCode == KEY_ENTER){
            CloseAndDestroy();
            _onDoneListener->OnEvent(this,"StoryScreenClosed",CreateEventData(0,0));
        }
        UIWindow::OnKeyUp(ScanCode);
    }

    StoryScreen(int screenWidth, int screenHeight, char *imgPath, EventConsumer* onDoneListener) : UIWindow(screenWidth, screenHeight) {
        titleImgPath = imgPath;
        _onDoneListener = onDoneListener;
        int borderBuffer = 50;


        scrollingText = new UITextArea(screenWidth-(borderBuffer*2), 300);
        scrollingText->SetText("But I must explain to you how all this mistaken idea of denouncing pleasure and praising pain was born and I will give you a complete account of the system, and expound the actual teachings of the great explorer of the truth, the master-builder of human happiness. No one rejects, dislikes, or avoids pleasure itself, because it is pleasure, but because those who do not know how to pursue pleasure rationally encounter consequences that are extremely painful. Nor again is there anyone who loves or pursues or desires to obtain pain of itself, because it is pain, but because occasionally circumstances occur in which toil and pain can procure him some great pleasure. To take a trivial example, which of us ever undertakes laborious physical exercise, except to obtain some advantage from it? But who has any right to find fault with a man who chooses to enjoy a pleasure that has no annoying consequences, or one who avoids a pain that produces no resultant pleasure?");
        scrollContainer = new UIScrollingText(scrollingText, screenHeight-(borderBuffer*2));
        scrollContainer->x = borderBuffer;
        scrollContainer->y = borderBuffer;
        AddChild(scrollContainer);
    }
    ~StoryScreen(){
        delete scrollingText;
        delete scrollContainer;
    }

};

class GameEndScreen: public UIWindow {
private:
    GrContext *titleImg = NULL;
    EventConsumer *_onDoneListener = NULL;
    int imgLoaded = 0;
    char *titleImgPath = NULL;
    UITextArea *title = NULL;
    void draw_internal(){
        if (!imgLoaded){
            titleImg = GameResources::LoadImage(titleImgPath);
            imgLoaded = 1;
        }
        GrBitBlt(ctx,0,0,titleImg,0,0,width-1,height-1,GrIMAGE);
    }
public:

    void OnKeyUp(int ScanCode){
        if (ScanCode == KEY_ENTER){
            CloseAndDestroy();
            EmitEvent("WindowClosed");
        }
        UIWindow::OnKeyUp(ScanCode);
    }

    GameEndScreen() : UIWindow(UIWindowController::Get()->GetScreen()->width, UIWindowController::Get()->GetScreen()->height){
        titleImgPath = "screenbg.png";
        title = new UITextArea(300, 20);
        title->x = 46;
        title->y = 46;
        title->SetText("Game Completed!");
        AddChild(title);
    }
    ~GameEndScreen(){
        delete title;
    }
};

class StageEndScreen: public UIWindow {

private:
    GrContext *titleImg = NULL;
    EventConsumer *_onDoneListener = NULL;
    int imgLoaded = 0;
    float timeTaken = 0;
    char *titleImgPath = NULL;
    UITextArea *title = NULL;
    UITextArea *timeArea = NULL;
    char *timeDisp = NULL;
    void draw_internal(){
        if (!imgLoaded){
            titleImg = GameResources::LoadImage(titleImgPath);
            imgLoaded = 1;
        }
        GrBitBlt(ctx,0,0,titleImg,0,0,width-1,height-1,GrIMAGE);
    }
public:

    void OnKeyUp(int ScanCode){
        if (ScanCode == KEY_ENTER){
            CloseAndDestroy();
            _onDoneListener->OnEvent(this,"StageCompleteClosed",CreateEventData(0,0));
        }
        UIWindow::OnKeyUp(ScanCode);
    }

    StageEndScreen(std::string stageCompleteTitle, float secs, EventConsumer *onDoneListener) : UIWindow(320, 200) {
        _onDoneListener = onDoneListener;
        titleImgPath = "screenbg.png";
        timeTaken = secs;
        title = new UITextArea(300, 20);
        title->x = 46;
        title->y = 46;
        timeArea = new UITextArea(100,50);
        timeArea->x = 46;
        timeArea->y = 70;
        timeDisp = (char*) malloc(sizeof(char)*100);
        sprintf(timeDisp,"Time taken: %f.00",secs);
        timeArea->SetText(timeDisp);
        title->SetText((char*)stageCompleteTitle.c_str());
        AddChild(title);
        AddChild(timeArea);
    }
    ~StageEndScreen(){
        free(timeDisp);
        delete title;
        delete timeArea;
    }

};


#endif // GameUI_cpp
