#ifndef Level1_CPP
#define Level1_CPP

#include "../GameWrld.cpp"
#include "../commands/UICmd.cpp"
#include "../GameScrp.cpp"
#include "../ChrTypes.cpp"
#include "../Waypoint.cpp"

class Level1 : public GameWorld {
private:
    AggressiveCharacter *monster1 = NULL;
    AggressiveCharacter *monster2 = NULL;
    AggressiveCharacter *monster3 = NULL;
    EventWaypoint *endOfLevel = NULL;
    clock_t timeStart;
    clock_t timeEnd;
public:

    void OnEvent(EventEmitter *source, const char *event, EventData data) {
        if (source == endOfLevel && event == "stageEnd"){
            //show stage end screen;
            timeEnd = clock();
            float timeTaken =  clockToMilliseconds(timeEnd-timeStart)/1000;
            StageEndScreen *stageEnd = new StageEndScreen("Stage 1 Complete", timeTaken, this);
            UIWindowController::Get()->AddWindow(stageEnd);
            UIWindowController::Get()->SetFocusedWindow(stageEnd);
        } else if (event == "StageCompleteClosed"){
            GameWorld::GetActiveWorld()->EmitEvent("ChangeWorld",WORLD_LEVEL2);
        }
    };

    void CreateWorldElements(){
        monster1 = new AggressiveCharacter("Monster 1", "EDDIE",2, 3.2);
        monster2 = new AggressiveCharacter("Monster 2", "EDDIE",2, 3.2);
        monster3 = new AggressiveCharacter("Monster 3", "EDDIE",2, 3.2);
        HealingPotion *hp = new HealingPotion("hp1", 20, GetHero());
        AddWorldElementAtTile(hp, 10, 30);
        endOfLevel = new EventWaypoint(this,"stageEnd",0,true,true);
        AddWorldElementAtTile(endOfLevel, 29, 27);
        AddWorldElement(monster1, 64, 192);
        AddWorldElement(monster2, 256, 64);
        AddWorldElement(monster3, 384, 352);
    }

    void OnWorldActivated(){
        AddWorldElement((GameWorldElement*) GetHero(),96,80);
        GetHero()->focus();
        GameMusic::PlaySong("DESCENT.MID",1);
        timeStart = clock();
    }

    Level1(int viewportWidth, int viewportHeight) : GameWorld("First Level", "LEVEL1", viewportWidth, viewportHeight){

    }
    ~Level1(){
        if (monster1){ delete monster1; }
        if (monster2){ delete monster2; }
        if (monster3){ delete monster3; }
        if (endOfLevel){ delete endOfLevel; }
    }
};

#endif

