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
    UIStackedPanel* panel;
    int borderWidth = 2;
    int padding = 2;
    int selectedItem = 0;

    void draw_internal(){
        GrClearContextC(ctx, GrAllocColor(0,0,0));
    }

public:

    void SetSelectedItem(int index){
        if (index < 0 || index > listItems.size() - 1){
            return;
        }
        selectedItem = index;
        for (int i = 0; i < listItemText.size() - 1; i++){
            listItemText.at(i)->SetColor(GrAllocColor(255,255,255), GrAllocColor(0,0,0));
        }
        listItemText.at(index)->SetColor(GrBlack(), GrAllocColor(255,255,255));
        EmitEvent("SelectedItemChanged", index);
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
        UITextArea* newArea = new UITextArea(panel->width, UIHelpers::CharHeight(THEME_DEFAULT_FONT) + (padding * 2));
        newArea->SetText(item);
        listItemText.push_back(newArea);
        panel->AddChild(newArea);
        if (listItems.size() == 1){
            SetSelectedItem(0);
        }
    }

    UIListBox(int width, int height) : UIDrawable(width, height){
        panel = new UIStackedPanel(GrAllocColor(0,0,0), width - (borderWidth*2) - (padding*2), height - (borderWidth*2) - (padding*2));
        AddChild(panel);
    }

    ~UIListBox(){
        for (UITextArea* txt : listItemText){
            delete txt;
            listItems.clear();
            listItemText.clear();
        }
    }
};

#endif