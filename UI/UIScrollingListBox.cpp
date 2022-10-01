#ifndef UIScrollingListBox_CPP
#define UIScrollingListBox_CPP

#include <tgmath.h>
#include <grx20.h>
#include <vector>
#include "../keyboard.h"
#include "UIDrawable.cpp"
#include "UITextArea.cpp"
#include "UIStackedPanel.cpp"


class UIScrollingListBox : public UIDrawable {
private:
    std::vector<std::string> listItems = std::vector<std::string>(0);
    std::vector<UITextArea*> listItemText = std::vector<UITextArea*>(0);
    UIStackedPanel *panel = NULL;
    int borderWidth = 2;
    int padding = 0;
    int selectedItem = 0;

    int scrollTopIndex = 0;
    int itemHeight = 0;
    int panelHeight = 0;
    int panelWidth = 0;
    int maxVisibleItems = 1;
    GrColor background;
    GrColor foreground;
    GrFont *font = NULL;

    BoxCoords yBar;
    BoxCoords yBarButtonUp;
    BoxCoords yBarButtonDown;
    BoxCoords yBarScrollPos;
	BoxCoords yBarSpaceAbove;
	BoxCoords yBarSpaceBelow;
    int scrollArrowPad = 4;
	bool scrollBarVisible = false;
	int scrollRepeatMS = 250;
    
    void draw_internal(){
        GrClearContextC(ctx, background);
        if (scrollBarVisible){
			//whole scrollbar bg
            Draw3dButton(ctx, yBar, THEME_COLOR_SCROLLBAR_BACKGROUND, true);

            //up button
            Draw3dButton(ctx, yBarButtonUp, THEME_COLOR_SCROLLBAR_BUTTON_BG, false);
            
            //up arrow shape
            int poly[3][2] = {
                {yBarButtonUp.x1 + (THEME_SCROLLBAR_WIDTH / 2), yBarButtonUp.y1 + scrollArrowPad},
                {yBarButtonUp.x1 + scrollArrowPad, yBarButtonUp.y2 - scrollArrowPad},
                {yBarButtonUp.x2 - scrollArrowPad, yBarButtonUp.y2 - scrollArrowPad}
            };
            GrFilledPolygon(3,poly, THEME_COLOR_SCROLLBAR_BUTTON_FG);

            //down button
            Draw3dButton(ctx, yBarButtonDown, THEME_COLOR_SCROLLBAR_BUTTON_BG, false);

            //down arrow shape
            int dnpoly[3][2] = {
                {yBarButtonDown.x1 + scrollArrowPad, yBarButtonDown.y1 + scrollArrowPad},
                {yBarButtonDown.x1 + (THEME_SCROLLBAR_WIDTH / 2), yBarButtonDown.y2 - scrollArrowPad},
                {yBarButtonDown.x2 - scrollArrowPad, yBarButtonDown.y1 + scrollArrowPad}                
            };
            GrFilledPolygon(3,dnpoly, THEME_COLOR_SCROLLBAR_BUTTON_FG);

            //scroll position block.
            Draw3dButton(ctx, yBarScrollPos, THEME_COLOR_SCROLLBAR_BUTTON_BG, false);
		}
    }

	double yScrollPercent = 0;
	int innerScrollBarHeight = 0;
	int yBarGrabHeight = 0;
    void SetBoxCoords(){
		yScrollPercent = (double)ScrollTop / (panel->innerHeight - innerHeight);
        int minPosWidth = THEME_SCROLLBAR_WIDTH;
        int maxY = height - 1;
		int yBarLeft = width - THEME_SCROLLBAR_WIDTH;
		int yBarRight = width - 1;
		innerScrollBarHeight = maxY - (THEME_SCROLLBAR_WIDTH * 2);
		
		yBarGrabHeight = innerScrollBarHeight - (panel->innerHeight - innerHeight);
		if (yBarGrabHeight < minPosWidth){ yBarGrabHeight = minPosWidth; }
		int innerPosTop = THEME_SCROLLBAR_WIDTH + ((double)(innerScrollBarHeight - yBarGrabHeight) * yScrollPercent);
		
		//scrollbar outer
		yBar = {yBarLeft, 0, yBarRight, maxY};
		yBarButtonUp = {yBarLeft, 0, yBarRight, THEME_SCROLLBAR_WIDTH};
		yBarButtonDown = {yBarLeft, maxY - THEME_SCROLLBAR_WIDTH, yBarRight, maxY};
		yBarScrollPos = {yBarLeft, innerPosTop, yBarRight, innerPosTop + yBarGrabHeight};
		yBarSpaceAbove = {yBarLeft, THEME_SCROLLBAR_WIDTH, yBarRight, innerPosTop};
		yBarSpaceBelow = {yBarLeft, innerPosTop + yBarGrabHeight, yBarRight, maxY - THEME_SCROLLBAR_WIDTH};
		scrollBarVisible = panel->innerHeight > innerHeight;
    }

