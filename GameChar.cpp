#ifndef GameChar_CPP
#define GameChar_CPP

#include <stdio.h>
#include <string.h>
#include "GameRes.cpp"
#include "GameChar.h"
#include "GameApp.h"
#include "Common.h"
#include "GameElem.cpp"

class GameCharacter : public GameWorldElement, public EventEmitter {
	private:

	GameCharModel *model = NULL;

	int index;
    int regularSpeed = 0;
	int frame;
	int init = 0;

    bool running = false;
	clock_t lastRegen = clock();


	void _update_health_end(clock_t now){
        if (health < totalHealth || endurance < totalEndurance || running){
            if (clockToMilliseconds(now-lastRegen) > 1000){
                if (health < totalHealth){
                    health+=healthRegenPerSecond;
                    if (health > totalHealth){ health = totalHealth; }
                }
                if (endurance < totalEndurance && !running){
                    endurance+=enduranceRegenPerSecond;
                    if (endurance > totalEndurance){ endurance = totalEndurance; }
                } else if (running){
                    endurance-=enduranceLossPerSecondWhenRunning;
                    if (endurance <= 0){ endurance = 0; toggleRunning(); }
                }
                lastRegen = now;
            }
        }
	}

	//jumping
	bool inJump = false;
	clock_t jumpStart;
	clock_t jumpLastPressed = clock();
	int jumpDuration = 1000; //ms
	int jumpPeak = 8; //pixels
	int jumpDecay = 50; //ms per pixel
	int jumpCooldown = 1000;
	bool jumpFall = false;


	void _end_jump(clock_t now){
        inJump = false;
        jumpFall = false;
        elevation = 0;
	}

	void _update_jump(clock_t now){
        if (inJump){
            int diff = clockToMilliseconds(now-jumpStart);

            if (diff > jumpDuration){
                _end_jump(now);
                return;
            }

            int diffFromInput = clockToMilliseconds(now-jumpLastPressed);
            if (diff < jumpDuration/2) {
                //beginning arc
                elevation = ((double) diff / (jumpDuration/2)) * jumpPeak;
            } else {
                //second half - either the user is sustaining the jump, or is falling
                int diffFromCenter = diff - (jumpDuration/2);
                if (diffFromInput > 150 || jumpFall){
                    jumpFall = true;
                    //decay
                    elevation -= diffFromCenter / jumpDecay;
                } else {
                    //sustain arc
                    elevation = jumpPeak - (((double) diffFromCenter/ (jumpDuration/2)) * jumpPeak);
                }
                if (elevation <= 0){
                    _end_jump(now);
                    return;
                }
            }

        }
	}

	public:

    int health = 100;
    int totalHealth = 100;
    int strength = 10;
    int endurance = 100;
    int totalEndurance = 100;
    int enduranceRegenPerSecond = 5;
    int enduranceLossPerSecondWhenRunning = 10;
    int healthRegenPerSecond = 1;
    double runningMultiplier = 1.5;
    bool alive = true;
    int lives = 1;

    void toggleRunning(){
        if (running){
            speed = regularSpeed;
            running = false;
        } else {
            regularSpeed = speed;
            speed = regularSpeed * runningMultiplier;
            running = true;
        }
    }

    clock_t lastTick;

	//draw the character
	void render(){
	    if (!init){
            SetSpriteset(model->spriteset);
            init = 1;
	    }
        clock_t now = clock();
        if (clockToMilliseconds(now-lastTick) < 250){
            //frame per quarter second
            return;
        }
		spriteId = model->sprites[direction][frame];
        lastTick = clock();
		if (!moving){ return; }
		if (frame == 19 || (frame+1 < 19 && model->sprites[direction][frame+1]==-1)){
			frame = 0;
		} else {
			frame++;
		}
	}

	void focus(){
		focused = 1;
	}

	void unfocus(){
		focused = 0;
	}

    void KillCharacter(){
        alive = false;
        direction = CHARDIR_DEAD;
        endurance = 0;
        health = 0;
        EmitEvent("CharacterDied");
    }

	void OnAttacked(GameCharacter* otherChar){
        if (!alive){
            return;
        }
        //TBD: some chance of missing?
        health -= otherChar->strength;
        if (health < 0){ health = 0; }
        if (health == 0){
            KillCharacter();
        }
	}

    bool Resurrect(){
        if (alive || lives == 0){
            return false;
        }

        direction = CHARDIR_IDLE;
        alive = true;
        lives--;
        health = totalHealth;
        endurance = totalEndurance;
        return true;
    }

	void Update(){
        clock_t now = clock();
        _update_health_end(now);
        _update_jump(now);
        GameWorldElement::Update();
	}


	void jump(){
	    clock_t now = clock();
	    if (!inJump && clockToMilliseconds(now-jumpLastPressed) < jumpCooldown){
            return;
	    }
	    jumpLastPressed = now;
        if (inJump){
            return;
        }
        inJump = true;
        jumpStart = now;
	}

	GameCharacter(char *charTitle, char *charmodel_id) : GameWorldElement(charTitle, 10, 10){
	    frame = 0;
		dynamic = 1;
		solidVsWorld = 1;
		solidVsDynamic = 1;
		character = 1;
		spriteset_loaded = 0;
		direction = CHARDIR_IDLE; //idle
        lastTick = clock();
		model = GameResources::GetGameCharModel(charmodel_id);
		SetDimensions(model->spriteset->spriteWidth,model->spriteset->spriteHeight,model->groundClipWidth,model->groundClipHeight);

	}
};

#endif
