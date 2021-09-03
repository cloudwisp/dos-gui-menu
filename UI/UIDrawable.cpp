
#ifndef UIDrawable_cpp
#define UIDrawable_cpp

#include <string>
#include <grx20.h>
#include "AppUI.h"
#include "../AppEvent.cpp"
#include "AppUITheme.h"


typedef UIDrawable *UIDrawableCollection[255];

class UIDrawable : public EventEmitter, public EventConsumer {
private:
	virtual void draw_internal() = 0;

	int indexOfChild(UIDrawable* child){
        int i;
        for (i = 0; i < childCount; i++){
            if (children[i]==child){
                return i;
            }
        }
	}

	void updateChildOrderIndex(int newIndex, int oldIndex) {
	    //expected to be called from the removeChild function in correct order (high/low)
        int i;
        for (i = 0; i < childDisplayOrderCount; i++){
            if (childDisplayOrder[i] == oldIndex){
                childDisplayOrder[i] == newIndex;
            }
        }
	}
	void removeChildOrderIndex(int removeIndex){
        int i, removed, z;
        removed = 0;
        for (i = childDisplayOrderCount-1; i >= 0; i--){
            if (childDisplayOrder[i]==removeIndex){
                for (z = i; z < childDisplayOrderCount-removed; z++){
                    childDisplayOrder[i] = childDisplayOrder[z];
                }
                removed++;
            }
        }
        childDisplayOrderCount-=removed;
	}

protected:

	bool needsRedraw = false;

    void UnfocusAllChildren(){
        int i;
        for (i = 0; i < childCount; i++){
            children[i]->UnFocus();
            children[i]->UnfocusAllChildren();
        }
    }

    UIDrawable *GetTopElement(){
        if (parent){
            return parent->GetTopElement();
        }
        return this;
    }

    void SendChildToBack(UIDrawable* child){
        int i, shifting, childInd;
		shifting = 0;
		childInd = indexOfChild(child);
		for (i = childDisplayOrderCount-1; i >= 0; i--){
			if (childDisplayOrder[i] == childInd){
				//begin shifting
				shifting = 1;
			}
			if (!shifting){
				continue;
			}
			if (i == 0){
				childDisplayOrder[i] = childInd;
			} else {
				childDisplayOrder[i] = childDisplayOrder[i-1];
			}
		}
		child->needsRedraw = true;
    }

    void BringChildToFront(UIDrawable* child){
        int i, shifting, childInd;
		shifting = 0;
		childInd = indexOfChild(child);
		for (i = 0; i < childDisplayOrderCount; i++){
			if (childDisplayOrder[i] == childInd){
				//begin shifting
				shifting = 1;
			}
			if (!shifting){
				continue;
			}
			if (i == childDisplayOrderCount-1){
				childDisplayOrder[i] = childInd;
			} else {
				childDisplayOrder[i] = childDisplayOrder[i+1];
			}
		}
		child->needsRedraw = true;
    }

	virtual void FocusNotify(UIDrawable* focusedDrawable){
	}

	Coord AbsolutePosition(){
		Coord absPos = {x, y};
		if (parent){
			Coord parentAbs = parent->AbsolutePosition();
			absPos.x = parentAbs.x + x;
			absPos.y = parentAbs.y + y;
		}
		return absPos;
	}

	//redraw boxes
	std::vector<BoxCoords> redrawBoxes = std::vector<BoxCoords>();
	void AddRedrawBox(int x1, int y1, int x2, int y2){
		BoxCoords coords = {x1,y1,x2,y2};
		redrawBoxes.push_back(coords);
	}
	void ClearRedrawBoxes(){
		redrawBoxes.clear();
		
		for (int i = 0; i < childCount; i++){
			children[i]->ClearRedrawBoxes();
		}
	}

	//child drawable container
	GrContext *innerContext = NULL;
	int innerContextX = 0;
	int innerContextY = 0;
	int innerWidth;
	int innerHeight;
	bool singleContext = false;

	Coord oldPosition = {-1,-1}; //kept for only 1 draw cycle to register a draw box for old pos.

public:
	GrContext *ctx = NULL;
	int x;
	int y;
	int width;
	int height;
	int visible;
	int freeze;

