#ifndef UIScrollingListBox_CPP
#define UIScrollingListBox_CPP

#include <grx20.h>
#include <vector>
#include "../keyboard.h"
#include "UIDrawable.cpp"
#include "UITextArea.cpp"
#include "UIStackedPanel.cpp"

class UIScrollingListBox : public UIDrawable {
private:
    std::vector<std::string> listItems;
    std::vector<UITextArea*> listItemText;
    UIStackedPanel *panel;
    int borderWidth = 2;
    int padding = 2;
    int selectedItem = 0;

    int scrollTopIndex = 0;
    int itemHeight = 0;
    int panelHeight = 0;
    int panelWidth = 0;
    int maxVisibleItems = 1;
    GrFont *font = NULL;
    
    void draw_internal(){
        GrClearContextC(ctx, THEME_CONTROL_BACKGROUND);
    }

    void ScrollToItem(int index){
        int availableHeight = panel->height;
        int numItems = availableHeight / itemHeight;
        if (index >= scrollTopIndex && index <= scrollTopIndex + maxVisibleItems){
            SetScrollTopIndex(scrollTopIndex); //force redraw at current index
            return;
        }
        if (index < scrollTopIndex){
            //scrolling up, put the item at the top of the container
            ScrollToItemAtTop(index);
        } else {
            ScrollToItemAtBottom(index);
        }
        
        
    }

    void ScrollToItemAtTop(int index){
        SetScrollTopIndex(index);
    }

    void ScrollToItemAtBottom(int index){
        SetScrollTopIndex(index - maxVisibleItems);
    }

    void SetScrollTopIndex(int index){
        if (index < 0){
            index = 0;
        } else if (index > listItemText.size() - 1){
            index = listItemText.size() - 1;
        }
        for (int i = 0; i < listItemText.size(); i++){
            if (i < index || i > index + maxVisibleItems){
                listItemText.at(i)->Hide();
            } else {
                listItemText.at(i)->Show();
            }
        }
        panel->ReFlow();
        scrollTopIndex = index;
    }

    int dbIndexOfChar(char c){
        for (int i = 0; i < listItems.size(); i++){
            if (listItems[i] == ""){
                continue;
            }
            if (tolower(listItems[i][0]) == tolower(c)){
                return i;
            }
        }
        return -1;
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
        if (index < 0){
            index = 0;
        }
        if (index > listItems.size() - 1){
            index = listItems.size() - 1;
        }
        if (index < 0){
            return;
        }
        selectedItem = index;
        for (int i = 0; i < listItemText.size(); i++){
            listItemText.at(i)->SetColor(THEME_CONTROL_TEXT, THEME_COLOR_TRANSPARENT);
        }
        listItemText.at(index)->SetColor(THEME_HIGHLIGHT_TEXT, THEME_HIGHLIGHT_BACKGROUND);
        EmitEvent("SelectedItemChanged", index);
        ScrollToItem(index);
        needsRedraw = true;
    }

    void OnKeyUp(int ScanCode, int ShiftState, int Ascii){
        if (ScanCode == KEY_UP_ARROW || ScanCode == KEY_KP_8){
            SetSelectedItem(selectedItem-1);
            return;
        }
        if (ScanCode == KEY_DOWN_ARROW || ScanCode == KEY_KP_2){
            SetSelectedItem(selectedItem+1);
            return;
        }
        if (ScanCode == KEY_PAGE_UP){
            SetSelectedItem(selectedItem-maxVisibleItems);
            return;
        }
        if (ScanCode == KEY_PAGE_DOWN){
            SetSelectedItem(selectedItem+maxVisibleItems);
            return;
        }
        int dbIndexLetter = dbIndexOfChar(char(Ascii));
        if (dbIndexLetter > -1){
            SetSelectedItem(dbIndexLetter);
        }
    }

    void AddItem(std::string item){
        listItems.push_back(item);
        UITextArea* newArea = new UITextArea(panel->innerWidth, itemHeight);
        newArea->Hide();
        newArea->SetText(item);
        if (font){
            newArea->SetFont(font);
        }
        newArea->BindEvent("LeftMouseButtonUp", this);
        listItemText.push_back(newArea);
        panel->AddChild(newArea);
        if (listItems.size() == 1){
            SetSelectedItem(0);
        }
        ScrollToItem(0);
        needsRedraw = true;
    }

    void SetFont(GrFont *listItemFont){
        font = listItemFont;
        if (listItemText.size() > 0){
            for (UITextArea *txt : listItemText){
                txt->SetFont(listItemFont);
            }
        }
    }

    UIScrollingListBox(int width, int height) : UIDrawable(width, height){
        itemHeight = UIHelpers::CharHeight(THEME_DEFAULT_FONT) + (padding * 2);
        panelHeight = height - (borderWidth*2) - (padding*2);
        panelWidth = width - (borderWidth*2) - (padding*2);
        maxVisibleItems = panelHeight / itemHeight;
        panel = new UIStackedPanel(THEME_COLOR_TRANSPARENT, panelWidth, panelHeight);
        AddChild(panel);
    }

    ~UIScrollingListBox(){
        delete panel;
        for (UITextArea* txt : listItemText){
            delete txt;
            listItems.clear();
            listItemText.clear();
        }
    }
};

#endif