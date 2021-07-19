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

    void draw_internal(){
        if (!spriteset || !spriteset->imagedata){
            return;
        }
        GrClearContextC(ctx, GrAllocColor(0,0,0));
        GrBitBlt(ctx, 0, 0, spriteset->imagedata, 0, 0, spriteset->imWidth, spriteset->imHeight, GrNOCOLOR);
        GrSetContext(ctx);
        GrBox(cursorX * spriteWidth, cursorY * spriteHeight, (cursorX * spriteWidth) + spriteWidth, (cursorY * spriteHeight) + spriteHeight, GrAllocColor(255,255,255));
        Freeze();
    }

public:

    void OnKeyUp(int ScanCode, int ShiftState, int Ascii){
        if (!spriteset){
            return;
        }
        if (ScanCode == KEY_LEFT_ARROW && cursorX > 0){
            cursorX--;
        }
        if (ScanCode == KEY_RIGHT_ARROW && cursorX < spriteset->tilesWide - 1){
            cursorX++;
        }
        if (ScanCode == KEY_UP_ARROW && cursorY > 0){
            cursorY--;
        }
        if (ScanCode == KEY_DOWN_ARROW && cursorY < spriteset->imHeight/spriteHeight){
            cursorY++;
        }
        Unfreeze();
    }

    void SetSpriteSet(std::string setName){
        spriteset = GameResources::GetSpriteSet(setName);
        spriteWidth = spriteset->spriteWidth;
        spriteHeight = spriteset->spriteHeight;
    }

    UISpriteSheetNavigator(int width, int height) : UIDrawable(width, height){
    }
};

#endif