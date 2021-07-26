#ifndef CharEditUI_cpp
#define CharEditUI_cpp

#include <string>
#include <grx20.h>
#include <math.h>
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
    int scaledWidth = 1;
    int scaledHeight = 1;

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
        //draw sprite boundary
        GrBox(0,0, scaledWidth -1, scaledHeight - 1, GrAllocColor(255,255,255));
        if (feetW == 0 || feetH == 0){
            return;
        }

        int plotAnchorX = roundf(((activeSpriteset->spriteWidth / 2) + 1) * scaleFactor);
        int plotAnchorY = roundf((activeSpriteset->spriteHeight + 2) * scaleFactor);
        int x1 = plotAnchorX - roundf((feetW * scaleFactor) / 2);
        int x2 = plotAnchorX + roundf((feetW * scaleFactor) / 2);
        int y1 = plotAnchorY - roundf(feetH * scaleFactor);
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

        if (spriteId == -1){
            return;
        }

        spriteCtx = GrCreateContext(spriteset->spriteWidth, spriteset->spriteHeight, NULL, NULL);
        GrClearContextC(spriteCtx, GrAllocColor(0,0,0));
        SpriteBoundingBox box = SpriteUtils::BlitSource(spriteId, spriteset->spriteWidth, spriteset->spriteHeight, spriteset->tilesWide);
        GrBitBlt(spriteCtx, 0, 0, spriteset->imagedata, box.x1, box.y1, box.x2, box.y2, GrNOCOLOR);
    
        float ratioX = (float)spriteset->spriteWidth / spriteset->spriteHeight;
        float ratioY = (float)spriteset->spriteHeight / spriteset->spriteWidth;

        spriteImg = GrImageFromContext(spriteCtx);
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
    }

    ~UISpritePreview(){
        if (scaledImg){
            GrImageDestroy(scaledImg);
        }
        if (spriteCtx){
            GrDestroyContext(spriteCtx);
        }
    }
};

class UISpriteAnimationFrameEditor : public UIDrawable {
private:
    int nRows = 0;
    int nCols = 0;
    UISpritePreview* spritePreviews[20] = {NULL};
    SpriteSet* activeSpriteset = NULL;

    void createPreviews(){
        int curCol = 0;
        int curRow = 0;
        int cellSize = width / nCols;
        for (int i = 0; i < 20; i++){
            spritePreviews[i] = new UISpritePreview(cellSize, cellSize);
            spritePreviews[i]->x = curCol * cellSize;
            spritePreviews[i]->y = curRow * cellSize;
            AddChild(spritePreviews[i]);
            if (curCol + 1 > nCols - 1){
                curCol = 0;
                curRow++;
            } else {
                curCol++;
            }
        }
    }

    void draw_internal(){
        GrClearContextC(ctx, GrAllocColor(0,0,0));
    }

public:

    void LoadAnimation(int animation[20], SpriteSet* spriteset){
        activeSpriteset = spriteset;
        for (int i = 0; i < 20; i++){
            spritePreviews[i]->UpdateImage(spriteset, animation[i]);
        }
    }

    UISpriteAnimationFrameEditor(int drawWidth, int drawHeight, int cols, int rows) : UIDrawable(drawWidth, drawHeight){
        nCols = cols;
        nRows = rows;
        createPreviews();
    }

    ~UISpriteAnimationFrameEditor(){
        for (int i = 0; i < 20; i++){
            delete spritePreviews[i];
            spritePreviews[i] = NULL;
        }
    }
};

#endif