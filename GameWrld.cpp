#ifndef GameWorld_CPP
#define GameWorld_CPP

#include "GameElem.cpp"
#include "GameRes.cpp"
#include "ChrTypes.cpp"
#include "GameWrld.h"

bool compare_y_pos(GameWorldElement *s1, GameWorldElement *s2){
    return s1->GetPosition().y < s2->GetPosition().y;
}

int compareYPos(const void *s1, const void *s2)
{
  GameWorldElement *e1 = (GameWorldElement *)s1;
  GameWorldElement *e2 = (GameWorldElement *)s2;

}

class GameWorld;
GameWorld *__activeWorld = NULL;
class GameWorld : public UIDrawable {

private:

    UITextArea *diagnostic = NULL;
    char *posDisplay;

	std::list<GameWorldElement*> worldElements;

    HeroCharacter* _hero = NULL;


    void updateDrawOrder()
    {
        worldElements.sort(compare_y_pos);
    }

	void drawWorld(){
	    GrClearContextC(ctx, GrAllocColor(10,10,10));
	    diagnostic->BringToFront();
		int i;
		for (i = 0; i < worldMap->layerCount; i++){
			drawLayer(worldMap->tileLayers[i]);
		}
		updateDrawOrder();
		drawWorldElements();
	}

	void drawLayer(TileLayer *tileLayer){
		if (!tileLayer->drawReady){ return; }
		GrSetContext(ctx);
		GrBitBlt(ctx,0,0,tileLayer->canvas,worldX,worldY,worldX+width-1,worldY+height-1,GrIMAGE);
		//masked_blit(tileLayer->canvas, ctx, worldX, worldY, 0, 0, width, height);
	}

	void drawWorldElements(){
	    for (std::list<GameWorldElement*>::iterator iter = worldElements.begin(); iter != worldElements.end(); iter++){
            GameWorldElement* elem = (*iter);
            elem->render();
			struct coord elpos = elem->GetPosition();
			if (elpos.x == -1 || elpos.y == -1 || !elem->spriteset_loaded){ continue; }
			drawSpriteOnWorld(elem->spriteset, elem->spriteId, elpos.x, elpos.y, elem->elevation, elem->GetGroundBounds(), elem->character);
		}
	}

	void drawShadowOverBounds(struct bounds bds){
	    struct coord center = center_of_bounds(bds);
	    GrSetContext(ctx);
        GrFilledCircle(center.x-worldX, center.y-worldY, bds.dimension.height/2,GrAllocColor(100,100,100));
	}

	void drawSpriteOnWorld(SpriteSet *spriteset, int spriteId, int drawWorldX, int drawWorldY, int elevation, struct bounds ground, bool drawShadow){
		//draw a sprite on the world in a free position in pixels (non-tile-based)
		int xEnd, yEnd, spriteWidth, spriteHeight;
		xEnd = worldX + width-1;
		yEnd = worldY + height-1;
		spriteWidth = spriteset->spriteWidth;
		spriteHeight = spriteset->spriteHeight;
		if (drawWorldX > xEnd || drawWorldY > yEnd || (drawWorldX+spriteWidth) <  worldX || (drawWorldY+spriteHeight) < worldY){
			//outside of viewable area
			return;
		}
		int tileX, tileY, subX, subY, drawX, drawY, shadowX, shadowY, clipWidth, clipHeight;
		tileX = spriteId % spriteset->tilesWide;
		tileY = (spriteId / spriteset->tilesWide);
		subX = tileX * spriteWidth;
		subY = tileY * spriteHeight;
		drawX = drawWorldX-worldX;
		drawY = drawWorldY-worldY-elevation;

		if (drawX < 0){
            //clipped
            subX -= drawX;
            spriteWidth = spriteWidth+drawX;
            drawX = 0;
		}
		if (drawY < 0){
            subY -= drawY;
            spriteHeight = spriteHeight+drawY;
            drawY = 0;
		}
		if (drawShadow){ drawShadowOverBounds(ground); }
		GrBitBlt(ctx,drawX, drawY,spriteset->imagedata,subX,subY,subX+spriteWidth-1,subY+spriteHeight-1,GrIMAGE);

		//masked_blit(spriteset->imagedata, ctx, subX, subY, drawX, drawY, spriteWidth-1, spriteHeight-1);
	}

