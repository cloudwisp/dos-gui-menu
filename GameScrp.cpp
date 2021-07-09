#ifndef GameScrp_cpp
#define GameScrp_cpp

#include "ScrptCmd.cpp"

class GameScript;
GameScript* __gameScript;

class GameScript {
private:
    ScriptCommand* commands[30] = {NULL};
    int commandCount = 0;
    ScriptCommand* activeCommand = NULL;
public:

    static GameScript* Get();
    static void Destroy();

    void AddCommand(ScriptCommand *cmd){
        commands[commandCount] = cmd;
        commandCount++;
    }

    void RemoveCommand(ScriptCommand *cmd){
        int i, z, removed;
        removed = 0;
        for (i = commandCount-1; i >= 0; i--){
            if (commands[i] == cmd){
                for (z = i+1; z < commandCount-removed; z++){
                    commands[z-1] = commands[z];
                    commands[z] = NULL;
                }
                removed++;
            }
        }
        commandCount-=removed;
    }

    int NeedsControl(){
        if (commandCount > 0){
            return 1;
        } else {
            return 0;
        }
    }

    void CheckInputs(){
        if (activeCommand != NULL){
            activeCommand->CheckInputs();
        }
    }

    void OnKeyUp(int ScanCode){
        if (activeCommand != NULL){
            activeCommand->OnKeyUp(ScanCode);
        }
    }

    void UpdateExecute(){
        if (activeCommand == NULL && commandCount > 0){
            activeCommand = commands[0]; //first in, first out
            activeCommand->OnCommandStart();
        }
        if (activeCommand != NULL){
            int cmdDone = 0;
            cmdDone = activeCommand->OnCommandUpdate();
            if (cmdDone){
                activeCommand->OnCommandEnd();
                RemoveCommand(activeCommand);
                delete activeCommand;
                activeCommand = NULL;
            }
        }
    }

    GameScript(){
        __gameScript = this;
    }
    ~GameScript(){
    }
};

GameScript* GameScript::Get(){
    if (!__gameScript){
        __gameScript = new GameScript();
    }
    return __gameScript;
}
void GameScript::Destroy(){
    if (__gameScript){
        delete __gameScript;
    }
}

#endif // GameScrp_cpp