	//scrollBarPos relative to the top of the container.
	int ScrollBarPosToScrollTop(int scrollBarPos){
		double travelHeight = innerScrollBarHeight - yBarGrabHeight;
		double offsetPos = scrollBarPos - THEME_SCROLLBAR_WIDTH;
		double newPercent = offsetPos / travelHeight;
		return newPercent * (double)(panel->innerHeight - innerHeight);
	}

	int ScrollTop = 0;

    void ScrollToItem(int index){
        if (listItemText.size() == 0){
            return;
        }
        if (index < 0){
            index = 0;
        } else if (index > listItemText.size() - 1){
            index = listItemText.size() - 1;
        }
        int newTop = listItemText.at(index)->y;
        if (newTop > ScrollTop && newTop < ScrollTop + innerHeight){
            //already visible;
            return;
        }
        if (newTop < ScrollTop){
            //scrolling up, put the item at the top of the container
            ScrollToItemAtTop(index);
        } else {
            ScrollToItemAtBottom(index);
        }
    }

    void ScrollToItemAtTop(int index){
        if (listItemText.size() == 0){
            return;
        }
        if (index < 0){
            index = 0;
        } else if (index > listItemText.size() - 1){
            index = listItemText.size() - 1;
        }
        int newTop = listItemText.at(index)->y;
        ScrollTo(newTop);
    }