	void draw_internal(){
		drawWorld();
	}

	void remove_flagged_elements(){
        for (std::list<GameWorldElement*>::reverse_iterator iter = worldElements.rbegin(); iter != worldElements.rend(); iter++){
            GameWorldElement* elem = (*iter);
            if (elem->flagForRemoval){
                RemoveWorldElement(elem);
            }
        }
	}

	int checkCollisionXAgainstWorldMap(GameWorldElement *sourceElement){
        struct bounds sourceGround = sourceElement->GetGroundBounds();
        if ((sourceElement->movingX < 0) && checkCornerCollisionXAgainstWorldMap(sourceGround.topleft.x, sourceGround.topleft.y, sourceElement)){ return 1; } //top left
        if ((sourceElement->movingX > 0) && checkCornerCollisionXAgainstWorldMap(sourceGround.bottomright.x, sourceGround.topleft.y, sourceElement)){ return 1; } //top right
        if ((sourceElement->movingX < 0) && checkCornerCollisionXAgainstWorldMap(sourceGround.topleft.x, sourceGround.bottomright.y, sourceElement)){ return 1; } //bottom left
        if ((sourceElement->movingX > 0) && checkCornerCollisionXAgainstWorldMap(sourceGround.bottomright.x, sourceGround.bottomright.y, sourceElement)){ return 1; } //bottom right
        return 0;
	}

	int checkCollisionYAgainstWorldMap(GameWorldElement *sourceElement){
        struct bounds sourceGround = sourceElement->GetGroundBounds();
        if ((sourceElement->movingY < 0) && checkCornerCollisionYAgainstWorldMap(sourceGround.topleft.x, sourceGround.topleft.y, sourceElement)){ return 1; } //top left
        if ((sourceElement->movingY < 0) && checkCornerCollisionYAgainstWorldMap(sourceGround.bottomright.x, sourceGround.topleft.y, sourceElement)){ return 1; } //top right
        if ((sourceElement->movingY > 0) && checkCornerCollisionYAgainstWorldMap(sourceGround.topleft.x, sourceGround.bottomright.y, sourceElement)){ return 1; } //bottom left
        if ((sourceElement->movingY > 0) && checkCornerCollisionYAgainstWorldMap(sourceGround.bottomright.x, sourceGround.bottomright.y, sourceElement)){ return 1; } //bottom right
        return 0;
	}

	int checkCornerCollisionYAgainstWorldMap(int originX, int originY, GameWorldElement *sourceElement){
	    int tileWidth = worldMap->tileWidth;
	    int tileHeight = worldMap->tileHeight;
        int destX = originX;
        int destY = originY + sourceElement->movingY;
        if (destX < 0 || destY < 0){ return 1; }
        int edgeOriginTileX = originX / tileWidth;
        int edgeOriginTileY = originY / tileHeight;
        int edgeDestTileX = destX / tileWidth;
        int edgeDestTileY = destY / tileHeight;
        if (edgeDestTileX > worldMap->tilesWide-1 || edgeDestTileY > worldMap->tilesHigh-1){ return 1; }

        if (edgeOriginTileY == edgeDestTileY){ return 0; } //no possible collision

        int originSolid = worldMap->blockLayer[edgeOriginTileY][edgeOriginTileX];
        int destSolid = worldMap->blockLayer[edgeDestTileY][edgeDestTileX];

        if (originSolid == -1 && destSolid == -1){ return 0; }  // no blocks

        if (edgeDestTileY > edgeOriginTileY){
            //check origin tile for bottom-blocking solid edge (1, 2, 4, 5, 8, 12) or if dest is full blocking
            if (destSolid == 0 || originSolid == 1 || originSolid == 2 || originSolid == 4 || originSolid == 5 || originSolid == 8 || originSolid == 12){
                return 1;
            }
            //check destination tile for top-blocking edge (2,3,4,6,7,10)
            if (destSolid == 2 || destSolid == 3 || destSolid == 4 || destSolid == 6 || destSolid == 7 || destSolid == 10){
                return 1;
            }

        } else if (edgeDestTileY < edgeOriginTileY){

            if (destSolid == 0 || originSolid == 2 || originSolid == 3 || originSolid == 4 || originSolid == 6 || originSolid == 7 || originSolid == 10){
                return 1;
            }
            if (destSolid == 1 || destSolid == 2 || destSolid == 4 || destSolid == 5 || destSolid == 8 || destSolid == 12){
                return 1;
            }

        }

        return 0;
	}

