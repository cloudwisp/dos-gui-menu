#ifndef Level1_CPP
#define Level1_CPP

#include <time.h>
#include "../GameWrld.cpp"
#include "../commands/UICmd.cpp"
#include "../GameScrp.cpp"
#include "../ChrTypes.cpp"
#include "../Waypoint.cpp"
#include "../commands/GmAppCmd.cpp"

class Level1 : public GameWorld {
private:
    AggressiveCharacter *monster1 = NULL;
    AggressiveCharacter *monster2 = NULL;
    AggressiveCharacter *monster3 = NULL;
    StageEndWaypoint *endOfLevel = NULL;

    void HandleStageClose() override{
        GameScript::Get()->AddCommand(new LoadWorldCommand(WORLD_LEVEL2,"Entering the East Wing..."));
    }
public:
    

    void CreateWorldElements(){
        monster1 = new AggressiveCharacter("Monster 1", "EDDIE",2, 3.2);
        monster2 = new AggressiveCharacter("Monster 2", "EDDIE",2, 3.2);
        monster3 = new AggressiveCharacter("Monster 3", "EDDIE",2, 3.2);
        HealingPotion *hp = new HealingPotion("hp1", 20, GetHero());
        AddWorldElementAtTile(hp, 10, 30);
        endOfLevel = new StageEndWaypoint(this);
        AddWorldElementAtTile(endOfLevel, 29, 27);
        AddWorldElement(monster1, 64, 192);
        AddWorldElement(monster2, 256, 64);
        AddWorldElement(monster3, 384, 352);
    }

    void OnWorldActivated(){
        AddWorldElement((GameWorldElement*) GetHero(),96,80);
        GetHero()->focus();
        GameMusic::PlaySong("DESCENT.MID",1);
    }

    Level1(int viewportWidth, int viewportHeight) : GameWorld("Stage 1", "LEVEL1", viewportWidth, viewportHeight){

    }
    ~Level1(){
        if (monster1){ delete monster1; }
        if (monster2){ delete monster2; }
        if (monster3){ delete monster3; }
        if (endOfLevel){ delete endOfLevel; }
    }
};

#endif

