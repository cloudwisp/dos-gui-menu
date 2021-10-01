#ifndef UIListBox_cpp
#define UIListBox_cpp

#include <grx20.h>
#include <vector>
#include "../keyboard.h"
#include "UIDrawable.cpp"
#include "UITextArea.cpp"
#include "UIStackedPanel.cpp"


class UIListBox : public UIDrawable
{
private:
    std::vector<std::string> listItems;
    std::vector<UITextArea*> listItemText;
    UIScrollingPanel* panel;
    //UIDrawable *panel;
    int borderWidth = 2;
    int padding = 2;
    int selectedItem = 0;

    void draw_internal(){
        GrClearContextC(ctx, THEME_CONTROL_BACKGROUND);
    }

public:

    void OnEvent(EventEmitter *source, std::string event, EventData data){
        for (int i = 0; i < listItemText.size(); i++){
            if (selectedItem != i && source == listItemText.at(i) && event == "LeftMouseButtonUp"){
                SetSelectedItem(i);
            }
        }
    }

    void SetSelectedItem(int index){
        if (index < 0 || index > listItems.size() - 1){
            return;
        }
        selectedItem = index;
        for (int i = 0; i < listItemText.size(); i++){
            listItemText.at(i)->SetColor(THEME_CONTROL_TEXT, THEME_COLOR_TRANSPARENT);
        }
        listItemText.at(index)->SetColor(THEME_HIGHLIGHT_TEXT, THEME_HIGHLIGHT_BACKGROUND);
        EmitEvent("SelectedItemChanged", index);
        panel->ScrollToChildFull(listItemText.at(index));
        needsRedraw = true;
    }

    void OnKeyUp(int ScanCode, int ShiftState, int Ascii){
        if (ScanCode == KEY_UP_ARROW){
            SetSelectedItem(selectedItem-1);
        }
        if (ScanCode == KEY_DOWN_ARROW){
            SetSelectedItem(selectedItem+1);
        }
    }

    void AddItem(std::string item){
        listItems.push_back(item);
        UITextArea* newArea = new UITextArea(panel->innerWidth, UIHelpers::CharHeight(THEME_DEFAULT_FONT) + (padding * 2));
        newArea->SetText(item);
        newArea->BindEvent("LeftMouseButtonUp", this);
        listItemText.push_back(newArea);
        panel->AddChild(newArea);
        if (listItems.size() == 1){
            SetSelectedItem(0);
        }
        needsRedraw = true;
    }

    UIListBox(int width, int height) : UIDrawable(width, height){
        //panel = new UIStackedPanel(THEME_COLOR_TRANSPARENT, width - (borderWidth*2) - (padding*2), height - (borderWidth*2) - (padding*2));
        panel = new UIScrollingPanel(true, THEME_COLOR_TRANSPARENT, width, height, 4, true, true);
        AddChild(panel);
    }

    ~UIListBox(){
        delete panel;
        for (UITextArea* txt : listItemText){
            delete txt;
            listItems.clear();
            listItemText.clear();
        }
    }
};

#endif