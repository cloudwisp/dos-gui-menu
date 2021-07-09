#ifndef GenCmd_CPP
#define GenCmd_CPP

#include "../ScrptCmd.cpp"


class WaitCommand : public ScriptCommand {
private:
    int waitSeconds;
    int ticksLeft;
public:

    void OnCommandStart(){
        ticksLeft = (waitSeconds * 1000) / MS_PER_UPDATE;
    }

    int OnCommandUpdate(){
        ticksLeft--;
        if (ticksLeft <= 0){
            return 1;
        }
        return 0;
    }
    void OnCommandEnd(){
    }
    WaitCommand(int seconds){
        waitSeconds = seconds;
    }
};


class EmitEventCommand : public ScriptCommand, public EventEmitter {
private:
    std::string _event;
    EventConsumer* _target = NULL;
    EventEmitter* _source = NULL;
    EventData _data;
public:

    void OnCommandStart(){
        _target->OnEvent(_source,_event,_data);
    }

    int OnCommandUpdate(){
        return 1;
    }
    void OnCommandEnd(){
    }
    EmitEventCommand(EventEmitter* source, EventConsumer* target, std::string event, EventData data){
        _source = source;
        _target = target;
        _event = event;
        _data = data;
    }
};


#endif // GenCmd_CPP