	int checkCornerCollisionXAgainstWorldMap(int originX, int originY, GameWorldElement *sourceElement){
	    int tileWidth = worldMap->tileWidth;
	    int tileHeight = worldMap->tileHeight;
        int destX = originX + sourceElement->movingX;
        int destY = originY;

        if (destX < 0 || destY < 0){ return 1; }

        int edgeOriginTileX = originX / tileWidth;
        int edgeOriginTileY = originY / tileHeight;
        int edgeDestTileX = destX / tileWidth;
        int edgeDestTileY = destY / tileHeight;
        if (edgeDestTileX > worldMap->tilesWide-1 || edgeDestTileY > worldMap->tilesHigh-1){ return 1; }
        if (edgeOriginTileX == edgeDestTileX){ return 0; } //no possible collision

        int originSolid = worldMap->blockLayer[edgeOriginTileY][edgeOriginTileX];
        int destSolid = worldMap->blockLayer[edgeDestTileY][edgeDestTileX];
        if (originSolid == -1 && destSolid == -1){ return 0; } // no blocks

        if (edgeDestTileX > edgeOriginTileX){
            //check origin tile for right-blocking solid edge (1, 3, 4, 7, 8, 11) or if dest is full blocking
            if (destSolid == 0 || originSolid == 1 || originSolid == 3 || originSolid == 4 || originSolid == 7 || originSolid == 8 || originSolid == 11){
                return 1;
            }
            //check destination tile for left-blocking edge, or full block (1,2,3,5,6,9)
            if (destSolid == 1 || destSolid == 2 || destSolid == 3 || destSolid == 5 || destSolid == 6 || destSolid == 9){
                return 1;
            }

        } else if (edgeDestTileX < edgeOriginTileX){

            if (destSolid == 0 || originSolid == 1 || originSolid == 2 || originSolid == 3 || originSolid == 5 || originSolid == 6 || originSolid == 9){
                return 1;
            }
            if (destSolid == 1 || destSolid == 3 || destSolid == 4 || destSolid == 7 || destSolid == 8 || destSolid == 11){
                return 1;
            }

        }

        return 0;
	}


	int checkCollisionBetweenElements(struct bounds newBounds, GameWorldElement *sourceElement, GameWorldElement *targetElement){
	    if (bounds_intersect(newBounds,targetElement->GetGroundBounds())){ return 1; }
        return 0;
	}

	int checkCollisionXAgainstWorldElements(GameWorldElement* sourceElement){
        int i;
        struct bounds newPos = sourceElement->GetGroundBounds();
        newPos = bounds_offset(newPos,sourceElement->movingX,0);
	    for (std::list<GameWorldElement*>::iterator iter = worldElements.begin(); iter != worldElements.end(); iter++){
            GameWorldElement* elem = (*iter);
            if (elem == sourceElement || !elem->dynamic){ continue; }
            if (checkCollisionBetweenElements(newPos,sourceElement,elem)){ return 1; }
        }
        return 0;
	}

    int checkCollisionYAgainstWorldElements(GameWorldElement* sourceElement){
        int i;
        struct bounds newPos = sourceElement->GetGroundBounds();
        newPos = bounds_offset(newPos,0,sourceElement->movingY);
	    for (std::list<GameWorldElement*>::iterator iter = worldElements.begin(); iter != worldElements.end(); iter++){
            GameWorldElement* elem = (*iter);
            if (elem == sourceElement || !elem->dynamic){ continue; }
            if (checkCollisionBetweenElements(newPos,sourceElement,elem)){ return 1; }
        }
        return 0;
    }

