#ifndef Res_CPP
#define Res_CPP


#include <grx20.h>
#include <stdlib.h>
#include "Common.cpp"
#include <iostream>
#include <fstream>
#include <dirent.h>

using namespace std;

struct CachedImage {
    string filename;
    GrContext *ctx = NULL;
};

struct DatabaseItem {
    string drive;
    string folder;
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

struct MenuItemFile {
    string name;
    string folder;
    string menuFile;
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

    vector<DatabaseItem*>* _ScanMenuItemFiles(vector<string> scanFolders){
        vector<DatabaseItem*>* files = new vector<DatabaseItem*>();
        for (string folder : scanFolders){
            vector<DatabaseItem*> subfolderItems = _RecursivelyScanFolderForMenuFiles(folder);
            if (subfolderItems.size() == 0){
                continue;
            }
            for (DatabaseItem* item : subfolderItems){
                files->push_back(item);
            }
        }
        return files;
    }

    vector<DatabaseItem*> _RecursivelyScanFolderForMenuFiles(string folder){
        vector<DatabaseItem*> files;
        if (DIR * dir = opendir(folder.c_str())){
            while (dirent * f = readdir(dir)){
                //debugOut(std::string(f->d_name));
                if (!f->d_name || f->d_name[0] == '.'){
                    continue;
                }
                std::string filename = std::string(f->d_name);
                if (f->d_type == DT_DIR){
                    vector<DatabaseItem*> subFiles = _RecursivelyScanFolderForMenuFiles(folder + filename + "\\");
                    if (subFiles.size() > 0){
                        for (DatabaseItem* subFile : subFiles){
                            files.push_back(subFile);
                        }
                    }
                    continue;
                }
                if (filename == std::string("menu.cfg")){
                    files.push_back(_ReadMenuFile(folder + filename, folder));
                }
            }
            closedir(dir);
        }
        return files;
    }

    DatabaseItem* _ReadMenuFile(string filePath, string folderPath){
        //PathInfo *pathInfo = GetPathInfo(filePath);
        DatabaseItem* currentItem = new DatabaseItem;
        currentItem->folder = folderPath;
        //currentItem->drive = pathInfo->drive;
        //currentItem->folder = pathInfo->folder;
        fstream dbfile;
        dbfile.open(filePath.c_str(),ios::in);
        if (dbfile.is_open()){
            string tp;
            while(getline(dbfile, tp)){
                if (tp.length() == 0){
                    continue;
                }
                if (tp.at(0) == 0x5B){
                    //TITLE
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
                                currentItem->path = folderPath + itemValue;
                            } else if (key == "image"){
                                currentItem->image = folderPath + itemValue;
                            } else if (key == "developer"){
                                currentItem->developer = itemValue;
                            } else if (key == "year"){
                                currentItem->year = itemValue;
                            } else if (key == "notes"){
                                currentItem->notes = itemValue;
                            } else if (key == "readme"){
                                currentItem->readme = folderPath + itemValue;
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
            dbfile.close();
        }
        return currentItem;
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

    static vector<DatabaseItem*>* ScanMenuItemFiles(vector<string> scanFolders){
        return Current()->_ScanMenuItemFiles(scanFolders);
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
