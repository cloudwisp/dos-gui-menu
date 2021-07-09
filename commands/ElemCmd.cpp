#ifndef ElemCmd_CPP
#define ElemCmd_CPP

#include "../ScrptCmd.cpp"

class TeleportElementCommand : public ScriptCommand {
private:
    GameWorldElement *element = NULL;
    int x;
    int y;
public:

    void OnCommandStart(){
        element->SetPosition(x, y);
    }

    int OnCommandUpdate(){
        return 1;
    }
    void OnCommandEnd(){
    }
    TeleportElementCommand(GameWorldElement *elem, int posX, int posY){
        element = elem;
        x = posX;
        y = posY;
    }
};


#endif // ElemCmd_CPP
