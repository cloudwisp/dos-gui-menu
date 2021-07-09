#ifndef GameElem_CPP
#define GameElem_CPP

#include <list>
#include "GameRes.h"
#include "GameElem.h"
#include "GameApp.h"
#include "spatial.cpp"


class GameWorldElement {
private:

    struct coord _pos;
    struct bounds _ground;
    struct bounds _cover;
    struct dimension _dimension;

	void set_dir_from_vector(int vectX, int vectY){
		if (vectX == 0 && vectY == 0){
			direction = CHARDIR_IDLE;
		} else if (vectX == 0){
			//in y axis
			if (vectY < 0){
				direction = CHARDIR_N;
			} else {
				direction = CHARDIR_S;
			}
		} else if (vectY == 0){
			//in x axis only
			if (vectX < 0){
				direction = CHARDIR_W;
			} else {
				direction = CHARDIR_E;
			}
		} else if (vectX > 0 && vectY > 0){
			direction = CHARDIR_SE;
		} else if (vectX < 0 && vectY > 0){
			direction = CHARDIR_SW;
		} else if (vectX < 0 && vectY < 0){
			direction = CHARDIR_NW;
		} else {
			direction = CHARDIR_NE;
		}
	}

public:
	int focused = 0;
	int deleted = 0;
	int solidVsWorld = 0;
	int solidVsDynamic = 0;
	int dynamic = 0;
	int friendly = 1;
	int allowPickup = 0;
	int autoPickup = 0;
	int allowDrop = 0;
	int autoInteraction = 0;
	int interactableInWorld = 0;
	int hero = 0;
	int character = 0;
	int flagForRemoval = 0;
	int flagForDestroy = 0;
    double speed = 3;
	int direction = CHARDIR_IDLE;
	clock_t movingLastUpdated;
	int moving = 0;
	int movingDelay = 0; //number of game ticks to wait, before moving 1 pixel.
	int movingDelayCounter = 0;
	double movingDelayRemainder = 0;
	int movingX = 0;
	int movingY = 0;
    int movingBlocked = 0;
    int groundClipWidth = 0;
    int groundClipHeight = 0;
    int groundOffsetX = 0; //offset from topleft
    int groundOffsetY = 0;
    int elevation = 0;

	char *name;

	virtual void render() = 0;
	virtual void OnInteraction(GameWorldElement *otherElement){

	}
	virtual void OnCheckSurroundings(std::list<GameWorldElement*> worldElements){

	}

	SpriteSet *spriteset = NULL;
	int spriteId = 0;
	int spriteset_loaded = 0;

	void SetSpriteset(SpriteSet *sprite_set){
		spriteset = sprite_set;
		spriteset_loaded = 1;
	}

	void SetPosition(struct coord pos){
		_pos = pos;
		_ground.topleft.x = _pos.x + groundOffsetX;
		_ground.topleft.y = _pos.y + groundOffsetY;
		_ground.bottomright.x = _pos.x + groundOffsetX + groundClipWidth;
		_ground.bottomright.y = _pos.y + groundOffsetY + groundClipHeight;
		_ground.dimension.width = _ground.bottomright.x-_ground.topleft.x;
		_ground.dimension.height = _ground.bottomright.y-_ground.topleft.y;
	}

	void SetPosition(int x, int y){
        struct coord myCoord;
        myCoord.x = x;
        myCoord.y = y;
        SetPosition(myCoord);
	}

	struct coord GetPosition(){
        return _pos;
	}

	struct coord GetGroundPosition(){
        return _ground.topleft;
	}

	struct bounds GetGroundBounds(){
        return _ground;
	}

	struct bounds GetGroundBoundsInFrontOfElement(){
        int offsetX, offsetY;
        offsetX = 0;
        offsetY = 0;
        if (direction == CHARDIR_E || direction == CHARDIR_SE || direction == CHARDIR_NE){
            offsetX = _ground.dimension.width;
        } else if (direction == CHARDIR_W || direction == CHARDIR_NW || direction == CHARDIR_SW){
            offsetX = -_ground.dimension.width;
        }
        if (direction == CHARDIR_N || direction == CHARDIR_NW || direction == CHARDIR_NE){
            offsetY = -_ground.dimension.height;
        } else if (direction == CHARDIR_S || direction == CHARDIR_SE || direction == CHARDIR_SW){
            offsetY = _ground.dimension.height;
        }
        return bounds_offset(_ground,offsetX,offsetY);
	};

	void SetDimensions(int w, int h, int groundClipW, int groundClipH){
	    _dimension.width = w;
	    _dimension.height = h;
        if (groundClipW == 0){ groundClipWidth = w; } else { groundClipWidth = groundClipW; }
        if (groundClipH == 0){ groundClipHeight = h; } else { groundClipHeight = groundClipH; }
        groundOffsetX = (_dimension.width - groundClipWidth) / 2;
        groundOffsetY = (_dimension.height - groundClipHeight);
        //reset position, to calculate bounding boxes
        SetPosition(_pos);
	}

	void SetGroundClip(int groundClipW, int groundClipH){
        SetDimensions(_dimension.width,_dimension.height,groundClipW,groundClipH);
	}

	struct dimension GetDimensions(){
        return _dimension;
	}

	int width(){
        return _dimension.width;
	}
	int height(){
        return _dimension.height;
	}

	virtual void Update(){

	}

	void move(int vectorX, int vectorY){
		//expected to be called continuously during movement, speed is in tiles per second
		if (speed == 0){ return; }
		moving = 1;
		set_dir_from_vector(vectorX, vectorY);
		double delay;
		delay = ((((1/speed) * 1000) / MS_PER_UPDATE) / GameResources::tileWidth)-1;
		delay+=movingDelayRemainder;
		movingDelay = delay;
		movingDelayRemainder = delay-movingDelay;
		movingX = vectorX;
		movingY = vectorY;
		movingLastUpdated = clock();
	}

	void Focus(){
		focused = 1;
	}

	void UnFocus(){
		focused = 0;
	}

	GameWorldElement(char *elementName, int elmWidth, int elmHeight){
	    SetDimensions(elmWidth, elmHeight,0,0);
	    _pos.x = 0;
	    _pos.y = 0;
		name = elementName;
		moving = 0;

	}
};

#endif
