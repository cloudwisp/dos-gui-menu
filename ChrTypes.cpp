#ifndef ChrTypes_CPP
#define ChrTypes_CPP
#include "GameChar.cpp"
#include "commands/UICmd.cpp"
#include "GameScrp.cpp"
#include "GameInv.cpp"
class HeroCharacter : public GameCharacter {
private:

    GameInventory* inventory = NULL;
    int invCreated = 0;

public:
    int traps = 10;

    GameInventory* GetInventory(){
        if (!invCreated){
            int w, h, x, y;
            UIAppScreen* screen = UIWindowController::Get()->GetScreen();
            w = screen->width/2;
            h = screen->height/2;
            x = screen->width / 4;
            y = screen->height / 4;
            inventory = new GameInventory(this,10,w,h);
            inventory->x = x;
            inventory->y = y;
            invCreated=1;
        }
        return inventory;
    }

    HeroCharacter(char *title) : GameCharacter(title, "HERO"){
        hero = 1;
        friendly = 1;
        solidVsDynamic = 0;
    }
};

class AggressiveCharacter : public GameCharacter {
private:
    int betweenAttacks = 2;
    clock_t lastAttack = clock();
public:
    void OnInteraction(GameWorldElement *otherElement){
        GameScript::Get()->AddCommand(new ShowDialogCommand("Hey! don't touch me...",200,20));
    }
    void OnCheckSurroundings(std::list<GameWorldElement*> worldElements){
        int i;
        struct coord mypos = GetPosition();
        for (std::list<GameWorldElement*>::iterator iter = worldElements.begin(); iter != worldElements.end(); iter++){
            GameWorldElement* elem = (*iter);
            struct coord elpos = elem->GetPosition();
            if (elem->hero){
                if (abs(elpos.x - mypos.x) < 100 && abs(elpos.y - mypos.y) < 100){
                    if (bounds_intersect(GetGroundBoundsInFrontOfElement(),elem->GetGroundBounds())){
                        //close enough to hit, stop moving and hit
                        clock_t now = clock();
                        if (clockToMilliseconds(now-lastAttack) >= betweenAttacks*1000){
                            ((GameCharacter*) elem)->OnAttacked(this);
                            lastAttack = now;
                        }
                    }
                    int vectX, vectY;
                    if (elpos.x > mypos.x){
                        vectX = 1;
                    } else if (elpos.x < mypos.x){
                        vectX = -1;
                    } else {
                        vectX = 0;
                    }
                    if (elpos.y > mypos.y){
                        vectY = 1;
                    } else if (elpos.y < mypos.y){
                        vectY = -1;
                    } else {
                        vectY = 0;
                    }
                    move(vectX, vectY);

                }
            }
        }
    }

    AggressiveCharacter(char *title, char *charmodel_id, int secondsBetweenAttacks, double moveSpeed) : GameCharacter(title, charmodel_id){
        friendly = 0;
        interactableInWorld=1;
        betweenAttacks = secondsBetweenAttacks;
        speed = moveSpeed;
    }
};

#endif // ChrTypes_CPP
