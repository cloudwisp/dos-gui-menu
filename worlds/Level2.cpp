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
    std::vector<GameWorldElement*> _disposableWorldElements;

    void AddDisposableElement(GameWorldElement* elem, int tileX, int tileY){
        _disposableWorldElements.push_back(elem);
        AddWorldElementAtTile(elem, tileX, tileY);
    }
protected:
    void HandleStageClose() override {
        GameWorld::GetActiveWorld()->EmitEvent("CompleteGame");
    }
public:

    void CreateWorldElements(){
        AddDisposableElement(new AggressiveCharacter("Monster 1", "EDDIE",2, 3.2), 24, 9);
        AddDisposableElement(new AggressiveCharacter("Monster 2", "EDDIE",2, 3.2), 36, 4);
        AddDisposableElement(new AggressiveCharacter("Monster 3", "EDDIE",2, 3.2), 47, 14);
        AddDisposableElement(new AggressiveCharacter("Monster 4", "EDDIE",2, 3.2), 63, 16);
        AddDisposableElement(new StageEndWaypoint(this), 2, 27);
        AddDisposableElement(new HealingPotion("a potion", GetHero()->totalHealth * 0.5, GetHero()), 16, 12);
    }

    void OnWorldActivated(){
        AddWorldElementAtTile((GameWorldElement*) GetHero(),16,9);
        GetHero()->focus();
        GameMusic::PlaySong("GROOT.MID",1);
    }

    Level2(int viewportWidth, int viewportHeight) : GameWorld("Stage 2", "LEVEL2", viewportWidth, viewportHeight){

    }
    ~Level2(){
        for(GameWorldElement* elem : _disposableWorldElements){
            if (elem){
                delete elem;
            }
        }
    }
};

#endif

