#ifndef Res_CPP
#define Res_CPP


#include <vector>
#include <grx20.h>
#include <stdlib.h>
#include "Common.cpp"
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <list>
#include "GrExt/fpng2ctx.c"

using namespace std;

struct RESRGB {
    UINT8 r;
    UINT8 g;
    UINT8 b;
};

class InMemoryImage {
private:
    GrContext *src = NULL;
    RESRGB *cachedImage = NULL;
    int width = 0;
    int height = 0;
public:

    bool WasCached = false;

    string FileName;

    GrContext *ToContext(){
        GrContext *prevCtx = GrCurrentContext();
        GrContext *ctx = GrCreateContext(width, height, NULL, NULL);
        GrSetContext(ctx);
        if (!WasCached){
            return ctx;
        }
        int x, y, ci;
        GrColor *pColors = (GrColor*)malloc(sizeof(GrColor) * width);
        ci = 0;
        for (y = 0; y < height; y++){
            for (x = 0; x < width; x++){
                RESRGB cell = cachedImage[ci];
                pColors[x] = GrAllocColor(cell.r,cell.g,cell.b);
                ci++;
            }
            GrPutScanline(0, width - 1, y, pColors, GrWRITE);
        }
        free(pColors);
        GrSetContext(prevCtx);
        return ctx;
    }

    InMemoryImage(string filename, GrContext *source){
        FileName = filename;
        GrContext *prevCtx = GrCurrentContext();
        GrSetContext(source);
        width = GrSizeX();
        height = GrSizeY();
        cachedImage = (RESRGB*)malloc(sizeof(struct RESRGB) * width * height);
        if (cachedImage == NULL){
            //out of memory?
            GrSetContext(prevCtx);
            return;
        }
        GrColor *pColors = NULL;
        pColors = (GrColor*)malloc(width * sizeof(GrColor));
        if (pColors == NULL){
            //out of memory?
            free(cachedImage);
            GrSetContext(prevCtx);
            return;
        }
        int y = 0;
        int x = 0;
        int ci = 0;
        for (y = 0; y < height; y++){
            memcpy(pColors,GrGetScanline(0,width-1,y),sizeof(GrColor)*width);
            for (x = 0; x < width; x++){
                int r, g, b;
                GrQueryColor(pColors[x], &r, &g, &b);
                cachedImage[ci].r = r;
                cachedImage[ci].g = g;
                cachedImage[ci].b = b;
                ci++;
            }
        }
        WasCached = true;
        GrSetContext(prevCtx);
        free(pColors);
    }
    ~InMemoryImage(){
        if (cachedImage){
            free(cachedImage);
        }
    }
};

struct MenuConfig {
    string resolution;
    vector<string> *scanFolders;
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
    string setup;
    bool inlineDescription;
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

    MenuConfig *config = NULL;
    bool configLoaded = false;

    std::vector<InMemoryImage*> cachedImages;