	void moveElementX(GameWorldElement *element){
        if (element->solidVsWorld && checkCollisionXAgainstWorldMap(element)){ return; } //cannot move in X axis due to world block
        if (element->solidVsDynamic && checkCollisionXAgainstWorldElements(element)){ return; }
        struct coord cur = element->GetPosition();
        element->SetPosition(cur.x+element->movingX, cur.y);

	}
	void moveElementY(GameWorldElement *element){
        if (element->solidVsWorld && checkCollisionYAgainstWorldMap(element)){ return; } //cannot move in Y axis due to world block
        if (element->solidVsDynamic && checkCollisionYAgainstWorldElements(element)){ return; }
        struct coord cur = element->GetPosition();
        element->SetPosition(cur.x, cur.y+element->movingY);
	}

	void checkForAutoCollision(GameCharacter *sourceChar){
        int i;
        struct bounds newPos = sourceChar->GetGroundBounds();
	    for (std::list<GameWorldElement*>::iterator iter = worldElements.begin(); iter != worldElements.end(); iter++){
            GameWorldElement* elem = (*iter);
            if (elem->autoPickup == 0 && elem->autoInteraction == 0){ continue; }
            if (checkCollisionBetweenElements(newPos,(GameWorldElement*)sourceChar,elem)){
                if (elem->autoInteraction){
                    elem->OnInteraction((GameWorldElement*) sourceChar);
                } else if (elem->autoPickup && sourceChar->hero){
                    if (GetHero()->GetInventory()->AddItem(elem)){
                        elem->flagForRemoval = true;
                    }
                }
            }
        }
	}


	void moveElements(){
	    clock_t current;
	    int msPassedFromInput;
	    current = clock();
	    for (std::list<GameWorldElement*>::iterator iter = worldElements.begin(); iter != worldElements.end(); iter++){
            GameWorldElement* elem = (*iter);
            if (!elem->moving){ continue; }
            //update and check delays

            msPassedFromInput = clockToMilliseconds(current-elem->movingLastUpdated);
            if (msPassedFromInput > MS_PER_UPDATE || elem->speed == 0){
                //time has passed, stop moving
                elem->moving = 0;
                elem->movingDelayCounter = 0;
                elem->movingX = 0;
                elem->movingY = 0;
                //elem->direction = CHARDIR_IDLE;
                continue;
            }
            if (elem->movingDelayCounter < elem->movingDelay){
                elem->movingDelayCounter++;
                continue;
            }

            if (abs(elem->movingX) > 0) { moveElementX(elem); }
            if (abs(elem->movingY) > 0) { moveElementY(elem); }
            if (elem->character){
                checkForAutoCollision((GameCharacter*) elem);
            }
            elem->movingDelayCounter = 0;
	    }
	}

public:
    char *mapId;
    char *worldName;
    GameMap *worldMap = NULL;

	int worldX = 0;
	int worldY = 0;

	void SetWorldPosition(int x, int y){
		if (x < 0){ x = 0; }
		if (y < 0){ y = 0; }
		if (x > (worldMap->tilesWide*worldMap->tileWidth)-width){ x = (worldMap->tilesWide*worldMap->tileWidth)-width; }
		if (y > (worldMap->tilesHigh*worldMap->tileHeight)-height){ y = (worldMap->tilesHigh*worldMap->tileHeight)-height; }
		worldX = x;
		worldY = y;
	}


    void Update(){
		remove_flagged_elements();
        for (std::list<GameWorldElement*>::iterator iter = worldElements.begin(); iter != worldElements.end(); iter++){
            GameWorldElement* elem = (*iter);
			elem->Update();
			elem->OnCheckSurroundings(worldElements);
			//check if character is in focus, move World to match
			if (elem->focused){
                int calcWorldX, calcWorldY;
                struct coord pos = elem->GetPosition();
                calcWorldX = pos.x - (width /2) + (worldMap->tileWidth / 2);
                calcWorldY = pos.y - (height / 2) + (worldMap->tileHeight / 2);
                SetWorldPosition(calcWorldX,calcWorldY);
			}
		}
		if (_hero){
            //drop it into the world
            while (_hero->GetInventory()->HasDropItem()){
                AddWorldElement(_hero->GetInventory()->GetNextDropItem(),_hero->GetPosition().x, _hero->GetPosition().y);
            }

            sprintf(posDisplay,"POS x %d, y %d", _hero->GetPosition().x, _hero->GetPosition().y);
            diagnostic->SetText(posDisplay);
		}
		moveElements();
    }

