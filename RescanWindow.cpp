#ifndef RescanWindow_CPP
#define RescanWindow_CPP

#include "AppUI.cpp"

class RescanWindow : public UITitledWindow {
private:
    UITextArea *statusMessage = NULL;
    bool pendingRescan = false;
    clock_t pendingRescanStart;
    void _rescan(){
        pendingRescan = false;
        DatabaseItems = AppResources::ScanMenuItemFiles();
        EmitEvent("ScanComplete");
        Close();
    }
public:

    void Update(){
        if (pendingRescan){
            clock_t now = clock();
            if (clockToMilliseconds(now - pendingRescanStart) > 1000){
                _rescan();
            }
        }
    }

    std::vector<DatabaseItem*> * DatabaseItems = NULL;

    void RescanFolders(){
        if (pendingRescan){
            return;
        }
        pendingRescan = true;
        pendingRescanStart = clock();
    }

    RescanWindow(int width, int height) : UITitledWindow(width, height, "Scan folders"){
        statusMessage = new UITextArea(innerWidth, innerHeight);
        statusMessage->SetAlign(GR_ALIGN_CENTER, GR_ALIGN_CENTER);
        statusMessage->SetText("Scanning for menu items...");
        AddChild(statusMessage);
    }
    ~RescanWindow(){
        delete statusMessage;
    }

};

#endif