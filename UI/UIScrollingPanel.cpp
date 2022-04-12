#ifndef UIScrollingPanel_cpp
#define UIScrollingPanel_cpp

#include <grx20.h>
#include "AppUITheme.h"
#include "UIDrawable.cpp"
#include "UIPanel.cpp"

class UIScrollingPanel: public UIDrawable {
private:
    bool doScrollX = false;
    bool doScrollY = false;
    int scrollTop = 0;
    int scrollLeft = 0;
    int scrollArrowPad = 4;
    GrColor backgroundColor;
    UIDrawable* innerPanel;

    BoxCoords xBar;
    BoxCoords xBarButtonLeft;
    BoxCoords xBarButtonRight;
    BoxCoords xBarScrollPos;

    BoxCoords yBar;
    BoxCoords yBarButtonUp;
    BoxCoords yBarButtonDown;
    BoxCoords yBarScrollPos;


    void draw_internal(){
        GrClearContextC(ctx, backgroundColor);
        GrClearContextC(innerContext, THEME_COLOR_TRANSPARENT);
        GrSetContext(ctx);
        
        if (doScrollY){
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

        if (doScrollX){
            Draw3dButton(ctx, xBar, THEME_COLOR_SCROLLBAR_BACKGROUND, true);

            Draw3dButton(ctx, xBarButtonLeft, THEME_COLOR_SCROLLBAR_BUTTON_BG, false);
            Draw3dButton(ctx, xBarButtonRight, THEME_COLOR_SCROLLBAR_BUTTON_BG, false);
            Draw3dButton(ctx, xBarScrollPos, THEME_COLOR_SCROLLBAR_BUTTON_BG, false);
        }

    }

    void SetBoxCoords(){
        double yPercent = (double)scrollTop / (innerPanel->height - innerHeight);
        double xPercent = (double)scrollLeft / (innerPanel->width - innerWidth);
        int minPosWidth = THEME_SCROLLBAR_WIDTH;
        if (doScrollY){
            int maxY = height - 1;
            if (doScrollX){
                maxY = height - THEME_SCROLLBAR_WIDTH - 1;
            }

            int yBarLeft = width - THEME_SCROLLBAR_WIDTH;
            int yBarRight = width - 1;
            int innerBarHeight = maxY - (THEME_SCROLLBAR_WIDTH * 2);
            
            int posHeight = innerBarHeight - (innerPanel->height - innerHeight);
            int innerPosTop = THEME_SCROLLBAR_WIDTH + ((double)(innerBarHeight - posHeight) * yPercent);
            if (posHeight < minPosWidth){ posHeight = minPosWidth; }
        
            //scrollbar outer
            yBar = {yBarLeft, 0, yBarRight, maxY};
            yBarButtonUp = {yBarLeft, 0, yBarRight, THEME_SCROLLBAR_WIDTH};
            yBarButtonDown = {yBarLeft, maxY - THEME_SCROLLBAR_WIDTH, yBarRight, maxY};
            yBarScrollPos = {yBarLeft, innerPosTop, yBarRight, innerPosTop + posHeight};
        }

        if (doScrollX){
            int xBarTop = height - THEME_SCROLLBAR_WIDTH - 1;
            int xBarBottom = height - 1;
            int innerBarWidth = width - (THEME_SCROLLBAR_WIDTH * 2);
            int innerPosLeft = THEME_SCROLLBAR_WIDTH + ((innerBarWidth - THEME_SCROLLBAR_WIDTH) * xPercent);
            
            xBar = {0, xBarTop, width - 1, height - 1};
            xBarButtonLeft = {0, xBarTop, THEME_SCROLLBAR_WIDTH, height -1};
            xBarButtonRight = {width - THEME_SCROLLBAR_WIDTH, xBarTop, width - 1, height - 1};
            xBarScrollPos = {innerPosLeft, xBarTop, innerPosLeft = THEME_SCROLLBAR_WIDTH, xBarBottom};
        }
    }

    void update(){

    }

    void resizeInner(){
        int maxX = 0;
        int maxY = 0;
        for (int i = 0; i < innerPanel->childCount; i++){
            if (!innerPanel->children[i]->visible){
                continue;
            }
            int thisX = innerPanel->children[i]->x + innerPanel->children[i]->width;
            int thisY = innerPanel->children[i]->y + innerPanel->children[i]->height;
            if (thisX > maxX){ maxX = thisX; }
            if (thisY > maxY){ maxY = thisY; }
        }
        int newWidth = innerWidth;
        int newHeight = innerHeight;
        if (maxX > innerWidth){
            newWidth = innerWidth;
        }
        if (maxY > innerHeight){
            newHeight = maxY;
        }
        
        innerPanel->SetDimensions(newWidth, newHeight);
    }

protected:

public:

    void AddChild(UIDrawable *child) override {
        innerPanel->AddChild(child);
        resizeInner();
        SetBoxCoords();
        needsRedraw = true;
    }

    void RemoveChild(UIDrawable *child) override {
        innerPanel->RemoveChild(child);
        resizeInner();
        SetBoxCoords();
        needsRedraw = true;
    }

    bool IsChildFullyVisible(UIDrawable *child) {
        return (!doScrollY || (child->y >= scrollTop && child->y + child->height <= scrollTop + innerHeight))
        && (!doScrollX || (child->x >= scrollLeft && child->x + child->width <= scrollLeft + innerWidth));
    }

    void ScrollToChildFull(UIDrawable *child){
        if (IsChildFullyVisible(child)){
            return;
        }
        //if element is to the right or bottom, scroll left or top 
        if (child->x < scrollLeft){
            SetScrollLeft(child->x);
        } else {
            SetScrollLeft(child->x - innerWidth + child->width);
        }
        if (child->y < scrollTop){
            SetScrollTop(child->y);
        } else {
            SetScrollTop(child->y - innerHeight + child->height);
        }
    }

    void SetScrollTop(int yPos){
        if (!doScrollY){
            return;
        }
        scrollTop = yPos;
        if (scrollTop < 0){
            scrollTop = 0;
        }
        if (scrollTop > innerPanel->height - innerHeight){
            scrollTop = innerPanel->height - innerHeight;
        }
        innerPanel->y = 0-scrollTop;
        SetBoxCoords();
        needsRedraw = true;
    }

    void SetScrollLeft(int xPos){
        if (!doScrollX){
            return;
        }
        scrollLeft = xPos;
        if (scrollLeft < 0){
            scrollLeft = 0;
        }
        if (scrollLeft > innerPanel->width - innerWidth){
            scrollLeft = innerPanel->width - innerWidth;
        }
        innerPanel->x = 0-scrollLeft;
        SetBoxCoords();
        needsRedraw = true;
    }

    void ScrollUp(int pixels){
        SetScrollTop(scrollTop - pixels);
    }

    void ScrollDown(int pixels){
        SetScrollTop(scrollTop + pixels);
    }

    void ScrollLeft(int pixels){
        SetScrollLeft(scrollLeft - pixels);
    }

    void ScrollRight(int pixels){
        SetScrollLeft(scrollLeft + pixels);
    }

    UIScrollingPanel(bool stacked, GrColor bgColor, int drawWidth, int drawHeight, int padding, bool scrollX, bool scrollY): UIDrawable(
        drawWidth,
        drawHeight,
        scrollY ? drawWidth - (padding*2) - THEME_SCROLLBAR_WIDTH : drawWidth - (padding * 2),
        scrollX ? drawHeight - (padding * 2) - THEME_SCROLLBAR_WIDTH : drawHeight - (padding * 2),
        padding,
        padding,
        false){
            doScrollX = scrollX;
            doScrollY = scrollY;
            backgroundColor = bgColor;
            if (stacked){
                innerPanel = new UIStackedPanel(THEME_COLOR_TRANSPARENT, innerWidth, innerHeight);
            } else {
                innerPanel = new UIPanel(THEME_COLOR_TRANSPARENT, innerWidth, innerHeight);
            }
            UIDrawable::AddChild(innerPanel);
            resizeInner();
            SetBoxCoords();
    }

    ~UIScrollingPanel(){
        delete innerPanel;
    }
};

#endif