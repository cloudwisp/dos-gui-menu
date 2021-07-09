#ifndef UICmd_CPP
#define UICmd_CPP

#include "../ScrptCmd.cpp"

class ShowDialogCommand : public ScriptCommand{
private:
    char *msgText = NULL;
    int width;
    int height;
    int dialogClosed = 0;
    UIMsgBox *dialog = NULL;
public:
    void OnCommandStart(){
        dialog = UIMsgBox::Create(msgText, width, height);
    }

    int OnCommandUpdate(){
        if (dialog == NULL){
            //has been closed by the user, allow return
            return 1;
        } else if (dialog->closed){
            return 1;
        }
        return 0;
    }
    void OnCommandEnd(){
    }
    ShowDialogCommand(char *text, int drawWidth, int drawHeight) : ScriptCommand() {
        width = drawWidth;
        height = drawHeight;
        msgText = text;
    }
};


#endif // UICmd_CPP
