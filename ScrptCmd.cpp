#ifndef ScrptCmd_CPP
#define ScrptCmd_CPP

class ScriptCommand{
private:

public:
    virtual void OnCommandStart() = 0;
    virtual int OnCommandUpdate() = 0; //return 1 when done
    virtual void OnCommandEnd() = 0;
    virtual void CheckInputs(){}
    virtual void OnKeyUp(int ScanCode){}
    ScriptCommand(){
    }
    ~ScriptCommand(){
    }
};
#endif // ScrptCmd_CPP
