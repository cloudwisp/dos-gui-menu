#ifndef GameInv_CPP
#define GameInv_CPP

#include "GameRes.cpp"
#include "GameElem.cpp"
#include "AppUI.cpp"

class GameInventory : public UIWindow {

private:
    int slots;
    GameWorldElement* items[255] = {NULL};
    GameWorldElement* dropItems[255] = {NULL};
    GameWorldElement *hero;
    int itemCount = 0;
    int dropItemCount = 0;
    int minBlankSlot = 0;
    int full = 0;
    int highlightedSlot = 0;
    int itemsWide;
    int spacing = 4;
    UITextArea *nameLabel;

    void updateStats(){
        int i;
        for (i = 0; i < slots; i++){
            if (items[i] == NULL){
                minBlankSlot = i;
                return;
            }
        }
        full = 1; //1+ size
    }

    void refreshInventory(){
        int i;
        for (i = 0; i < slots; i++){
            if (items[i] == NULL){
                continue;
            }
            if (items[i]->deleted){
                //remove from inventory;
                RemoveItem(i);
            }
        }
    }

    void draw_internal(){
        GrClearContextC(ctx, GrAllocColor(10,10,10));
        int x, y, i, row, col, tileX, tileY, subX, subY, spriteWidth, spriteHeight;
        spriteWidth = GameResources::tileWidth;
        spriteHeight = GameResources::tileHeight;
        for (i = 0; i < slots; i++){

            row = i / itemsWide;
            col = i - (row * itemsWide);
            x = (col * (spriteWidth + spacing)) + spacing;
            y = (row * (spriteHeight + spacing)) + spacing;
            //highlighting
            if (i == highlightedSlot){
                //rect(ctx, x, y, x+spriteWidth, y + spriteHeight, makecol(255,255,255));
                GrSetContext(ctx);
                GrBox(x, y, x+spriteWidth, y+spriteHeight, GrWhite());
            } else {
                //rect(ctx, x, y, x+spriteWidth, y + spriteHeight, makecol(100,100,100));
                GrSetContext(ctx);
                GrBox(x, y, x+spriteWidth, y+spriteHeight, GrAllocColor(100,100,100));
            }
            //sprite itself
            if (items[i] == NULL){ continue; }
            if (items[i]->spriteset_loaded == 0){ continue; }
            tileX = items[i]->spriteId % items[i]->spriteset->tilesWide;
            tileY = (items[i]->spriteId / items[i]->spriteset->tilesWide);
            subX = tileX * spriteWidth;
            subY = tileY * spriteHeight;
            GrBitBlt(ctx,x,y,items[i]->spriteset->imagedata,subX,subY,subX+spriteWidth-1,subY+spriteHeight-1,GrIMAGE);
            //masked_blit(items[i]->spriteset->imagedata, ctx, subX, subY, x, y, spriteWidth-1, spriteHeight-1);
        }
    }

    void CheckInputs(){
        UIDrawable::CheckInputs();
    }

    void OnKeyUp(int ScanCode, int ShiftState, int Ascii){
        UIWindow::OnKeyUp(ScanCode, ShiftState, Ascii);
        if (ScanCode == KEY_RIGHT_ARROW){
            if (highlightedSlot >= slots-1){ return; }
            highlightedSlot++;
        } else if (ScanCode == KEY_LEFT_ARROW){
            if (highlightedSlot == 0){
                return;
            }
            highlightedSlot--;
        } else if (ScanCode == KEY_DOWN_ARROW){
            if (highlightedSlot + itemsWide >= slots){ return; }
            highlightedSlot += itemsWide;
        } else if (ScanCode == KEY_UP_ARROW){
            if (highlightedSlot - itemsWide < 0){ return; }
            highlightedSlot -= itemsWide;
        } else if (ScanCode == KEY_ENTER){
            //activate
            if (items[highlightedSlot]){
                items[highlightedSlot]->OnInteraction(hero);
            }
        } else if (ScanCode == KEY_D && items[highlightedSlot] && items[highlightedSlot]->allowDrop){
            DropItem(highlightedSlot);
        } else if (ScanCode == KEY_ESC || ScanCode == KEY_I){
            Close();
        }
    }

    void DropItem(int invIndex){
        dropItems[dropItemCount] = items[invIndex];
        dropItemCount++;
        RemoveItem(invIndex);
        updateStats();
    }

public:

    int AddItem(GameWorldElement *item){
        if (full){ return 0; }
        items[minBlankSlot] = item;
        updateStats();
        return 1;
    }

    void RemoveItem(int invIndex){
        items[invIndex] = NULL;
        updateStats();
    }

    void RemoveItem(GameWorldElement *item){
        int i;
        for (i = 0; i < slots; i++){
            if (items[i] && items[i] == item){
                RemoveItem(i);
            }
        }
        updateStats();
    }

    int HasDropItem(){
        return (dropItemCount > 0);
    }
    GameWorldElement* GetNextDropItem(){
        if (dropItemCount == 0){ return NULL; }
        GameWorldElement* nextItem = dropItems[dropItemCount-1];
        dropItems[dropItemCount-1] = NULL;
        dropItemCount--;
        return nextItem;
    }

    void Update(){
        refreshInventory();
        if (items[highlightedSlot]){
            nameLabel->SetText(items[highlightedSlot]->name);
        } else {
            nameLabel->SetText("");
        }
    }

    GameInventory(GameWorldElement *gameHero, int inventorySlots, int drawWidth, int drawHeight) : UIWindow(drawWidth, drawHeight) {
        slots = inventorySlots;
        hero = gameHero;
        itemsWide = drawWidth / (GameResources::tileWidth + (spacing*2));
        nameLabel = new UITextArea(drawWidth, 20);
        nameLabel->y = drawHeight-20;
        nameLabel->x = 0;
        nameLabel->SetAlign(CW_ALIGN_CENTER,CW_ALIGN_CENTER);
        AddChild(nameLabel);
        nameLabel->BringToFront();
    }

};

#endif // GameInv_CPP