	GrContext *_LoadImage(string filename){
        GrContext *result = NULL;
        if (filename.find(".png", 0) != std::string::npos){
            result = _LoadPng(filename);
        } else {
            result = _LoadPnm(filename);
        }
        return result;
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

    PathInfo* GetDirInfo(std::string path){
        size_t firstSlash = path.find('\\');
        string drive = path.substr(0, firstSlash);
        string folder = path.substr(firstSlash);
        return new PathInfo {drive, folder, ""};
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
            pathInfo = GetDirInfo(curDir);
            batch.write(pathInfo->drive.c_str(), pathInfo->drive.size());
            batch.write("\n",1);
            batch.write("cd ", 3);
            batch.write(pathInfo->folder.c_str(), pathInfo->folder.size());
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

    void _WriteMenuItemsLocal(vector<DatabaseItem*>* items){
        fstream menu;
        char* curDir = currentDir();
        menu.open("mnucache.db", ios::out);
        if (menu.is_open()){
            for (DatabaseItem* item : *items){
                menu.write("[", 1);
                menu.write(item->name.c_str(), item->name.size());
                menu.write("]\n", 2);
                menu.write("folder=", 7);
                menu.write(item->folder.c_str(), item->folder.size());
                menu.write("\n", 1);
                menu.write("genre=", 6);
                menu.write(item->genre.c_str(), item->genre.size());
                menu.write("\n", 1);
                menu.write("path=", 5);
                menu.write(item->path.c_str(), item->path.size());
                menu.write("\n", 1);
                menu.write("image=", 6);
                menu.write(item->image.c_str(), item->image.size());
                menu.write("\n", 1);
                menu.write("developer=", 10);
                menu.write(item->developer.c_str(), item->developer.size());
                menu.write("\n", 1);
                menu.write("year=", 5);
                menu.write(item->year.c_str(), item->year.size());
                menu.write("\n", 1);
                menu.write("notes=", 6);
                menu.write(item->notes.c_str(), item->notes.size());
                menu.write("\n", 1);
                menu.write("readme=", 7);
                menu.write(item->readme.c_str(), item->readme.size());
                menu.write("\n", 1);
                menu.write("setup=", 6);
                menu.write(item->setup.c_str(), item->setup.size());
                menu.write("\n", 1);
                menu.write("favorite=", 9);
                menu.write(item->favorite ? "T" : "F", 1);
                menu.write("\n", 1);
                menu.write("cd=", 3);
                menu.write(item->cd ? "T" : "F", 1);
                menu.write("\n", 1);
                menu.write("description=", 12);
                if (item->inlineDescription){
                    menu.write("T", 1);
                }
                menu.write("\n\n", 2);
            }
            
            menu.close();
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
        _WriteMenuItemsLocal(files);
        return files;
    }

    vector<DatabaseItem*>* _ScanMenuItemFiles(){
        vector<string> *folders = GetMenuConfig()->scanFolders;
        return _ScanMenuItemFiles(*folders);
    }

    vector<DatabaseItem*>* _GetCachedMenuItems(){
        vector<DatabaseItem*>* files = new vector<DatabaseItem*>();
        char* curDir = currentDir();
        string folder = std::string(curDir);
        string filePath = folder + "\\mnucache.db";
        for (DatabaseItem* item : _ReadMenuFile(filePath, folder, true)){
            files->push_back(item);
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
                if (filename == std::string("_menu.cfg")){
                    vector<DatabaseItem*> subFiles = _ReadMenuFile(folder + filename, folder, false);
                    if (subFiles.size() > 0){
                        for (DatabaseItem* subFile : subFiles){
                            files.push_back(subFile);
                        }
                    }
                }
            }
            closedir(dir);
        }
        return files;
    }

    void _LoadMenuConfig(string configFile){
        config = new MenuConfig();
        fstream dbfile;
        vector<string> * scanFolders = new vector<string>();
        dbfile.open(configFile.c_str(),ios::in);
        if (dbfile.is_open()){
            string tp;
            bool inFoldersList = false;
            bool inOptions = false; 
            while(getline(dbfile, tp)){
                if (tp.length() == 0){
                    if (inFoldersList){
                        inFoldersList = false;
                        config->scanFolders = scanFolders;
                    }
                    continue;
                }
                if (tp.at(0) == 0x5B){
                    string itemName = tp.substr(1,tp.length()-2);
                    if (itemName == "scan"){
                        inFoldersList = true;
                        inOptions = false;
                    } else if (itemName == "options"){
                        inOptions = true;
                        inFoldersList = false;
                    }
                } else if (inFoldersList) {
                    scanFolders->push_back(string(tp));
                } else if (inOptions){
                    string key;
                    for (int i = 0; i < tp.length(); i++){
                        if (tp.at(i) == 0x3D && tp.length() > i+1){
                            //check for supported keys and store value from equals onwards and then break;
                            string itemValue = tp.substr(i+1);
                            if (key == "resolution"){
                                config->resolution = itemValue;
                            }
                            break;
                        }
                        key += tp.at(i);
                    }
                }
            }
            dbfile.close();
        }
        configLoaded = true;
    }

    vector<DatabaseItem*> _ReadMenuFile(string filePath, string folderPath, bool isCachedWithFullPaths){
        //PathInfo *pathInfo = GetPathInfo(filePath);
        //currentItem->drive = pathInfo->drive;
        //currentItem->folder = pathInfo->folder;
        vector<DatabaseItem*> items;
        fstream dbfile;
        DatabaseItem *currentItem = NULL;
        dbfile.open(filePath.c_str(),ios::in);
        if (dbfile.is_open()){
            string tp;
            while(getline(dbfile, tp)){
                if (tp.length() == 0){
                    continue;
                }
                if (tp.at(0) == 0x5B){
                    //TITLE
                    if (currentItem){
                        items.push_back(currentItem);
                    }
                    currentItem = new DatabaseItem;
                    currentItem->folder = folderPath;
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
                                if (!isCachedWithFullPaths){
                                    currentItem->path = folderPath + itemValue;
                                } else {
                                    currentItem->path = itemValue;
                                }
                            } else if (key == "image"){
                                if (!isCachedWithFullPaths){
                                    currentItem->image = folderPath + itemValue;
                                } else {
                                    currentItem->image = itemValue;
                                }
                            } else if (key == "developer"){
                                currentItem->developer = itemValue;
                            } else if (key == "year"){
                                currentItem->year = itemValue;
                            } else if (key == "notes"){
                                currentItem->notes = itemValue;
                            } else if (key == "readme"){
                                if (!isCachedWithFullPaths){
                                    currentItem->readme = folderPath + itemValue;
                                } else {
                                    currentItem->readme = itemValue;
                                }
                            } else if (key == "favorite"){
                                currentItem->favorite = itemValue == "T";
                            } else if (key == "cd"){
                                currentItem->cd = itemValue == "T";
                            } else if (key == "description"){
                                currentItem->inlineDescription = true;
                            } else if (key == "folder"){
                                folderPath = itemValue;
                                currentItem->folder = itemValue;
                            } else if (key == "setup"){
                                if (!isCachedWithFullPaths){
                                    currentItem->setup = folderPath + itemValue;
                                } else {
                                    currentItem->setup = itemValue;
                                }
                            }
                            break;
                        }
                        key += tp.at(i);
                    }
                }
            }
            dbfile.close();
        }
        if (currentItem){
            items.push_back(currentItem);
        }
        return items;
    }

    string _GetInlineDescription(string menuFile, string menuItem){
        fstream dbfile;
        string description;
        string currentItemName;
        bool inMatchingItem = false;
        bool capturingDescription = false;
        bool anyDelimInLine = false;
        dbfile.open(menuFile.c_str(),ios::in);
        if (dbfile.is_open()){
            string tp;
            while(getline(dbfile, tp)){
                if (tp.length() == 0 && !capturingDescription){
                    continue;
                } else if (tp.length() == 0 && capturingDescription){
                    description += "\r\n";
                    continue;
                }
                if (tp.at(0) == 0x5B){
                    if (inMatchingItem){
                        //new item in multi-item config file - bail out.
                        break;
                    }
                    //new item
                    string itemName = tp.substr(1,tp.length()-2);
                    if (itemName == menuItem){
                        inMatchingItem = true;
                    } else {
                        inMatchingItem = false;
                    }
                } else if (inMatchingItem) {
                    //scan string until = separator
                    string key;
                    anyDelimInLine = false;
                    bool firstOfDescription = false;
                    for (int i = 0; i < tp.length(); i++){
                        if (tp.at(i) == 0x3D){
                            anyDelimInLine = true;
                            if (key == "description"){
                                capturingDescription = true;
                                firstOfDescription = true;
                                if (tp.length() > i + 1){
                                    string itemValue = tp.substr(i+1);
                                    description += itemValue + "\r\n";
                                }
                            }
                        }
                        key += tp.at(i);
                    }
                    if (!anyDelimInLine && capturingDescription){
                        description += tp + "\r\n";
                    } else if (anyDelimInLine && !firstOfDescription){
                        capturingDescription = false;
                    }
                }
            }
            dbfile.close();
            return description;
        }
    }

