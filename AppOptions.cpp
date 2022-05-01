#ifndef AppOptions_CPP
#define AppOptions_CPP

#include "AppUI.cpp"
#include "map"

#define SELECTOR_MENU_ITEM_RESCAN 1
#define SELECTOR_MENU_ITEM_EXIT 2

class AppOptionsWindow : public UITitledWindow {
private:
    std::map<int, std::string> menuItems = { {SELECTOR_MENU_ITEM_RESCAN,"Rescan Folders"},{SELECTOR_MENU_ITEM_EXIT,"Quit"} };
    std::map<int, UIButton*> menuButtons;
    UIStackedPanel *buttonContainer = NULL;

public:

    void OnEvent(EventEmitter* source, std::string event, EventData data){
        if (event == "Click"){
            for (auto item : menuButtons){
                if (source == item.second){
                    EmitEvent("ItemClicked", item.first);
                }
            }
        }
        UITitledWindow::OnEvent(source, event, data);
    }

    AppOptionsWindow(int width, int height) : UITitledWindow(width, height, "Menu"){
        int buttonWidth = width * 0.75;
        int buttonHeight = 20;
        int menuPadding = 4;
        buttonContainer = new UIStackedPanel(THEME_COLOR_TRANSPARENT,buttonWidth, height - menuPadding, 0, menuPadding);
        buttonContainer->y = menuPadding;
        buttonContainer->x = (width - buttonContainer->width) / 2;
        AddChild(buttonContainer);
        for (auto item : menuItems){
            UIButton *thisButton = new UIButton(buttonWidth, buttonHeight);
            thisButton->SetText((char*)(item.second.c_str()));
            thisButton->BindEvent("Click", this);
            menuButtons.insert(std::make_pair(item.first, thisButton));
            buttonContainer->AddChild(thisButton);
        }

    }
};

#endif