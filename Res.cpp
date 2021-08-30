#ifndef Res_CPP
#define Res_CPP


#include <grx20.h>
#include <stdlib.h>
#include "Common.cpp"
#include <iostream>
#include <fstream>

using namespace std;


struct CachedImage {
    string filename;
    GrContext *ctx = NULL;
};

struct DatabaseItem {
    string name;
    string path;
    string image;
    string genre;
    string year;
    string publisher;
    string description;
};

class AppResources {
private:

    static AppResources *self;

	GrContext *_LoadImage(string filename){
        if (filename.find(".png", 0) != std::string::npos){
            return _LoadPng(filename);
        }
	    
        return _LoadPnm(filename);
	}

    GrContext *_LoadPnm(string filename){
        int w, h, m;
        GrQueryPnm((char*)filename.c_str(),&w,&h,&m);
		GrContext *ctx = GrCreateContext(w,h,NULL,NULL);
		if (ctx == NULL){
            debugOut("Error creating context - out of memory?");
		}
		if (GrLoadContextFromPnm(ctx, (char*) filename.c_str()) == -1){
            debugOut("Error loading pnm file");
		}
		return ctx;
    }

    GrContext *_LoadPng(string filename){
        int w, h;
        GrQueryPng((char*)filename.c_str(),&w,&h);
		GrContext *ctx = GrCreateContext(w,h,NULL,NULL);
		if (ctx == NULL){
            debugOut("Error creating context - out of memory?");
		}
		if (GrLoadContextFromPng(ctx, (char*) filename.c_str(),0) == -1){
            debugOut("Error loading png file");
		}
		return ctx;
    }

    std::vector<DatabaseItem*> *_databaseItems = new std::vector<DatabaseItem*>();
    std::vector<DatabaseItem*> *_GetDatabaseItems(string filename){
        if (_databaseItems->size() > 0){
            return _databaseItems;
        }
        fstream dbfile;
        DatabaseItem* currentItem = NULL;
        dbfile.open(filename.c_str(),ios::in);
        if (dbfile.is_open()){
            string tp;
            while(getline(dbfile, tp)){
                if (tp.length() == 0){
                    continue;
                }
                if (tp.at(0) == 0x5B){
                    if (currentItem){
                        //add previous to collection
                        _databaseItems->push_back(currentItem);
                    }
                    //new item
                    currentItem = new DatabaseItem;
                    string itemName = tp.substr(1,tp.length()-2);
                    currentItem->name = itemName;
                } else {
                    //scan string until = separator
                    string key;
                    for (int i = 0; i < tp.length(); i++){
                        if (tp.at(i) == 0x3D && tp.length() > i+1){
                            //check for supported keys and store value from equals onwards and then break;
                            string itemValue = tp.substr(i+1);
                            if (key == "genre"){
                                currentItem->genre = itemValue;
                            } else if (key == "path"){
                                currentItem->path = itemValue;
                            } else if (key == "image"){
                                currentItem->image = itemValue;
                            } else if (key == "publisher"){
                                currentItem->publisher = itemValue;
                            } else if (key == "year"){
                                currentItem->year = itemValue;
                            } else if (key == "description"){
                                currentItem->description = itemValue;
                            }
                            break;
                        }
                        key += tp.at(i);
                    }
                }
            }
            if (currentItem){
                _databaseItems->push_back(currentItem);
            }
            dbfile.close();
            return _databaseItems;
        }

    }

public:

    static AppResources* Current(){
        if (!self){
            self = new AppResources();
        }
        return self;
    }

    static GrContext *LoadImage(string filename){
        return Current()->_LoadImage(filename);
	}

	static std::vector<DatabaseItem*>* GetDatabaseItems(string filename){
        return Current()->_GetDatabaseItems(filename);
	}

    static void Destroy(){
        delete self;
	}

    ~AppResources(){
        for (int i = 0; i < _databaseItems->size(); i++){
            delete (*_databaseItems)[i];
        }

        _databaseItems->clear();
        delete _databaseItems;
    }

};


AppResources* AppResources::self = NULL;

#endif // Res_CPP