    void ScrollToItemAtBottom(int index){
        if (listItemText.size() == 0){
            return;
        }
        if (index < 0){
            index = 0;
        } else if (index > listItemText.size() - 1){
            index = listItemText.size() - 1;
        }
        UITextArea *item = listItemText.at(index);
        int newTop = item->y + item->height - innerHeight;
        ScrollTo(newTop);
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

    void Update(){
		if (inButtonHold && buttonHoldDir == -1){
			ScrollUp();
		} else if (inButtonHold){
			ScrollDown();
		}
		SetBoxCoords();
		UIDrawable::Update();
	}

    clock_t lastRepeatScroll;

	void CheckInputs() override {
		clock_t now = clock();
		if (clockToMilliseconds(now - lastRepeatScroll) > scrollRepeatMS){
			if (KeyState(KEY_UP_ARROW)){
				SelectUp();
			} else if (KeyState(KEY_DOWN_ARROW)){
				SelectDown();
			} else if (KeyState(KEY_PAGE_UP)){
				SelectPageUp();
			} else if (KeyState(KEY_PAGE_DOWN)){
				SelectPageDown();
			}
			lastRepeatScroll = clock();
		}
        UIDrawable::CheckInputs();
	}

public:

    bool inButtonHold = false;
	int buttonHoldDir = 0;
	bool inScrollDrag = false;
	int scrollDragOffset = 0;

    void OnEvent(EventEmitter *source, std::string event, EventData data){
        for (int i = 0; i < listItemText.size(); i++){
            if (selectedItem != i && source == listItemText.at(i) && event == "LeftMouseButtonUp"){
                SetSelectedItem(i);
                return;
            }
        }
        if (scrollBarVisible && event == "LeftMouseButtonDown"){
			int localX = data.data1;
			int localY = data.data2;
			if (CoordsIntersectBox(yBarButtonUp, localX, localY)){
				inButtonHold = true;
				buttonHoldDir = -1;
			} else if (CoordsIntersectBox(yBarButtonDown, localX, localY)){
				inButtonHold = true;
				buttonHoldDir = 1;
			} else if (CoordsIntersectBox(yBarScrollPos, localX, localY)){
				inScrollDrag = true;
				scrollDragOffset = yBarScrollPos.y1 - localY;
			}
		} else if (scrollBarVisible && event == "LeftMouseButtonUp"){
			if (!inButtonHold && !inScrollDrag){
				//check if empty space on either side of scrollbar is clicked
				if (CoordsIntersectBox(yBarSpaceAbove, data.data1, data.data2)){
					ScrollPageUp();
				} else if (CoordsIntersectBox(yBarSpaceBelow, data.data1, data.data2)){
					ScrollPageDown();
				}
			}
			inButtonHold = false;
			inScrollDrag = false;
			buttonHoldDir = 0;
		} else if (scrollBarVisible && event == "MouseMove"){
			if (inScrollDrag){
				int newScrollTop = ScrollBarPosToScrollTop(data.data2 + scrollDragOffset);
				ScrollTo(newScrollTop);
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
        if (ScanCode == KEY_DOWN_ARROW){
            SelectDown();
            return;
        } else if (ScanCode == KEY_UP_ARROW){
            SelectUp();
            return;
        }
        int dbIndexLetter = dbIndexOfChar(char(Ascii));
        if (dbIndexLetter > -1){
            SetSelectedItem(dbIndexLetter);
        }
    }

    void ScrollUp(){
        ScrollTo(ScrollTop - itemHeight);
    }
    void SelectUp(){
        SetSelectedItem(selectedItem-1);
    }

    void ScrollPageUp(){
        ScrollTo(ScrollTop - innerHeight);
    }
    void SelectPageUp(){
        SetSelectedItem(selectedItem-maxVisibleItems);
    }

    void ScrollDown(){
        ScrollTo(ScrollTop + itemHeight);
    }
    void SelectDown(){
        SetSelectedItem(selectedItem+1);
    }

    void ScrollPageDown(){
        ScrollTo(ScrollTop + innerHeight);
    }
    void SelectPageDown(){
        SetSelectedItem(selectedItem+maxVisibleItems);
    }

	void ScrollTo(int top){
		if (top > panel->height - height){
			top = panel->height - height;
		}
		if (top < 0){
			top = 0;
		}
		ScrollTop = top;
		panel->y = 0-ScrollTop;
		SetBoxCoords();
		needsRedraw = true;
	}

    void AddItem(std::string item){
        listItems.push_back(item);
        int itemTextWidth = GrFontStringWidth(font, item.c_str(), item.size(), 0);
        int lines = 1;
        if (itemTextWidth > panel->innerWidth){
            lines = ceil(itemTextWidth / (double)panel->innerWidth);
        }
        UITextArea* newArea = new UITextArea(panel->innerWidth, itemHeight * lines, padding);
        newArea->SetText(item);
        if (font){
            newArea->SetFont(font);
        }
        newArea->BindEvent("LeftMouseButtonUp", this);
        listItemText.push_back(newArea);
        panel->AddChild(newArea);
        panel->SizeHeightToContent(innerHeight);
        if (listItems.size() == 1){
            SetSelectedItem(0);
        }
        SetBoxCoords();
        needsRedraw = true;
    }

    void Clear(){
        for (int i = listItemText.size() - 1; i >= 0; i--){
            if (listItemText.at(i) != NULL){
                listItemText.at(i)->UnbindAllEventsForConsumer(this);
                panel->RemoveChild(listItemText.at(i));
                delete listItemText.at(i);
            }
        }
        listItems.clear();
        listItems.resize(0);
        listItemText.clear();
        listItemText.resize(0);
        SetBoxCoords();
        needsRedraw = true;
    }

    void SetColor(GrColor fg, GrColor bg){
        foreground = fg;
        background = bg;
        for (UITextArea *txt : listItemText){
            txt->SetColor(fg, THEME_COLOR_TRANSPARENT);
        }
        needsRedraw = true;
    }

    UIScrollingListBox(int width, int height, GrFont *itemFont) : UIDrawable(width, height, width - THEME_SCROLLBAR_WIDTH, height, 0, 0, false){
        font = itemFont;
        itemHeight = UIHelpers::CharHeight(itemFont) + (padding * 2);
        panelHeight = innerHeight - (borderWidth*2) - (padding*2);
        panelWidth = innerWidth - (borderWidth*2) - (padding*2);
        maxVisibleItems = panelHeight / itemHeight;
        background = THEME_CONTROL_BACKGROUND;
        foreground = THEME_CONTROL_TEXT;
        panel = new UIStackedPanel(THEME_COLOR_TRANSPARENT, panelWidth, panelHeight);
        AddChild(panel);
    }

    UIScrollingListBox(int width, int height) : UIScrollingListBox(width, height, UIHelpers::ResolveFont(THEME_DEFAULT_FONT)){
    }

    ~UIScrollingListBox(){
        delete panel;
        for (UITextArea* txt : listItemText){
            delete txt;
        }
        listItemText.clear();
    }
};

#endif