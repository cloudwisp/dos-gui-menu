
#ifndef UIDrawable_cpp
#define UIDrawable_cpp

#include <string>
#include <grx20.h>
#include "AppUI.h"
#include "../AppEvent.cpp"
#include "AppUITheme.h"
#include "UIHelpers.cpp"


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
			absPos.x = parentAbs.x + x + parent->innerContextX;
			absPos.y = parentAbs.y + y + parent->innerContextY;
		}
		return absPos;
	}

	BoxCoords AbsoluteBounds(){
		Coord absPos = AbsolutePosition();
		return {absPos.x, absPos.y, absPos.x + width, absPos.y + height};
	}

	BoxCoords AbsoluteInnerBounds(){
		Coord absPos = AbsolutePosition();
		return {absPos.x + innerContextX, absPos.y + innerContextY, absPos.x + innerContextX + innerWidth, absPos.y + innerContextY + innerHeight};
	}

	//redraw boxes - these are only relevant to the screen, but drawables don't have a hook on the screen, so it's on this class
	// accessed through GetTopElement()->AddRedrawBox
	std::vector<BoxCoords> redrawBoxes = std::vector<BoxCoords>();
	void AddRedrawBox(int x1, int y1, int x2, int y2){
		BoxCoords coords = {x1,y1,x2,y2};
		redrawBoxes.push_back(coords);
	}
	void ClearRedrawBoxes(){
		redrawBoxes.clear();
	}
	
	bool singleContext = false;

	Coord oldPosition = {-1,-1}; //kept for only 1 draw cycle to register a draw box for old pos.
	bool positionChangedSinceDraw = false;

