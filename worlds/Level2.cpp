#ifndef Level2_CPP
#define Level2_CPP

#include "../GameWrld.cpp"
#include "../commands/UICmd.cpp"
#include "../GameScrp.cpp"
#include "../ChrTypes.cpp"
#include "../Waypoint.cpp"

class Level2 : public GameWorld {
private:
    AggressiveCharacter *monster1 = NULL;
    AggressiveCharacter *monster2 = NULL;
    AggressiveCharacter *monster3 = NULL;
    AggressiveCharacter *monster4 = NULL;
    StageEndWaypoint * stageEnd = NULL;
protected:
    void HandleStageClose() override {
        GameWorld::GetActiveWorld()->EmitEvent("CompleteGame");
    }
public:

    void CreateWorldElements(){
        monster1 = new AggressiveCharacter("Monster 1", "EDDIE",2, 3.2);
        monster2 = new AggressiveCharacter("Monster 2", "EDDIE",2, 3.2);
        monster3 = new AggressiveCharacter("Monster 3", "EDDIE",2, 3.2);
        monster4 = new AggressiveCharacter("Monster 4", "EDDIE",2, 3.2);
        stageEnd = new StageEndWaypoint(this);

        AddWorldElementAtTile(stageEnd, 2, 27);

        AddWorldElementAtTile(monster1, 24, 9);
        AddWorldElementAtTile(monster2, 36, 4);
        AddWorldElementAtTile(monster3, 47, 14);
        AddWorldElementAtTile(monster3, 63, 16);
        //potion = new HealingPotion("a potion",50,GetHero());
    }

    void OnWorldActivated(){
        AddWorldElementAtTile((GameWorldElement*) GetHero(),16,9);
        GetHero()->focus();
        GameMusic::PlaySong("GROOT.MID",1);
    }

    Level2(int viewportWidth, int viewportHeight) : GameWorld("Stage 2", "LEVEL2", viewportWidth, viewportHeight){

    }
    ~Level2(){
        if (monster1){ delete monster1; }
        if (monster2){ delete monster2; }
        if (monster3){ delete monster3; }
        if (monster4){ delete monster4; }
        if (stageEnd){ delete stageEnd; }
    }
};

#endif

