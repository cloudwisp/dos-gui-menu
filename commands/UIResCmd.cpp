#ifndef UIResCmd_CPP
#define UIResCmd_CPP

#include "../ScrptCmd.cpp"

class PreLoadImageCommand : public ScriptCommand, public EventConsumer{
private:
    std::string msgText;
    std::vector<std::string> imageNames;
    bool withLoadingMsg = false;
    UITextWindow *dialog = NULL;
    int delay = 100;
    int tick = 0;
public:
    void OnCommandStart(){
        if (withLoadingMsg && !dialog){
            dialog = new UITextWindow((char*)msgText.c_str(), UIWindowController::Get()->GetScreen()->width, UIWindowController::Get()->GetScreen()->height);
            UIWindowController::Get()->AddWindow(dialog,1);
        }
    }

    int OnCommandUpdate(){
        tick++;
        if (tick < delay){
            return 0;
        }
        if (imageNames.size() > 0){
            GameResources::LoadImage(imageNames.back());
            imageNames.pop_back();
            return 0;
        } else {
            return 1;
        }
    }
    void OnCommandEnd(){
        if (dialog){
            dialog->CloseAndDestroy();
        }
    }

    PreLoadImageCommand(bool displayLoadingMsg, std::string loadingMsg, std::vector<std::string> imagesToLoad) : ScriptCommand() {
        withLoadingMsg = displayLoadingMsg;
        msgText = loadingMsg;
        imageNames = imagesToLoad;
    }
};


#endif // UIResCmd_CPP
