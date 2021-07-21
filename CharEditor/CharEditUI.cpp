#ifndef CharEditUI_cpp
#define CharEditUI_cpp

#include <string>
#include <grx20.h>
#include "../AppUI.cpp"
#include "../GameRes.cpp"
#include "../keyboard.h"

class UISpriteSheetNavigator : public UIDrawable {
private:

    SpriteSet* spriteset = NULL;
    int spriteWidth = 16;
    int spriteHeight = 16;
    int cursorX = 0;
    int cursorY = 0;
    int scrollTop = 0;
    int scrollLeft = 0;

    void draw_internal(){
        if (!spriteset || !spriteset->imagedata){
            return;
        }
        GrClearContextC(ctx, GrAllocColor(0,0,0));
        GrBitBlt(ctx, 0, 0, spriteset->imagedata, scrollLeft, scrollTop, spriteset->imWidth, spriteset->imHeight, GrNOCOLOR);
        GrSetContext(ctx);
        GrBox((cursorX * spriteWidth) - scrollLeft, (cursorY * spriteHeight) - scrollTop, ((cursorX * spriteWidth) + spriteWidth) - scrollLeft, ((cursorY * spriteHeight) + spriteHeight) - scrollTop, GrAllocColor(255,255,255));
        Freeze();
    }

public:

    void OnKeyUp(int ScanCode, int ShiftState, int Ascii){
        if (!spriteset){
            return;
        }
        if (ScanCode == KEY_LEFT_ARROW && cursorX > 0){
            cursorX--;
            if (cursorX * spriteWidth < scrollLeft){
                scrollLeft -= spriteWidth;
            }
        }
        if (ScanCode == KEY_RIGHT_ARROW && cursorX < spriteset->tilesWide - 1){
            cursorX++;
            if (cursorX * spriteWidth > (scrollLeft + width - spriteWidth)){
                scrollLeft += spriteWidth;
            }
        }
        if (ScanCode == KEY_UP_ARROW && cursorY > 0){
            cursorY--;
            if (cursorY * spriteHeight < scrollTop){
                scrollTop -= spriteHeight;
            }
        }
        if (ScanCode == KEY_DOWN_ARROW && cursorY < spriteset->imHeight/spriteHeight){
            cursorY++;
            if (cursorY * spriteHeight > (scrollTop + height - spriteHeight)){
                scrollTop += spriteHeight;
            }
        }
        Unfreeze();
    }

    void SetSpriteSet(std::string setName){
        spriteset = GameResources::GetSpriteSet(setName);
        spriteWidth = spriteset->spriteWidth;
        spriteHeight = spriteset->spriteHeight;
        Unfreeze();
    }

    UISpriteSheetNavigator(int width, int height) : UIDrawable(width, height){
    }
};

#endif