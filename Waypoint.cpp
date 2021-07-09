#ifndef Waypoint_cpp
#define Waypoint_cpp
#include "AppEvent.cpp"
#include "GameWrld.cpp"
#include "commands/GenCmd.cpp"

class EventWaypoint : public GameWorldElement, public EventEmitter {
private:
    EventConsumer *_eventTarget = NULL;
    char *_eventNameOnTrigger = NULL;
    int _eventDataOnTrigger = 0;
    bool _oneTime = false;
    bool _heroOnly = false;
    bool emitted = false;
public:

    void render(){
    }

    void OnInteraction(GameWorldElement* otherElement){
        if (_heroOnly && !otherElement->hero){ return; }
        if (_oneTime && emitted){ return; }
        emitted = true;
        _eventTarget->OnEvent(this,_eventNameOnTrigger,CreateEventData(_eventDataOnTrigger,0));
    }

    EventWaypoint(EventConsumer *eventTarget, char *eventNameOnTrigger, int eventDataOnTrigger, bool oneTime, bool heroOnly): GameWorldElement("waypoint", GameResources::tileWidth, GameResources::tileHeight) {
        autoInteraction = 1;
        _eventTarget = eventTarget;
        _eventNameOnTrigger = eventNameOnTrigger;
        _eventDataOnTrigger = eventDataOnTrigger;
        _oneTime = oneTime;
        _heroOnly = heroOnly;
    }
};

class TeleportWaypoint : public GameWorldElement {
private:
    int dstWorld;
    int destX;
    int destY;
    clock_t lastEmit = clock();
public:

    void OnInteraction(GameWorldElement* otherElement){
        if (!otherElement->hero){ return; }
        clock_t now = clock();
        if (clockToMilliseconds(now-lastEmit) < 5000){ return; }
        lastEmit = now;
        GameWorld::GetActiveWorld()->EmitEvent("ChangeWorld",dstWorld);
        GameScript::Get()->AddCommand(new WaitCommand(1)); //wait one second to avoid user from running right back into teleport
        otherElement->SetPosition(destX,destY);
    }

    void render(){

    }

    TeleportWaypoint(char *name, int destWorld, int tileX, int tileY) : GameWorldElement(name,GameResources::tileWidth,GameResources::tileHeight){
        dstWorld = destWorld;
        destX = tileX*GameResources::tileWidth;
        destY = tileY*GameResources::tileHeight;
        autoInteraction = 1;
    }

};

#endif // Waypoint_cpp
