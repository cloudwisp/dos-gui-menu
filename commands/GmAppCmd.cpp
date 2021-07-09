#ifndef GmAppCmd_cpp
#define GmAppCmd_cpp


#include "../ScrptCmd.cpp"

class LoadWorldCommand : public ScriptCommand, public EventConsumer{
private:
    std::string msgText;
    UITextWindow *dialog = NULL;
    int delay = 100;
    int tick = 0;
    int world = 0;
    bool done = false;
    bool changeEventSent = false;
public:
    void OnCommandStart(){
        dialog = new UITextWindow((char*)msgText.c_str(), UIWindowController::Get()->GetScreen()->width, UIWindowController::Get()->GetScreen()->height);
        UIWindowController::Get()->AddWindow(dialog,1);
        CWApplication::GetApplication()->BindEvent("WorldChanged",this);
    }

    int OnCommandUpdate(){
        tick++;
        if (tick < delay){
            return 0;
        }
        if (!changeEventSent){
            CWApplication::GetApplication()->EmitEvent("ChangeWorld",world);
            changeEventSent = true;
            return 0;
        }
        if (done){
            return 1;
        }
        return 0;
    }
    void OnCommandEnd(){
        dialog->CloseAndDestroy();
        CWApplication::GetApplication()->UnbindAllEventsForConsumer(this);
    }

    void OnEvent(EventEmitter* source, std::string event, EventData data){
        if (event == "WorldChanged"){
            done = true;
        }
    }

    LoadWorldCommand(int world_id, std::string loadingMsg) : ScriptCommand() {
        msgText = loadingMsg;
        world = world_id;
    }
};


#endif // GmAppCmd