public:

    static AppResources* Current(){
        if (!self){
            self = new AppResources();
        }
        return self;
    }

    static MenuConfig* GetMenuConfig(){
        AppResources *cur = Current();
        if (cur->configLoaded){
            return cur->config;
        } else {
            cur->_LoadMenuConfig("menu.cfg");
            return cur->config;
        }
    }

    static GrContext *LoadImage(string filename){
        return Current()->_LoadImage(filename);
	}

    static vector<DatabaseItem*>* ScanMenuItemFiles(vector<string> scanFolders){
        return Current()->_ScanMenuItemFiles(scanFolders);
    }

    static vector<DatabaseItem*>* ScanMenuItemFiles(){
        return Current()->_ScanMenuItemFiles();
    }

    static std::vector<DatabaseItem*>* GetCachedMenuItems(){
        return Current()->_GetCachedMenuItems();
    }

    static std::string GetReadme(string filename){
        return Current()->_GetReadme(filename);
    }

    static std::string GetInlineDescription(string menuFile, string itemName){
        return Current()->_GetInlineDescription(menuFile, itemName);
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
        if (configLoaded){
            config->scanFolders->clear();
            delete config->scanFolders;
            delete config;
        }
    }

};


AppResources* AppResources::self = NULL;

#endif // Res_CPP
