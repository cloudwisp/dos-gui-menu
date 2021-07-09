
#ifndef GamUICmd_CPP
#define GamUICmd_CPP

#include "../ScrptCmd.cpp"
#include "../GameUI.cpp"

class ShowTitleScreenCommand : public ScriptCommand, public EventConsumer {
private:
    int dialogClosed = 0;
    char *imgPath = NULL;
    bool done = false;
    TitleScreen *window = NULL;
public:
    void OnCommandStart(){
        UIAppScreen *screen = UIWindowController::Get()->GetScreen();
        window = new TitleScreen(screen->width,screen->height,imgPath,(EventConsumer*) this);
		UIWindowController::Get()->AddWindow(window,1);
    }

    int OnCommandUpdate(){
        if (done){
            return 1;
        }
        return 0;
    }

    void OnCommandEnd(){

    }

    void OnEvent(EventEmitter* source, std::string event, EventData data){
        done = true;
    }

    ShowTitleScreenCommand(char *titleImagePath) : ScriptCommand(), EventConsumer() {
        imgPath = titleImagePath;
    }
};


#endif // GamUICmd_CPP