    bool ElementInWorld(GameWorldElement *elem){
        int i;
        for (std::list<GameWorldElement*>::iterator iter = worldElements.begin(); iter != worldElements.end(); iter++){
            GameWorldElement* gelem = (*iter);
            if (gelem == elem){
                return true;
            }
        }
        return false;
    }

    void AddWorldElement(GameWorldElement *elem, int posX, int posY){
        if (ElementInWorld(elem)){
            elem->SetPosition(posX, posY);
            return;
        }
        elem->SetPosition(posX,posY);
        worldElements.push_back(elem);
    }

    void AddWorldElementAtTile(GameWorldElement *elem, int tileX, int tileY){
        AddWorldElement(elem, GameResources::tileWidth*tileX, GameResources::tileHeight*tileY);
    }

    void RemoveWorldElement(GameWorldElement *elem){
        worldElements.remove(elem);
        if (elem->flagForDestroy){
            delete elem;
        }
    }

    void FireInteraction(GameWorldElement *sourceElement){
        //determine if there are any nearby elements to interact with
        int zoneX, zoneY;
        struct bounds testBds = sourceElement->GetGroundBounds();
        if (sourceElement->direction == CHARDIR_E || sourceElement->direction==CHARDIR_SE || sourceElement->direction==CHARDIR_NE){
            testBds = bounds_offset(testBds,testBds.dimension.width,0);
        } else if (sourceElement->direction == CHARDIR_W ||sourceElement->direction==CHARDIR_SW || sourceElement->direction==CHARDIR_NW){
            testBds = bounds_offset(testBds,-testBds.dimension.width,0);
        } else if (sourceElement->direction == CHARDIR_S || sourceElement->direction==CHARDIR_IDLE){
            testBds = bounds_offset(testBds,0,testBds.dimension.height);
        } else if (sourceElement->direction == CHARDIR_N){
            testBds = bounds_offset(testBds,0,-testBds.dimension.height);
        }
        int distX, distY, thres, minDistX, minDistY;
        //thres = GameResources::tileWidth / 2;
        minDistX = 999;
        minDistY = 999;
        GameWorldElement *minElem = NULL;
	    for (std::list<GameWorldElement*>::iterator iter = worldElements.begin(); iter != worldElements.end(); iter++){
            GameWorldElement* elem = (*iter);
            //check for one closest to the zone corner
            if (!elem->interactableInWorld && (sourceElement != _hero || !elem->allowPickup)){ continue; }
            distX = abs(elem->GetPosition().x-zoneX);
            distY = abs(elem->GetPosition().y-zoneY);
            if (bounds_intersect(testBds,elem->GetGroundBounds())){
                minDistX = distX;
                minDistY = distY;
                minElem = elem;
            }
        }
        if (minElem){
            if (minElem->interactableInWorld){
                minElem->OnInteraction(sourceElement);
            } else if (sourceElement == _hero && minElem->allowPickup) {
                if (GetHero()->GetInventory()->AddItem(minElem)){
                    RemoveWorldElement(minElem);
                }
            }
        }
    }

    void TakeHero(HeroCharacter *hero){
        _hero = hero;
        //AddCharacter((GameCharacter*) hero,0,0);
        //AddWorldElement((GameWorldElement*) hero,0,0);
    }

    HeroCharacter*  GetHero(){
        return _hero;
    }

    virtual void CreateWorldElements(){
    }
    virtual void OnWorldActivated(){
    }

    GameWorld(char *world_name, char *map_id, int viewportWidth, int viewportHeight) : UIDrawable(viewportWidth, viewportHeight){
        mapId = map_id;
        worldName = world_name;
        worldMap = GameResources::GetMap(map_id);
        diagnostic = new UITextArea(60, 20);
        AddChild((UIDrawable*) diagnostic);
        diagnostic->x = viewportWidth-60;
        diagnostic->y = viewportHeight-20;
        posDisplay = (char*) malloc(sizeof(char)*255);
        strcpy(posDisplay,"");
    }

    static GameWorld* GetActiveWorld();
    static void SetActiveWorld(GameWorld *world);
};

GameWorld* GameWorld::GetActiveWorld(){
    return __activeWorld;
}
void GameWorld::SetActiveWorld(GameWorld *world){
    __activeWorld = world;
}

#endif