public:
	GrContext *ctx = NULL;
	int x;
	int y;
	int width;
	int height;
	int visible;
	int freeze;
	
	//child drawable container
	GrContext *innerContext = NULL;
	int innerContextX = 0;
	int innerContextY = 0;
	int innerWidth;
	int innerHeight;

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
		if (parent == NULL){
			return;
		}
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
		positionChangedSinceDraw = true;
		x = posX;
		y = posY;
		needsRedraw = true;
	}

	int RedrawSize(){
		int totalArea = 0;
		for (int i = 0; i < redrawBoxes.size(); i++){
			BoxCoords box = redrawBoxes.at(i);
			totalArea  += ((box.x2-box.x1) * (box.y2-box.y1));
		}
		return totalArea;
	}

	UIDrawable(int drawWidth, int drawHeight) : UIDrawable(drawWidth, drawHeight, drawWidth, drawHeight, 0, 0, true) {
	}

	UIDrawable(int drawWidth, int drawHeight, int padding) : UIDrawable(drawWidth, drawHeight, drawWidth - (padding*2), drawHeight - (padding*2), padding, padding, padding == 0){
	}

	UIDrawable(int drawWidth, int drawHeight, int innerDrawWidth, int innerDrawHeight, int innerX, int innerY, bool asSingleContext){
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
			innerContextX = innerX;
			innerContextY = innerY;
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
		BoxCoords screenClip = {0,0,width - 1,height - 1};
		for (int i = 0; i < redrawBoxes.size(); i++){
			BoxCoords coords = redrawBoxes.at(i);
			GrSetContext(ontoContext);
			GrFilledBox(coords.x1,coords.y1,coords.x2,coords.y2,THEME_COLOR_BLACK);
			BlitBox(coords.x1, coords.y1, coords, ontoContext, screenClip);
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

	BoxCoords VisibleAbsoluteBox(){
		if (!parent){
			return BoxCoords {0, 0, width -1, height - 1};
		}
		if (!visible){
			return BoxCoords {0,0,0,0};
		}
		
		Coord myAbsolutePos = AbsolutePosition();
		BoxCoords parentClip = parent->VisibleAbsoluteBox();
		parentClip.x1 += parent->innerContextX;
		parentClip.y1 += parent->innerContextY;
		parentClip.x2 -= (parent->width - parent->innerWidth);
		parentClip.y2 -= (parent->height - parent->innerHeight);

		int minX = myAbsolutePos.x;
		int maxX = myAbsolutePos.x + width - 1;
		int minY = myAbsolutePos.y;
		int maxY = myAbsolutePos.y + height - 1;

		if (maxX < parentClip.x1
			|| minX > parentClip.x2
			|| maxY < parentClip.y1
			|| minY > parentClip.y2){
				//outside of parent's visible clip area
				return BoxCoords {0,0,0,0};
			}
		
		if (minX < parentClip.x1){
			minX = parentClip.x1;
		}
		if (maxX > parentClip.x2){
			maxX = parentClip.x2;
		}
		if (minY < parentClip.y1){
			minY = parentClip.y1;
		}
		if (maxY > parentClip.y2){
			maxY = parentClip.y2;
		}
		return BoxCoords {minX, minY, maxX, maxY};
	}

	clock_t lastUpdate = clock();

	void BlitBox(int screenX, int screenY, BoxCoords coords, GrContext *ontoContext, BoxCoords parentClip){
		BoxCoords clipped = coords;
		// if (parent){
		// 	clipped = BoxIntersection(coords, parent->AbsoluteInnerBounds());
		// 	if (clipped.x2-clipped.x1 <= 0 || clipped.y2-clipped.y1 <=0 ){ return; }
		// }

		clipped = BoxIntersection(parentClip, coords);
		if (clipped.x2-clipped.x1 <= 0 || clipped.y2-clipped.y1 <=0 ){ return; }
		int plotOffsetX = clipped.x1 - coords.x1; //if local area should be clipped below or after the target coordinate, it needs to be plotted on the screen further down or across
		int plotOffsetY = clipped.y1 - coords.y1;
		BoxCoords myCoords = ScreenCoordsToLocalContext(clipped);
		
		int mx1 = myCoords.x1 < 0 ? 0 : myCoords.x1;
		int my1 = myCoords.y1 < 0 ? 0 : myCoords.y1;
		int mx2 = myCoords.x2 > width - 1 ? width - 1 : myCoords.x2;
		int my2 = myCoords.y2 > height - 1 ? height - 1 : myCoords.y2;

		if (mx2-mx1 < 0 || my2-my1 < 0){
			return;
		}
		int screenOffsetX = myCoords.x1 < 0 ? 0-myCoords.x1 : 0;
		int screenOffsetY = myCoords.y1 < 0 ? 0-myCoords.y1 : 0;
		screenOffsetX += plotOffsetX;
		screenOffsetY += plotOffsetY;
		GrBitBlt(ontoContext, screenX + screenOffsetX, screenY + screenOffsetY, ctx, mx1, my1, mx2, my2, GrIMAGE);
		BoxCoords myAbsBox = AbsoluteInnerBounds();
		int i, o;
		for (o = 0; o < childDisplayOrderCount; o++){
			i = childDisplayOrder[o];
			if (!children[i]->visible){ continue; }
			// if (mx2 < children[i]->x + innerContextX
			// 	|| mx1 >= children[i]->x + children[i]->width + innerContextX
			// 	|| my2 < children[i]->y + innerContextY
			// 	|| my1 >= children[i]->y + children[i]->height + innerContextY){
			// 		//box is outside of child context area
			// 		continue;
			// 	}
				
			children[i]->BlitBox(screenX, screenY, coords, ontoContext, myAbsBox);
		}
	}

	void Draw(bool boxFlaggedForRedraw){
		
		bool fullContainerRedraw = false;
		if (needsRedraw){
			//if the container has moved, flag the previous location for redraw.
			if (positionChangedSinceDraw){
				//TODO: handle if parent has moved at same time.
				Coord parentPos = {0,0};
				if (parent){
					parentPos = parent->AbsolutePosition();
				}
				GetTopElement()->AddRedrawBox(parentPos.x+oldPosition.x, parentPos.y+oldPosition.y, parentPos.x + oldPosition.x + width - 1, parentPos.y + oldPosition.y + height - 1);
				positionChangedSinceDraw = false;
			}

			draw_internal();

			if (!singleContext){
				GrBitBlt(ctx, innerContextX, innerContextY, innerContext, 0, 0, innerWidth-1, innerHeight-1,GrIMAGE);
			}
			
			if (!boxFlaggedForRedraw){
				//Coord myPos = AbsolutePosition();
				//BoxCoords myPos = VisibleAbsoluteBox();
				//GetTopElement()->AddRedrawBox(myPos.x1, myPos.y1, myPos.x2, myPos.y2);
				Coord parentPos = {0,0};
				int offsetX = 0;
				int offsetY = 0;
				if (parent){
					parentPos = parent->AbsolutePosition();
					offsetX = parent->innerContextX;
					offsetY = parent->innerContextY;
				}
				GetTopElement()->AddRedrawBox(parentPos.x + offsetX + x, parentPos.y + offsetY + y, parentPos.x + offsetX + x + width - 1, parentPos.y + offsetY + y + height - 1);

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
		oldPosition = {x, y};
		int i, o;
		for (o = 0; o < childDisplayOrderCount; o++){
			i = childDisplayOrder[o];
			if (!children[i]->visible){ continue; }
			children[i]->Draw(boxFlaggedForRedraw);
		}
	}
};


#endif