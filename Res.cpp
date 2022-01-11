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
    string readme;
    string image;
    string genre;
    string year;
    string developer;
    string notes;
    bool favorite;
    bool cd;
};

struct PathInfo {
    string drive;
    string folder;
    string file;
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
        w = 1;
        GrQueryPnm((char*)filename.c_str(),&w,&h,&m);
        if (w == 1){
            return NULL;
        }
        GrContext *ctx = GrCreateContext(w,h,NULL,NULL);
		if (ctx == NULL){
            debugOut("Error creating context - out of memory?");
            return ctx;
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

    std::string _GetReadme(std::string path){
        fstream rmfile;
        std::string file_contents;
        rmfile.open(path.c_str(),ios::in);
        if (rmfile.is_open()){
            string tp;
            while(getline(rmfile, tp)){
                file_contents += tp;
                file_contents.push_back('\n');
            }
        }
        return file_contents;
    }

    std::string _GetDefaultItem(){
        fstream dfile;
        dfile.open("DEFAULT.DAT", ios::in);
        if (dfile.is_open()){
            string tp;
            getline(dfile, tp);
            return tp;
        }
        return string("");
    }

    PathInfo* GetPathInfo(std::string path){
        size_t firstSlash = path.find('\\');
        string drive = path.substr(0, firstSlash);
        size_t lastSlash = path.find_last_of('\\');
        string folder = path.substr(firstSlash, lastSlash-firstSlash);
        string fileName = path.substr(lastSlash + 1);
        return new PathInfo {drive, folder, fileName};
    }

    void _WriteLaunchBat(std::string path, std::string returnTitle){
        fstream batch;
        PathInfo* pathInfo = GetPathInfo(path);
        char* curDir = currentDir();
        batch.open("menuitem.bat", ios::app);
        if (batch.is_open()){
            batch.write("\n",1);
            batch.write(pathInfo->drive.c_str(), pathInfo->drive.size());
            batch.write("\n",1);
            batch.write("cd ", 3);
            batch.write(pathInfo->folder.c_str(), pathInfo->folder.size());
            batch.write("\n", 1);
            batch.write(pathInfo->file.c_str(), pathInfo->file.size());
            batch.write("\n", 1);
            delete pathInfo;
            pathInfo = GetPathInfo(curDir);
            batch.write(pathInfo->drive.c_str(), pathInfo->drive.size());
            batch.write("\n",1);
            batch.write("cd ", 3);
            batch.write(pathInfo->folder.c_str(), pathInfo->folder.size());
            batch.write("\n", 1);
            batch.write(pathInfo->file.c_str(), pathInfo->file.size());
            batch.write("\n", 1);
            delete pathInfo;
            batch.close();
        }

        fstream returnMeta;
        returnMeta.open("default.dat", ios::out);
        if (returnMeta.is_open()){
            returnMeta.write(returnTitle.c_str(), returnTitle.size());
            returnMeta.close();
        }
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
                            } else if (key == "developer"){
                                currentItem->developer = itemValue;
                            } else if (key == "year"){
                                currentItem->year = itemValue;
                            } else if (key == "notes"){
                                currentItem->notes = itemValue;
                            } else if (key == "readme"){
                                currentItem->readme = itemValue;
                            } else if (key == "favorite"){
                                currentItem->favorite = itemValue == "T";
                            } else if (key == "cd"){
                                currentItem->cd = itemValue == "T";
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

    static std::string GetReadme(string filename){
        return Current()->_GetReadme(filename);
    }

    static void WriteLaunchBat(string path, string returnTitle){
        return Current()->_WriteLaunchBat(path, returnTitle);
    }

    static std::string GetDefaultItem(){
        return Current()->_GetDefaultItem();
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
