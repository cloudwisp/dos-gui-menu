#ifndef GameApp_CPP
#define GameApp_CPP

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <pc.h>
#include <vector>
#include <algorithm>

#include "Common.h"
#include "AppUI.h"
#include "keyboard.h"
#include "GameRes.h"
#include "GameElem.h"
#include "App.cpp"
#include "GameChar.cpp"
#include "ChrTypes.cpp"
#include "Objects.cpp"
#include "GameInv.cpp"
#include "GameWrld.cpp"
#include "GameScrp.cpp"
#include "GameMus.cpp"
#include "GameUI.cpp"
#include "GameApp.h"
#include "worlds/Worlds.cpp"
#include "commands/UIResCmd.cpp"
#include "commands/GamUICmd.cpp"
#include "commands/UICmd.cpp"
#include "commands/GmAppCmd.cpp"

class GameMainWindow : public UIWindow {
private:

public:

    void CheckInputs(){
        UIWindow::CheckInputs();
    }

    void OnKeyUp(int ScanCode, int ShiftState, int Ascii){
        UIWindow::OnKeyUp(ScanCode, ShiftState, Ascii);
    }

    GameMainWindow(int drawWidth, int drawHeight) : UIWindow(drawWidth, drawHeight){

    }

};


class GameApplication : public CWApplication {
private:

	GameWorld *_cw_current_world = NULL;
	int _cw_current_world_set = 0;
    int _cw_current_world_id = -1;

	GameInventory *inventory = NULL;

	GameEventCallback _cw_event_callbacks[255];
	int _cw_event_callback_count = 0;
	int initialWorld = 0;

    GameMainWindow *mainWindow = NULL;
	int viewportWidth = 0;
	int viewportHeight = 0;
    int viewportX = 0;
    int viewportY = 0;
    int heroMove = 0;
    int heroVectX = 0;
    int heroVectY = 0;
    int heroJump = 0;
    HeroCharacter *hero = NULL;
    GameScript *script = NULL;
    HealthWidget *healthDisplay = NULL;
    HealthWidget *endDisplay = NULL;
    TitleScreen *titleScreen = NULL;
    UITextWindow *loadingScreen = NULL;
    GameEndScreen* gameEndScreen = NULL;

    bool loadingSequencePending = false;

	void on_start(){
        //events
        this->BindEvent("ChangeWorld", this);
        //define sequence of events
        GameMusic::PlaySong("fanfare.mid",0);
        std::vector<string> preLoadImgs;
        preLoadImgs.push_back("mazerun.png");
        preLoadImgs.push_back("screenbg.png");
        script->AddCommand(new PreLoadImageCommand(true,"Please Wait...",preLoadImgs));
        script->AddCommand(new ShowTitleScreenCommand("mazerun.png"));
        script->AddCommand(new ShowStoryScreenCommand("screenbg.png"));
        script->AddCommand(new LoadWorldCommand(initialWorld,"Entering the Palace of Hierolia..."));
        //script->AddCommand(new EmitEventCommand((EventEmitter*) this, (EventConsumer*) this,"ChangeWorld",CreateEventData(initialWorld,0)));
	}

	void render(){

	}

	void update(){

        if (script->NeedsControl()){
            script->UpdateExecute();
            heroMove = 0;
            return;
        }

        if (heroMove){
            GetHero()->move(heroVectX, heroVectY);
	    }
	    if (heroJump){
            GetHero()->jump();
	    }
	    healthDisplay->totalHP = GetHero()->totalHealth;
	    healthDisplay->currentHP = GetHero()->health;
	    healthDisplay->BringToFront();
	    endDisplay->totalHP = GetHero()->totalEndurance;
	    endDisplay->currentHP = GetHero()->endurance;
	    endDisplay->BringToFront();
	}

