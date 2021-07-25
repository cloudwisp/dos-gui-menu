#ifndef CharEditUI_cpp
#define CharEditUI_cpp

#include <string>
#include <grx20.h>
#include "../AppUI.cpp"
#include "../GameRes.cpp"
#include "../keyboard.h"
#include "../SpriteUtils.cpp"

class UISpriteSheetNavigator : public UIDrawable {
private:

    SpriteSet* spriteset = NULL;
    int spriteWidth = 16;
    int spriteHeight = 16;
    int cursorX = 0;
    int cursorY = 0;
    int scrollTop = 0;
    int scrollLeft = 0;
    int cursorId = 0;

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

    void updateCursorId(){
        cursorId = ((cursorY + 1) * spriteset->tilesWide) - (spriteset->tilesWide - cursorX);
    }

public:

    void OnKeyUp(int ScanCode, int ShiftState, int Ascii){
        if (!spriteset){
            return;
        }
        bool changed = false;
        if (ScanCode == KEY_LEFT_ARROW && cursorX > 0){
            cursorX--;
            if (cursorX * spriteWidth < scrollLeft){
                scrollLeft -= spriteWidth;
            }
            changed = true;
        }
        if (ScanCode == KEY_RIGHT_ARROW && cursorX < spriteset->tilesWide - 1){
            cursorX++;
            if (cursorX * spriteWidth > (scrollLeft + width - spriteWidth)){
                scrollLeft += spriteWidth;
            }
            changed = true;
        }
        if (ScanCode == KEY_UP_ARROW && cursorY > 0){
            cursorY--;
            if (cursorY * spriteHeight < scrollTop){
                scrollTop -= spriteHeight;
            }
            changed = true;
        }
        if (ScanCode == KEY_DOWN_ARROW && cursorY < spriteset->imHeight/spriteHeight){
            cursorY++;
            if (cursorY * spriteHeight > (scrollTop + height - spriteHeight)){
                scrollTop += spriteHeight;
            }
            changed = true;
        }
        updateCursorId();

        if (changed){
            EmitEvent("SpriteChanged", cursorId);
        }
        Unfreeze();
    }

    void SetSpriteSet(SpriteSet* sprites){
        spriteset = sprites;
        spriteWidth = spriteset->spriteWidth;
        spriteHeight = spriteset->spriteHeight;
        Unfreeze();
    }

    UISpriteSheetNavigator(int width, int height) : UIDrawable(width, height){
    }
};

class UISpritePreview : public UIDrawable{
private:
    GrImage* scaledImg = NULL;
    GrContext* spriteCtx = NULL;
    GrImage* spriteImg = NULL;
    SpriteSet* activeSpriteset = NULL;
    bool updated = false;
    float scaleFactor = 1;
    int feetW = 0;
    int feetH = 0;

    void draw_internal(){
        GrClearContextC(ctx, GrAllocColor(0,0,0));
        GrSetContext(ctx);
        if (scaledImg){
            GrImageDisplay(0,0,scaledImg);
        }

        //draw feet clipping box
        if (!activeSpriteset){
            Freeze();
            return;
        }
        int plotAnchorX = (activeSpriteset->spriteWidth / 2) * scaleFactor;
        int plotAnchorY = activeSpriteset->spriteHeight * scaleFactor;
        int x1 = plotAnchorX - ((feetW * scaleFactor) / 2);
        int x2 = plotAnchorX + ((feetW * scaleFactor) / 2);
        int y1 = plotAnchorY - (feetH * scaleFactor);
        int y2 = plotAnchorY;
        GrBox(x1,y1,x2,y2,GrAllocColor(255,0,255));
        Freeze();
    }
public:

    void SetFeet(int w, int h){
        feetW = w;
        feetH = h;
        Unfreeze();
    }

    void UpdateImage(SpriteSet* spriteset, int spriteId){
        activeSpriteset = spriteset;
        if (spriteCtx){
            GrDestroyContext(spriteCtx);
        }

        if (scaledImg){
            GrImageDestroy(scaledImg);
        }

        spriteCtx = GrCreateContext(spriteset->spriteWidth, spriteset->spriteHeight, NULL, NULL);
        GrClearContextC(spriteCtx, GrAllocColor(0,0,0));
        SpriteBoundingBox box = SpriteUtils::BlitSource(spriteId, spriteset->spriteWidth, spriteset->spriteHeight, spriteset->tilesWide);
        GrBitBlt(spriteCtx, 0, 0, spriteset->imagedata, box.x1, box.y1, box.x2, box.y2, GrNOCOLOR);
    
        float ratioX = (float)spriteset->spriteWidth / spriteset->spriteHeight;
        float ratioY = (float)spriteset->spriteHeight / spriteset->spriteWidth;

        spriteImg = GrImageFromContext(spriteCtx);
        int scaledWidth, scaledHeight;
        if (spriteset->spriteHeight > spriteset->spriteWidth){
            //fit vertically
            scaledWidth = width * ratioX;
            scaledHeight = height;
            scaledImg = GrImageStretch(spriteImg, scaledWidth, height);
        } else {
            //fit horizontally
            scaledWidth = width;
            scaledHeight = height * ratioY;
            scaledImg = GrImageStretch(spriteImg, width, scaledHeight);
        }
        scaleFactor = scaledWidth / spriteset->spriteWidth;
        Unfreeze();
    }

    UISpritePreview(int width, int height) : UIDrawable(width, height){
        if (scaledImg){
            GrImageDestroy(scaledImg);
        }
        if (spriteCtx){
            GrDestroyContext(spriteCtx);
        }
    }
};

#endif