#ifndef GameRes_CPP
#define GameRes_CPP

#include <grx20.h>
#include <stdlib.h>
#include "Common.cpp"
#include "GameRes.h"
using namespace std;

struct SpriteSetStr {
	string id;
	int imWidth;
	int imHeight;
	int tilesWide;
	int spriteWidth;
	int spriteHeight;
	GrContext *imagedata = NULL;
};

struct TileLayerStr {
	int drawReady;
	int tilesWide;
	int tilesHigh;
	string id;
	vector<vector<int> > tileMap;
	SpriteSet *spriteset = NULL;
	GrContext *canvas = NULL;
};

struct GameMapStr {
	string id;
	int layerCount;
	int tileWidth;
	int tileHeight;
	int tilesWide;
	int tilesHigh;
	TileLayer *tileLayers[25] = {NULL};
	vector<vector<int> > blockLayer;
};

struct GameCharModelStr {
    string id;
    SpriteSet *spriteset = NULL;
    int groundClipWidth;
    int groundClipHeight;
    int sprites[10][20];
};

struct CachedImage {
    string filename;
    GrContext *ctx = NULL;
};

class GameResources {
private:

    static GameResources *self;

    std::vector<CachedImage*> _cachedImages;
	std::vector<GameCharModel*> _charModels;
    std::vector<GameMap*> _maps;
    std::vector<TileLayer*> _tilelayers;
    std::vector<SpriteSet*> _spritesets;

	string ReadSpriteSetMetaData(string spriteset_id, int *sprWidth, int *sprHeight){
	    string meta_fn = string(spriteset_id) += ".SPR";
	    FILE* stream = fopen(meta_fn.c_str(),"r");
		if (stream == NULL){
			//no definition, use default
			string defaultImage = string(spriteset_id)+=".PNG";

			(*sprWidth) = tileWidth;
			(*sprHeight) = tileHeight;
			return defaultImage;
		}
		char line[1024];
		int y, x, spriteId;
		y = 0;
		char* spriteset_fn;
		while (fgets(line,1024,stream)){
			if (!strcmp(line,"\n") || !strcmp(line,"\r\n") || !strcmp(line,"") || line[0]==0x23){
				//empty
				continue;
			}
			char* tmp;
			tmp = strdup(line);

			char* token = strtok(tmp,",");
			x = 0;
			while (token != NULL){
                if (x == 0){
                    spriteset_fn = strdup(token);
                } else if (x == 1){
                    (*sprWidth) = atoi(token);
                } else if (x == 2){
                    (*sprHeight) = atoi(token);
                }
				token = strtok(NULL,",");
				x++;
			}
			free(tmp);
			free(token);
			y++;
		}
		fclose(stream);
        return string(spriteset_fn);
	}



	void _cw_populate_tilelayer_canvas(TileLayer *tileLayer){
		int w, h;
		w = tileLayer->tilesWide*tileWidth;
		h = tileLayer->tilesHigh*tileHeight;
		tileLayer->canvas = GrCreateContext(w,h,NULL,NULL);
		GrClearContextC(tileLayer->canvas,GrAllocColor(0,0,0));
		int y, x, spriteId, drawX, drawY, tileX, tileY, subX, subY;
		for (x = 0; x < tileLayer->tilesWide; x++){
			for (y = 0; y < tileLayer->tilesHigh; y++){
                spriteId = tileLayer->tileMap[y][x];

				drawX = (x * tileWidth);
				drawY = (y * tileHeight);
				if (spriteId > 0){
					tileX = spriteId % tileLayer->spriteset->tilesWide;
					tileY = (spriteId / tileLayer->spriteset->tilesWide);
					subX = tileX * tileWidth;
					subY = tileY * tileHeight;
					GrBitBlt(tileLayer->canvas,drawX,drawY,tileLayer->spriteset->imagedata,subX,subY,subX+tileWidth-1,subY+tileHeight-1,GrIMAGE);
				}
			}
		}
	}

	vector<vector<int> > ReadTileMap(string fn){
	    debugOut("read tilemap" + fn);
	    vector<vector<int> > tm;
        FILE* stream = fopen(fn.c_str(),"r");
		if (stream == NULL){
			return tm;
		}
		char line[1024];
		int y, x, spriteId;
		y = 0;
		while (fgets(line,1024,stream)){
			if (!strcmp(line,"\n") || !strcmp(line,"\r\n") || !strcmp(line,"")){
				//empty
				break;
			}
			tm.push_back(vector<int>());
			char* tmp;
			tmp = strdup(line);

			char* token = strtok(tmp,",");
			x = 0;
			while (token != NULL){

				spriteId = atoi(token);
                tm[y].push_back(spriteId);

				token = strtok(NULL,",");
				x++;
			}
			free(tmp);
			free(token);
			y++;
		}
		fclose(stream);
		return tm;
	}

protected:

	GameCharModel* _GetGameCharModel(string id){
        for (int x = 0; x < _charModels.size(); x++) {
            if (_charModels[x]->id == id){
                return _charModels[x];
            }
	    }
        GameCharModel *newModel = new GameCharModel;
        newModel->id = string(id);
        _charModels.push_back(newModel);

	    string fn = string(id)+=".CHR";
	    FILE* stream = fopen(fn.c_str(),"r");
		if (stream == NULL){
            debugOut("file read error");
			return newModel;
		}
		char line[1024];
		string spriteset_id;
		int y, x, parseId, thisDirection, numSprites;
		y = 0;
		//line 1 is spritesheet, line2 is dimensions.
		while (fgets(line,1024,stream)){
            if (!strcmp(line,"\n") || !strcmp(line,"\r\n") || !strcmp(line,"") || line[0]==0x23){
				//empty or comment, important not to increment line number
				continue;
			}

			char* tmp;
			tmp = strdup(line);
			char* token = strtok(tmp,",");
			x = 0;
			if (y == 0){
			    while (token != NULL){
                    if (x == 0){
                        spriteset_id = string(token);
                    } else if (x == 1){
                        newModel->groundClipWidth = atoi(token);
                    } else if (x == 2){
                        newModel->groundClipHeight = atoi(token);
                    }
                    token = strtok(NULL,",");
                    x++;
                }
			} else {
                while (token != NULL){
                    parseId = atoi(token);
                    if (x == 0){
                        thisDirection = parseId;
                    } else if (x == 1){
                        numSprites = parseId;
                    } else {
                        //spriteids
                        newModel->sprites[thisDirection][x-2] = parseId;
                    }
                    token = strtok(NULL,",");
                    x++;
                }
                //fill in the rest of the spriteId with -1
                for (x = x-3; x<20; x++){
                    newModel->sprites[thisDirection][x] = -1;
                }
			}


			free(tmp);
			free(token);
			y++;
		}
		fclose(stream);
		newModel->spriteset = _GetSpriteSet(spriteset_id);
		return newModel;
	}

	void _DestroyCharModels(){
	    for (int x = 0; x < _charModels.size(); x++){
            delete _charModels[x];
	    }
	    _charModels.clear();
	}


	TileLayer *_GetTileLayer(string id, string spriteset_id, int mapTilesWide, int mapTilesHigh){
		//check if tile layer is already loaded
		debugOut("GetTileLayer " + id);
		for (int x = 0; x < _tilelayers.size(); x++){
            if (_tilelayers[x]->id == id){
                return _tilelayers[x];
            }
		}


		//create new
		TileLayer *newTileLayer;
		newTileLayer = new TileLayer;
		_tilelayers.push_back(newTileLayer);
        debugOut("Before GetSpriteSet");
		newTileLayer->spriteset = _GetSpriteSet(spriteset_id);
		debugOut("after GetSpriteSet");
		newTileLayer->id = string(id);
		newTileLayer->drawReady = 0;
		newTileLayer->tilesWide = mapTilesWide;
		newTileLayer->tilesHigh = mapTilesHigh;
		//Load map file
		string fn = string(id)+".CSV";
		newTileLayer->tileMap = ReadTileMap(fn);
		debugOut("Before populate tile layer canvas");
		_cw_populate_tilelayer_canvas(newTileLayer);
		newTileLayer->drawReady = 1;
		debugOut("Before return tile layer");
		return newTileLayer;
	}

	void _DestroyTileLayers(){
        int i;
        for (i = 0; i < _tilelayers.size(); i++){
            GrDestroyContext(_tilelayers[i]->canvas);
            delete _tilelayers[i];
        }
        _tilelayers.clear();
	}

	SpriteSet *_GetSpriteSet(string spriteset_id){
        //check for existing;
        debugOut("GetSpriteSet " + spriteset_id);
        for (int i = 0; i < _spritesets.size(); i++){
            if (_spritesets[i]->id == spriteset_id){
                return _spritesets[i];
            }
        }
		SpriteSet* newSpriteSet = new SpriteSet;

		int w, h, sprWidth, sprHeight;

		string filename = ReadSpriteSetMetaData(spriteset_id, &sprWidth, &sprHeight);
		debugOut("Before load image " + filename);
		GrContext *ctx = _LoadImage(filename);
		GrContext *prevCtx = GrCurrentContext();
		GrSetContext(ctx);
		w = GrSizeX();
		h = GrSizeY();
		GrSetContext(prevCtx);

		//free(prevdir);
		newSpriteSet->spriteWidth = sprWidth;
		newSpriteSet->spriteHeight = sprHeight;
		newSpriteSet->id = string(spriteset_id);
		newSpriteSet->imWidth = w;
		newSpriteSet->imHeight = h;
		newSpriteSet->tilesWide = (int)w/sprWidth;
		newSpriteSet->imagedata = ctx;
		debugOut("Before return spriteset");
		return newSpriteSet;
	}