	int focus;
	int tabstop; // when set to a value over zero, will tell the window to focus the element during key tabbing in the order defined by this value.
	int containertabstop = 0; // when set to a value over zero, will group tab stop child elements together
	UIDrawable *parent = NULL;
	UIDrawable *children[255] = {NULL};
	UIDrawable *window = NULL;
    int childDisplayOrder[255];
	int childDisplayOrderCount = 0;
	int childCount = 0;
	int highlight = 0;

	GrContext *GetContext(){
		return ctx;
	}

	virtual void OnKeyUp(int KeyCode, int ShiftState, int Ascii) {}
	virtual void OnMouseOver(){}
	virtual void OnMouseOut(){}
	virtual void CheckInputs(){}
	virtual void Update(){}

	void UnHighlightAllChildren(){
		highlight = 0;
		for (int i = 0; i < childCount; i++){
			children[i]->UnHighlightAllChildren();
		}
		needsRedraw = true;
	}

	void Highlight(){
		highlight = 1;
		needsRedraw = true;
	}

	virtual void AddChild(UIDrawable *subElement){
		if (subElement == this){
			return;
		}
		subElement->parent = this;
		children[childCount] = subElement;
		childDisplayOrder[childDisplayOrderCount] = childCount;
		childCount++;
		childDisplayOrderCount++;
		if (window){
            //sets all children
            SetWindow(window);
		}
		needsRedraw = true;
	}

	virtual void RemoveChild(UIDrawable *subElement){
        int i, removed, z;
        removed = 0;
        for (i = childCount-1; i >= 0; i--){
            if (children[i]==subElement){
                removeChildOrderIndex(i);
                for (z = i; z < childCount-removed; z++){
                    children[i] = children[z];
                    updateChildOrderIndex(i,z); //new, old
                }
                removed++;
            }
        }
        childCount-=removed;
		needsRedraw = true;
	}

	void SetWindow(UIDrawable *theWindow){
        int i;
        window = theWindow;
        for (i = 0; i < childCount; i++){
            children[i]->SetWindow(theWindow);
        }
	}

	void Show(){
		visible = 1;
		needsRedraw = true;
	}

	void Hide(){
		visible = 0;
		needsRedraw = true;
	}

	void Focus(){
	    if (!focus){
            GetTopElement()->UnfocusAllChildren();
            focus = 1;
            EmitEvent("GotFocus");
			if (window){
				window->FocusNotify(this);
			}
			needsRedraw = true;
	    }
	}

	void UnFocus(){
		if (focus){
            focus = 0;
            EmitEvent("LostFocus");
			needsRedraw = true;
		}
	}

	void PropagateMouseEvent(int subX, int subY, const char *event){
		int i, o;
		
		for (o = childDisplayOrderCount-1; o >= 0; o--){
            i = childDisplayOrder[o];
			if (!children[i]->visible){ continue; }
			if (subX > children[i]->x + innerContextX
				 && subX < innerContextX + children[i]->x+children[i]->width
				 && subY > children[i]->y + innerContextY
				 && subY < innerContextY + children[i]->y + children[i]->height){
				children[i]->PropagateMouseEvent(subX-children[i]->x-innerContextX, subY-children[i]->y-innerContextY, event);
			}
		}
		EmitEvent(event, subX, subY);
	}

	void IdentifyVisibleElementsAtPosition(int subX, int subY, UIDrawableCollection elements, int *elementCount){
		int elIndex = *elementCount;
		elements[elIndex] = this;
		*elementCount = elIndex + 1;

		if (childCount == 0){
			return;
		}
		int o, i;
		for (o = childDisplayOrderCount-1; o >= 0; o--){
            i = childDisplayOrder[o];
			if (!children[i]->visible){ continue; }
			if (subX > children[i]->x + innerContextX
				&& subX < innerContextX + children[i]->x + children[i]->width
				&& subY > children[i]->y + innerContextY
				&& subY < innerContextY + children[i]->y + children[i]->height){
				children[i]->IdentifyVisibleElementsAtPosition(subX-children[i]->x-innerContextX, subY-children[i]->y-innerContextY, elements, elementCount);
			}
		}
	}

