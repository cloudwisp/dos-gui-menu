#ifndef GameUI_cpp
#define GameUI_cpp

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

    StageEndScreen(char *stageCompleteTitle, float secs, EventConsumer *onDoneListener) : UIWindow(320, 200) {
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
        title->SetText(stageCompleteTitle);
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