	void _DestroySpriteSets(){
        int i;
        for (i = 0; i < _spritesets.size(); i++){
            delete _spritesets[i];
        }
        _spritesets.clear();
	}


	GrContext *_LoadImage(string filename){

	    //check for existing;
		int i = 0;
		for (i = 0; i < _cachedImages.size(); i++){
            if (_cachedImages[i]->filename == filename){
                return _cachedImages[i]->ctx;
            }
		}

		//create new
		CachedImage* newImage = new CachedImage;
	    newImage->filename = string(filename);
	    _cachedImages.push_back(newImage);

	    int w, h;
        GrQueryPng((char*)filename.c_str(),&w,&h);
		GrContext *ctx = GrCreateContext(w,h,NULL,NULL);
		newImage->ctx = ctx;
		if (ctx == NULL){
            debugOut("Error creating context - out of memory?");
		}
		if (GrLoadContextFromPng(ctx, (char*) filename.c_str(),0) == -1){
            debugOut("Error loading png file");
		}
		return ctx;
	}

	void _DestroyImages() {
	    for (int x = 0; x < _cachedImages.size(); x++){
            GrDestroyContext(_cachedImages[x]->ctx);
            delete _cachedImages[x];
	    }
	    _cachedImages.clear();
	}


	GameMap* _GetMap(string id){

		for (int i = 0; i < _maps.size(); i++){
            if (_maps[i]->id == id){
                return _maps[i];
            }
		}

		//create new
		GameMap* newMap = new GameMap;

		newMap->layerCount = 0;
		newMap->id = string(id);

		newMap->tileWidth = tileWidth;
		newMap->tileHeight = tileHeight;
		//load list of layers from file definition
		string fn = string(id)+".WRL";

		FILE* stream = fopen(fn.c_str(),"r");
		if (stream == NULL){
			return NULL;
		}

		char line[1024];
		int r = 0;
		while (fgets(line,1024,stream)){
			char* tmp;
			tmp = strdup(line);

			char* token = strtok(tmp,",");
			int pos=0;
			string map_id;
			string spriteset_id;

			while (token != NULL){
				if (pos == 0){
					//name
					map_id = string(token);
				} else if (pos == 1){
					//spriteset
					spriteset_id = string(token);
				}
				token = strtok(NULL,",");
				pos++;
			}
            if (r == 0){
                //first row contains dimensions
                newMap->tilesWide = atoi(map_id.c_str());
                newMap->tilesHigh = atoi(spriteset_id.c_str());
            } else if (spriteset_id == "BLOCKLAYER"){
                //populate the block layer
                string blockFileName = string(map_id) + ".CSV";
                newMap->blockLayer = ReadTileMap(blockFileName);
			} else {
                TileLayer *tl = _GetTileLayer(map_id,spriteset_id, newMap->tilesWide, newMap->tilesHigh);
                newMap->tileLayers[newMap->layerCount] = tl;
                newMap->layerCount++;
			}
			free(tmp);
			free(token);
			r++;
		}

		return newMap;
	}

	void _DestroyGameMaps(){
        int i;
        for (i = 0; i<_maps.size(); i++){
            delete _maps[i];
        }
        _maps.clear();
	}

public:


    static GameResources* Current(){
        if (!self){
            self = new GameResources();
        }
        return self;
    }

    GameResources(){

    }
    ~GameResources(){

    }


	static int tileWidth;
	static int tileHeight;


	static GameCharModel *GetGameCharModel(string id){
        return Current()->_GetGameCharModel(id);
	}

	static TileLayer *GetTileLayer(string id, string spriteset_id, int mapTilesWide, int mapTilesHigh){
		return Current()->_GetTileLayer(id, spriteset_id, mapTilesWide, mapTilesHigh);
	}

	static SpriteSet *GetSpriteSet(string spriteset_id){
		return Current()->_GetSpriteSet(spriteset_id);
	}

	static GrContext *LoadImage(string filename){
        return Current()->_LoadImage(filename);
	}

	static GameMap *GetMap(string id){
		return Current()->_GetMap(id);
	}

	static void Destroy(){
        Current()->_DestroyImages();
        Current()->_DestroySpriteSets();
        Current()->_DestroyGameMaps();
        Current()->_DestroyTileLayers();
        Current()->_DestroyCharModels();
        delete self;
	}

};

int GameResources::tileWidth = 16;
int GameResources::tileHeight = 16;
GameResources* GameResources::self = NULL;


#endif