	void check_inputs(int *cancelInputPropagation){
        if (GetFocusedWindow() == mainWindow && !script->NeedsControl()){

            //main window focused, main section to handle player input controls

            //x axis
            if (TrueKeyState(KEY_LEFT_ARROW) || (TrueKeyState(KEY_KP_4) && ShiftState & SHIFTSTATE_NUM_LOCK_ON)){
                heroVectX = -1;
                heroMove = 1;
            } else if (TrueKeyState(KEY_RIGHT_ARROW) || (TrueKeyState(KEY_KP_6) && ShiftState & SHIFTSTATE_NUM_LOCK_ON)){
                heroVectX = 1;
                heroMove = 1;
            } else {
                heroVectX = 0;
            }
            //y axis
            if (TrueKeyState(KEY_DOWN_ARROW) || (TrueKeyState(KEY_KP_2) && ShiftState & SHIFTSTATE_NUM_LOCK_ON)){
                heroVectY = 1;
                heroMove = 1;
            } else if (TrueKeyState(KEY_UP_ARROW) || (TrueKeyState(KEY_KP_8) && ShiftState & SHIFTSTATE_NUM_LOCK_ON)){
                heroVectY = -1;
                heroMove = 1;
            } else {
                heroVectY = 0;
            }
            //check if still moving
            if (heroVectX == 0 && heroVectY == 0){
                heroMove = 0;
            }
            if (TrueKeyState(KEY_A)){
                heroJump = 1;
            } else {
                heroJump = 0;
            }

            (*cancelInputPropagation) = 1;
        } else if (GetFocusedWindow() == mainWindow && script->NeedsControl()){
            script->CheckInputs();
        }
	}

	void on_keyup(int ScanCode, int ShiftState, int Ascii, int *cancelInputPropagation){
        if (GetFocusedWindow() == mainWindow && script->NeedsControl()){
            script->OnKeyUp(ScanCode, ShiftState, Ascii);
            (*cancelInputPropagation) = 1;
            return;
        }
        if (GetFocusedWindow() == mainWindow){

            //keys pressed with no other windows open

            if (ScanCode == KEY_I){
                //open the inventory
                ToggleInventory();
                //GetInventory()->Open();
                //SetFocusedWindow((UIWindow*) GetInventory());
                (*cancelInputPropagation) = 1;
            } else if (ScanCode == KEY_ENTER){
                GetActiveWorld()->FireInteraction(GetHero());
            } else if (ScanCode == KEY_ESC){
                End();
                (*cancelInputPropagation) = 1;
            } else if (ScanCode == KEY_F){
                if (GetHero()->focused){
                    GetHero()->UnFocus();
                } else {
                    GetHero()->Focus();
                }
            } else if (ScanCode == KEY_R){
                GetHero()->toggleRunning();
            } else if (ScanCode == KEY_T && _cw_current_world_set && GetHero()->traps > 0){
                Trap *newTrap = new Trap("trap", 1);
                _cw_current_world->AddWorldElement(newTrap, GetHero()->GetPosition().x, GetHero()->GetPosition().y);
                GetHero()->traps--;
            }
            return;
        }

	}

	/*
        World binding means that the class has been instantiated, and that events have been subscribed
	*/
	std::vector<int> boundWorlds;

	GameWorld *worldObjects[255] = {NULL}; //this will contain the actual loaded objects, using index from world const val
    bool world_is_bound(int world){
        return std::find(boundWorlds.begin(), boundWorlds.end(), world) != boundWorlds.end();
    }

    int add_bound_world(int world){
        boundWorlds.push_back(world);
    }

	void SetActiveWorld(GameWorld *active_world, int worldId){
	    _cw_current_world = active_world;
		_cw_current_world_set = 1;
        _cw_current_world_id = worldId;
		active_world->width = viewportWidth;
		active_world->height = viewportHeight;
		active_world->x = viewportX;
		active_world->y = viewportY;

		mainWindow->AddChild((UIDrawable *) active_world);
		active_world->BringToFront();
		active_world->Activate();
		GameWorld::SetActiveWorld(active_world); //for static access without a reference to this class
		//viewport->SetWorld(active_world);
		//TODO: any things that need to be one when a world is changed, events?
	}

    void RemoveActiveWorld(){
        if (_cw_current_world_set == 1){
            mainWindow->RemoveChild((UIDrawable *) _cw_current_world);
            //remove hero from the current world
            _cw_current_world->RemoveWorldElement((GameWorldElement*) GetHero());
            UnloadWorld(_cw_current_world_id);
        }
    }

public:

	void OnEvent(EventEmitter *source, std::string event, EventData data){
	    if (_cw_current_world_set && source == _cw_current_world && _cw_current_world->focus){
			if (event == "LeftMouseButtonClick"){
				//emit an event in context of the world coordinates
				EmitEvent("WorldClick", _cw_current_world->worldX+data.data1, _cw_current_world->worldY+data.data2);
			}
		}

        if (event == "CharacterDied" && source == hero){
            if (hero->Resurrect()){
                script->AddCommand(new LoadWorldCommand(_cw_current_world_id,"Respawning..."));
            } else {
                // game over.
                CompleteGame();
            }
        }

		if (event == "ChangeWorld"){
            ChangeWorld(data.data1);
		}
        if (event == "CompleteGame"){
            CompleteGame();
        }
        if (source == gameEndScreen && event == "WindowClosed"){
            End();
        }
		//pass events onto parent
		CWApplication::OnEvent(source,event,data);
	}

