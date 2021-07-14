#ifndef Objects_CPP
#define Objects_CPP

#include "GameElem.cpp"

class Book : public GameWorldElement {
private:
    char *bookText;
public:

    void OnInteraction(GameWorldElement *otherElement){
        UIMsgBox::Create(bookText, 200, 100);
    }
    void update(GameMap* worldMap){

    }

    void render(){

    }

    Book(char *name, char *text) : GameWorldElement(name, 16,16){
        bookText = text;
        spriteId = 38;
        allowDrop = 1;
        interactableInWorld = 0;
        allowPickup = 1;
        SetSpriteset(GameResources::GetSpriteSet("ITEM"));
    }
};


class HealingPotion : public GameWorldElement {
private:
    HeroCharacter *_hero = NULL;
    int _healAmount = 0;
public:

    void OnInteraction(GameWorldElement *otherElement){
        if (_hero->health + _healAmount > _hero->totalHealth){
            _hero->health = _hero->totalHealth;
        } else {
            _hero->health+=_healAmount;
        }
        flagForRemoval = 1;
        flagForDestroy = 1;
    }
    void update(GameMap* worldMap){

    }

    void render(){

    }

    HealingPotion(char *name, int healAmount, HeroCharacter *hero) : GameWorldElement(name, 16,16){
        spriteId = 31;
        allowPickup = 0;
        allowDrop = 0;
        autoPickup = 0;
        autoInteraction = 1;
        dynamic = 0;
        interactableInWorld=0;
        _hero = hero;
        _healAmount = healAmount;
        SetSpriteset(GameResources::GetSpriteSet("ITEM"));
    }
};


class OneUp : public GameWorldElement {
private:
    HeroCharacter *_hero = NULL;
public:

    void OnInteraction(GameWorldElement *otherElement){
        _hero->lives++;
        flagForRemoval = 1;
        flagForDestroy = 1;
    }
    void update(GameMap* worldMap){

    }

    void render(){

    }

    OneUp(HeroCharacter *hero) : GameWorldElement("OneUp", 16,16){
        spriteId = 31;
        allowPickup = 0;
        allowDrop = 0;
        autoPickup = 0;
        autoInteraction = 1;
        dynamic = 0;
        interactableInWorld=0;
        _hero = hero;
        SetSpriteset(GameResources::GetSpriteSet("ITEM"));
    }
};


class Trap : public GameWorldElement {
private:
    int _trapTime = 0;
    bool inTrap = false;
    bool spent = false;
    clock_t startTrap;
    double trappedCharSpeed;
    GameWorldElement *trappedElement;
public:

    void OnInteraction(GameWorldElement *otherElement){
        if (inTrap || spent){ return; }
        if (otherElement->hero){ return; }
        inTrap = true;
        autoInteraction = 0;
        startTrap = clock();
        trappedElement = otherElement;
        trappedCharSpeed = otherElement->speed;
        otherElement->speed = 0;
    }
    void Update(){
        if (!inTrap){ return; }
        if (clockToMilliseconds(clock()-startTrap) > _trapTime*1000){
            //destroy trap and release element
            trappedElement->speed = trappedCharSpeed;
            flagForRemoval = 1;
            flagForDestroy = 1;
            inTrap = false;
            spent = true;
        }
    }

    void render(){

    }

    Trap(char *name, int trapSeconds) : GameWorldElement(name, 16,16){
        spriteId = 55;
        allowPickup = 0;
        allowDrop = 0;
        autoPickup = 0;
        autoInteraction = 1;
        dynamic = 0;
        interactableInWorld=0;
        _trapTime = trapSeconds;
        SetSpriteset(GameResources::GetSpriteSet("ITEM"));
    }
};

#endif // Objects_CPP