	void SendToBack(){
		if (parent == NULL){ return; }
		parent->SendChildToBack(this);
	}

	void BringToFront(){
		if (parent == NULL){ return; }
		parent->BringChildToFront(this);
	}

	void SetDimensions(int drawWidth, int drawHeight){
        GrDestroyContext(ctx);
        width = drawWidth;
        height = drawHeight;
        ctx = GrCreateContext(drawWidth, drawHeight, NULL, NULL);
        GrClearContextC(ctx,THEME_COLOR_TRANSPARENT);
		needsRedraw = true;
	}

	void SetInnerDimensions(int innerDrawWidth, int innerDrawHeight, int innerX, int innerY){
		if (singleContext){
			return;
		}
		GrDestroyContext(innerContext);
		innerWidth = innerDrawWidth;
		innerHeight = innerDrawHeight;
		innerContextX = innerX;
		innerContextY = innerY;
		innerContext = GrCreateContext(innerDrawWidth, innerDrawHeight, NULL, NULL);
		GrClearContextC(innerContext, THEME_COLOR_TRANSPARENT);
		needsRedraw = true;
	}

	void SetPosition(int posX, int posY){
		oldPosition = {x, y};
		x = posX;
		y = posY;
		needsRedraw = true;
	}

	UIDrawable(int drawWidth, int drawHeight) : UIDrawable(drawWidth, drawHeight, drawWidth, drawHeight, true) {
	}

	UIDrawable(int drawWidth, int drawHeight, int innerDrawWidth, int innerDrawHeight, bool asSingleContext){
		x = 0;
		y = 0;
		width = drawWidth;
		height = drawHeight;
		innerWidth = innerDrawWidth;
		innerHeight = innerDrawHeight;
		singleContext = asSingleContext;
		visible = 1;
		freeze = 0;
		focus = 0;
		tabstop = 0;
		childCount = 0;
		childDisplayOrderCount = 0;
		ctx = GrCreateContext(width, height, NULL, NULL);
		GrClearContextC(ctx,THEME_COLOR_TRANSPARENT);
		if (!asSingleContext){
			innerContext = GrCreateContext(innerWidth, innerHeight, NULL, NULL);
			GrClearContextC(innerContext, THEME_COLOR_TRANSPARENT);
		}
		needsRedraw = true;
	}

	~UIDrawable(){
		if (ctx){
			GrDestroyContext(ctx);
		}
		if (innerContext){
			GrDestroyContext(innerContext);
		}
	}

	bool NeedsRedraw(){
		if (needsRedraw){
			return true;
		}
		int i, o;
		for (o = 0; o < childDisplayOrderCount; o++){
			i = childDisplayOrder[o];
			if (children[i]->NeedsRedraw()){
				return true;
			}
		}
		return false;
	}

	//These methods blit redraw boxes in a stack from bottom to top directly against the screen buffer 
	void BlitBoxes(GrContext* ontoContext){
		//consolidate boxes?
		for (int i = 0; i < redrawBoxes.size(); i++){
			BoxCoords coords = redrawBoxes.at(i);
			GrFilledBox(coords.x1,coords.y1,coords.x2,coords.y2,THEME_COLOR_BLACK);
			BlitBox(coords.x1, coords.y1, coords, ontoContext);
			GrSetContext(ontoContext);
			//GrBox(0,0, width-1, height-1, THEME_HIGHLIGHT_BORDER);
		}
	}

	BoxCoords ScreenCoordsToLocalContext(BoxCoords screenCoords){
		Coord myPos = AbsolutePosition();
		BoxCoords adjusted = {
			screenCoords.x1 - myPos.x,
			screenCoords.y1 - myPos.y,
			screenCoords.x2 - myPos.x,
			screenCoords.y2 - myPos.y
		};
		return adjusted;
	}