	void InitWorld(int gameWorld){
        if (world_is_bound(gameWorld)){
            return;
        }
        if (gameWorld == WORLD_LEVEL1){
            worldObjects[gameWorld] = new Level1(viewportWidth,viewportHeight);
        } else if (gameWorld == WORLD_LEVEL2){
            worldObjects[gameWorld] = new Level2(viewportWidth, viewportHeight);
        } else {
            return;
        }
        //hook to certain events
        worldObjects[gameWorld]->BindEvent("ChangeWorld", (EventConsumer*)this);
        worldObjects[gameWorld]->BindEvent("CompleteGame", this);
        worldObjects[gameWorld]->BindEvent("LeftMouseButtonClick", (EventConsumer*) this);
        worldObjects[gameWorld]->TakeHero(GetHero());
        worldObjects[gameWorld]->CreateWorldElements();
        add_bound_world(gameWorld);
	}

	void UnloadWorlds(){
        for (int worldId : boundWorlds){
            if (worldObjects[worldId]){
                delete worldObjects[worldId];
            }
        }
	}

    void UnloadWorld(int worldId){
        boundWorlds.erase(std::remove(boundWorlds.begin(), boundWorlds.end(), worldId), boundWorlds.end());
        delete worldObjects[worldId];
    }

	void ChangeWorld(int gameWorld){
	    RemoveActiveWorld();
        //LoadingScreen::Show();
        InitWorld(gameWorld);
        SetActiveWorld(worldObjects[gameWorld], gameWorld);
        EmitEvent("WorldChanged", gameWorld);
        //LoadingScreen::Hide();
	}

    void CompleteGame(){
        RemoveActiveWorld();
        gameEndScreen = new GameEndScreen();
        gameEndScreen->BindEvent("WindowClosed", this);
        UIWindowController::Get()->AddWindow(gameEndScreen);
        UIWindowController::Get()->SetFocusedWindow(gameEndScreen);
    }

	GameWorld *GetActiveWorld(){
        return _cw_current_world;
	}

    HeroCharacter *GetHero(){
        if (hero != NULL){ return hero; }
        hero = new HeroCharacter("HERO");
        hero->BindEvent("CharacterDied", this);
        return hero;
    }

    GameInventory *GetInventory(){
        if (inventory != NULL){ return inventory; }
        inventory = GetHero()->GetInventory();
        inventory->Close();
        inventory->AddItem((GameWorldElement*) new Book("Book of wisdom", "I bring great wisdom..."));
        AddWindow((UIWindow *) inventory);
        return inventory;
    }

    void ToggleInventory(){
        GameInventory *inv = GetInventory();
        if (!inv->closed){
            inv->Close();
        } else {
            inv->Open();
            SetFocusedWindow((UIWindow*) inv);
        }
    }

	GameApplication(int screenWidth, int screenHeight, int bitDepth, int viewport_Width, int viewport_Height, int viewport_X, int viewport_Y, int initial_World): CWApplication (screenWidth, screenHeight, bitDepth, MS_PER_UPDATE) {

		viewportWidth = viewport_Width;
		viewportHeight = viewport_Height;
		viewportX = viewport_X;
		viewportY = viewport_Y;
		initialWorld = initial_World;

		mainWindow = new GameMainWindow(screenWidth, screenHeight);
		AddWindow(mainWindow);

		script = GameScript::Get();

		healthDisplay = new HealthWidget(GrAllocColor(255,0,0));
		mainWindow->AddChild(healthDisplay);
		healthDisplay->x = screenWidth-healthDisplay->width-4;
		healthDisplay->y = 2;

		endDisplay = new HealthWidget(GrAllocColor(0,0,254));
		mainWindow->AddChild(endDisplay);
		endDisplay->x = screenWidth-(endDisplay->width*2)-8;
		endDisplay->y = 2;

	}
	~GameApplication(){
	    UnloadWorlds();
	    if (inventory){ delete inventory; }
	    if (hero){ delete hero; }
	    if (mainWindow){ delete mainWindow; }
	    if (titleScreen){ delete titleScreen; }
	    if (healthDisplay){ delete healthDisplay; }
	    if (endDisplay){ delete endDisplay; }
        if (gameEndScreen) { delete gameEndScreen; }
	    GameScript::Destroy();
	}

};

#endif