	void BlitBox(int screenX, int screenY, BoxCoords coords, GrContext *ontoContext){
		
		BoxCoords myCoords = ScreenCoordsToLocalContext(coords);

		int mx1 = myCoords.x1 < 0 ? 0 : myCoords.x1;
		int my1 = myCoords.y1 < 0 ? 0 : myCoords.y1;
		int mx2 = myCoords.x2 > width - 1 ? width - 1 : myCoords.x2;
		int my2 = myCoords.y2 > height - 1 ? height - 1 : myCoords.y2;
		if (mx2-mx1 <= 0 || my2-my1 <= 0){
			return;
		}

		int screenOffsetX = myCoords.x1 < 0 ? 0-myCoords.x1 : 0;
		int screenOffsetY = myCoords.y1 < 0 ? 0-myCoords.y1 : 0;
		GrBitBlt(ontoContext, screenX + screenOffsetX, screenY + screenOffsetY, ctx, mx1, my1, mx2, my2, GrIMAGE);

		int i, o;
		for (o = 0; o < childDisplayOrderCount; o++){
			i = childDisplayOrder[o];
			if (!children[i]->visible){ continue; }
			// if (myCoords.x2 < children[i]->x + innerContextX
			// 	|| myCoords.x1 > children[i]->x + children[i]->width + innerContextX
			// 	|| myCoords.y1 < children[i]->y + innerContextY
			// 	|| myCoords.y2 > children[i]->y + children[i]->height + innerContextY){
			// 		//box is outside of child context area
			// 		continue;
			// 	}
				
			children[i]->BlitBox(screenX, screenY, coords, ontoContext);
		}
	}

	void DrawNew(bool boxFlaggedForRedraw){
		if (oldPosition.x > -1){
			//TODO: handle if parent has moved at same time.
			Coord parentPos = {0,0};
			if (parent){
				parentPos = parent->AbsolutePosition();
			}
			GetTopElement()->AddRedrawBox(parentPos.x+oldPosition.x, parentPos.y+oldPosition.y, parentPos.x + oldPosition.x + width - 1, parentPos.y + oldPosition.y + height - 1);
			oldPosition = {-1,-1};
		}
		bool fullContainerRedraw = false;
		if (needsRedraw){
			draw_internal();
			if (!boxFlaggedForRedraw){
				Coord myPos = AbsolutePosition();
				GetTopElement()->AddRedrawBox(myPos.x, myPos.y, myPos.x + width - 1, myPos.y + height - 1);
				boxFlaggedForRedraw = true;
			}
			needsRedraw = false;
			fullContainerRedraw = true; //indicate that the child boxes aren't relevant, since the full drawable was redrawn
			if (focus){
				GrSetContext(ctx);
				GrBox(0, 0, width-1, height-1, GrWhite());
			}
			if (highlight){
				GrSetContext(ctx);
				GrBox(0,0, width-1, height-1, THEME_HIGHLIGHT_BORDER);
			}
		}
		int i, o;
		for (o = 0; o < childDisplayOrderCount; o++){
			i = childDisplayOrder[o];
			if (!children[i]->visible){ continue; }
			children[i]->DrawNew(boxFlaggedForRedraw);
		}
	}

	void Draw(GrContext *ontoContext){
		GrContext* childCanvas;
		if (singleContext){
			childCanvas = ctx;
		} else {
			childCanvas = innerContext;
		}

		if (NeedsRedraw()){
			draw_internal();
			int i, o;
			for (o = 0; o < childDisplayOrderCount; o++){
                i = childDisplayOrder[o];
				if (!children[i]->visible){ continue; }
				children[i]->Draw(childCanvas);
			}
			needsRedraw = false;
		}
		if (ontoContext == NULL){ return; }
		if (focus){
			GrSetContext(ctx);
			GrBox(0, 0, width-1, height-1, GrWhite());
		}
		if (highlight){
			GrSetContext(ctx);
			GrBox(0,0, width-1, height-1, THEME_HIGHLIGHT_BORDER);
		}
		if (!singleContext){
			GrBitBltCount(ctx, innerContextX, innerContextY, innerContext, 0, 0, innerWidth-1, innerHeight-1,GrIMAGE);
		}
		GrBitBltCount(ontoContext,x,y,ctx,0,0,width-1,height-1,GrIMAGE);
	}
};


#endif