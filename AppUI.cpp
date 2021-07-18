#ifndef AppUI_CPP
#define AppUI_CPP
#include <string>
#include <vector>
#include <grx20.h>
#include <pc.h>
#include "AppUI.h"
#include "AppEvent.cpp"

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
    }

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


	GrContext *GetContext(){
		return ctx;
	}

	virtual void OnKeyUp(int KeyCode, int ShiftState, int Ascii) {}
	virtual void OnMouseOver(){}
	virtual void OnMouseOut(){}
	virtual void CheckInputs(){}
	virtual void Update(){}

	void AddChild(UIDrawable *subElement){
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
	}

	void RemoveChild(UIDrawable *subElement){
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
	}

	void Hide(){
		visible = 0;
	}

	void Focus(){
	    if (!focus){
            GetTopElement()->UnfocusAllChildren();
            focus = 1;
            EmitEvent("GotFocus");
	    }
	}

	void UnFocus(){
		if (focus){
            focus = 0;
            EmitEvent("LostFocus");
		}
	}


	UIDrawable* GetNextTabStop(UIDrawable *currentStop, int *takeNext){
		if (tabstop && (currentStop == NULL || (currentStop != this && *takeNext == 1))){
			*takeNext = 1;
			return this;
		}
		if (childCount == 0){
			return NULL;
		}

		UIDrawable *orderedChildren[255];
		int i, j;
		for (i = 0; i < childCount; i++){
			orderedChildren[i] = children[i];
		}
		//sort by tab stop
		for (i = 0; i < childCount; i++){
			for (j = i+1; j < childCount; j++){
				if (orderedChildren[j]->tabstop > orderedChildren[i]->tabstop){
					UIDrawable *tmp = orderedChildren[i];
					orderedChildren[i] = orderedChildren[j];
					orderedChildren[j] = tmp;
				}
			}
		}
		//TODO: handle switching to next stop when element is hidden
		UIDrawable *nextStop;
		for (i = 0; i < childCount; i++){
			if (currentStop && orderedChildren[i] == currentStop){
				*takeNext = 1;
			}
			if (!orderedChildren[i]->visible){ continue; }
			nextStop = orderedChildren[i]->GetNextTabStop(currentStop, takeNext);
			if (nextStop && *takeNext){
				//something was found in the descendents, return it
				return nextStop;
			}
		}
		return NULL;
	}

	void PropagateMouseEvent(int subX, int subY, const char *event){
		int i, o;
		for (o = childDisplayOrderCount-1; o >= 0; o--){
            i = childDisplayOrder[o];
			if (!children[i]->visible){ continue; }
			if (subX > children[i]->x && subX < (children[i]->x+children[i]->width) && subY > children[i]->y && subY < children[i]->y+children[i]->height){
				children[i]->PropagateMouseEvent(subX-children[i]->x, subY-children[i]->y, event);
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
			if (subX > children[i]->x && subX < (children[i]->x+children[i]->width) && subY > children[i]->y && subY < children[i]->y+children[i]->height){
				children[i]->IdentifyVisibleElementsAtPosition(subX-children[i]->x, subY-children[i]->y, elements, elementCount);
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

	//freeze rendering, will blit currently stored context without internal update
	void Freeze(){
		freeze = 1;
	}

	void Unfreeze(){
		freeze = 0;
	}

	void SetDimensions(int drawWidth, int drawHeight){
        GrDestroyContext(ctx);
        width = drawWidth;
        height = drawHeight;
        ctx = GrCreateContext(drawWidth, drawHeight, NULL, NULL);
        GrClearContextC(ctx,GrAllocColor(0,0,0));
	}

	UIDrawable(int drawWidth, int drawHeight){
		x = 0;
		y = 0;
		width = drawWidth;
		height = drawHeight;
		visible = 1;
		freeze = 0;
		focus = 0;
		tabstop = 0;
		childCount = 0;
		childDisplayOrderCount = 0;
		ctx = GrCreateContext(width, height, NULL, NULL);
		GrClearContextC(ctx,GrAllocColor(0,0,0));
	}

	~UIDrawable(){
		if (ctx){
			GrDestroyContext(ctx);
		}
	}

	void Draw(GrContext *ontoContext){
		if (!freeze){
			draw_internal();
			int i, o;
			for (o = 0; o < childDisplayOrderCount; o++){
                i = childDisplayOrder[o];
				if (!children[i]->visible){ continue; }
				children[i]->Draw(ctx);
			}
		}
		if (ontoContext == NULL){ return; }
		GrBitBlt(ontoContext,x,y,ctx,0,0,width-1,height-1,GrIMAGE);
		if (focus){
			GrSetContext(ontoContext);
			GrBox(0, 0, width-1, height-1, GrWhite());
		}
	}
};

//Window is a container for tab stops, and receives keyboard events when in focus
class UIWindow : public UIDrawable {
private:

    UIDrawable *focusedElement = NULL;
	
	UIDrawable* findTabContainer(UIDrawable *child){
		if (child->parent == NULL){
			return NULL; //not attached to tree.
		}

		if (child->parent && child->parent->containertabstop > 0){
			//found container;
			return child->parent;
		}

		if (child->parent && child->parent == this){
			//reached top of tree, return the window
			return child->parent;
		}

		if (child == this){
			//in case the window is passed to method.
			return child;
		}

		return findTabContainer(child->parent); //traverse one level up and try again.
	}

	UIDrawable* findNextTabElementWithinTabContainer(UIDrawable *parent, int prevTabStop, UIDrawable* lowestNextTabStopSoFarElem, int* lowestNextTabStopSoFar){
		for (int i = 0; i < parent->childDisplayOrderCount; i++){
			UIDrawable* thisElem = parent->children[parent->childDisplayOrder[i]];
			if (thisElem->tabstop > prevTabStop){
				//candidate for next tab element
				if (lowestNextTabStopSoFarElem && thisElem->tabstop < *lowestNextTabStopSoFar){
					// found a lower tab stop than the previous found tab stop
					lowestNextTabStopSoFarElem = thisElem;
					*lowestNextTabStopSoFar = thisElem->tabstop;
				} else if (!lowestNextTabStopSoFarElem){
					//no lowest stored, store this one as the first.
					lowestNextTabStopSoFarElem = thisElem;
					*lowestNextTabStopSoFar = thisElem->tabstop;
				}
			}
			if (thisElem->childCount > 0){
				//try to find the next one within the sub-tree (the elements may not be direct children of the tab container)
				UIDrawable* foundNextElement = findNextTabElementWithinTabContainer(thisElem, prevTabStop, lowestNextTabStopSoFarElem, lowestNextTabStopSoFar);
				if (foundNextElement && foundNextElement->tabstop < *lowestNextTabStopSoFar){
					//the one found within the tree is lower than the lowest discovered from previous elements in this tree, record it
					lowestNextTabStopSoFarElem = foundNextElement;
					*lowestNextTabStopSoFar = foundNextElement->tabstop;
				}
			}
		}
		if (lowestNextTabStopSoFarElem){
			return lowestNextTabStopSoFarElem;
		}

		return NULL;
	}

	UIDrawable* findNextTabContainer(UIDrawable *parent, int prevContainerTabStop, UIDrawable* firstContainerTabStop, UIDrawable* lowestNextTabStopSoFarElem, int* lowestNextTabStopSoFar){
		for (int i = 0; i < parent->childDisplayOrderCount; i++){
			UIDrawable* thisElem = parent->children[parent->childDisplayOrder[i]];
			if (thisElem->containertabstop > 0 && firstContainerTabStop == NULL){
				firstContainerTabStop = thisElem;
			}
			if (thisElem->containertabstop > prevContainerTabStop){
				//candidate for next tab element
				if (lowestNextTabStopSoFarElem && thisElem->containertabstop < *lowestNextTabStopSoFar){
					// found a lower tab stop than the previous found tab stop
					lowestNextTabStopSoFarElem = thisElem;
					*lowestNextTabStopSoFar = thisElem->containertabstop;
				} else if (!lowestNextTabStopSoFarElem){
					//no lowest stored, store this one as the first.
					lowestNextTabStopSoFarElem = thisElem;
					*lowestNextTabStopSoFar = thisElem->containertabstop;
				}
			}
			if (thisElem->childCount > 0){
				//try to find the next one within the sub-tree (the elements may not be direct children of the tab container)
				UIDrawable* foundNextElement = findNextTabContainer(thisElem, prevContainerTabStop, firstContainerTabStop, lowestNextTabStopSoFarElem, lowestNextTabStopSoFar);
				if (foundNextElement && foundNextElement->containertabstop < *lowestNextTabStopSoFar){
					//the one found within the tree is lower than the lowest discovered from previous elements in this tree, record it
					lowestNextTabStopSoFarElem = foundNextElement;
					*lowestNextTabStopSoFar = foundNextElement->containertabstop;
				}
			}
		}
		if (lowestNextTabStopSoFarElem){
			return lowestNextTabStopSoFarElem;
		}

		return NULL;
	}

	void FocusNextTabStop(){
		UIDrawable* tabContainer = this;
		int lastStop = 0;
		if (focusedElement){
			tabContainer = findTabContainer(focusedElement);
			lastStop = focusedElement->tabstop;
			if (!tabContainer){ // not attached
				tabContainer = this;
				lastStop = 0;
			}
		}
		int parentStop = tabContainer->containertabstop;
		int lowestTabStop = 255;
		UIDrawable* nextTabWithinParent = findNextTabElementWithinTabContainer(tabContainer, lastStop, NULL, &lowestTabStop);
		if (nextTabWithinParent){
			//found one, focus it
			nextTabWithinParent->Focus();
			focusedElement = nextTabWithinParent;
		} else {
			//didn't find one within the container, need to find the next container, or wrap around to the beginning.
			UIDrawable* firstContainerStop = NULL;
			UIDrawable* nextTabContainer = NULL;
			int lowestSoFar = 255;
			nextTabContainer = findNextTabContainer(this, parentStop, firstContainerStop, NULL, &lowestSoFar);
			UIDrawable* focusFirst;
			lowestSoFar = 255;
			UIDrawable* searchContainer;
			if (nextTabContainer){
				//found one, focus first element in this container
				searchContainer = nextTabContainer;
			} else if (firstContainerStop){
				//wrap around to first one with a stop > 0 within window
				searchContainer = firstContainerStop;
			} else {
				searchContainer = this;
			}
			focusFirst = GetFirstStopInParent(searchContainer, NULL, &lowestSoFar);
			if (focusFirst){
				focusFirst->Focus();
				focusedElement = focusFirst;
			}
		}
	}

	UIDrawable* GetFirstStopInParent(UIDrawable* parent, UIDrawable* lowestTabStopSoFarElem, int* lowestTabStopSoFar){
		for (int i = 0; i < parent->childCount; i++){
			UIDrawable* thisItem = parent->children[parent->childDisplayOrder[i]];
			if (thisItem->tabstop > 0){
				if (lowestTabStopSoFarElem == NULL || thisItem->tabstop < *lowestTabStopSoFar){
					lowestTabStopSoFarElem = thisItem;
					*lowestTabStopSoFar = thisItem->tabstop;
				}
			};
			if (thisItem->childCount > 0){
				GetFirstStopInParent(thisItem, lowestTabStopSoFarElem, lowestTabStopSoFar);
			}
		}
		if (lowestTabStopSoFarElem == NULL){
			return NULL;
		} else {
			return lowestTabStopSoFarElem;
		}
	}

	void _set_next_focused(){
		FocusNextTabStop();
	}

	void draw_internal(){
		GrClearContextC(ctx,GrAllocColor(10,10,10));
	}

public:
    int closed = 0;
    int destroy = 0;
    //classes inheriting from UIWindow should make sure to call this method as well if they override
    void OnKeyUp(int ScanCode, int ShiftState, int Ascii){
        if (ScanCode == KEY_TAB){
            //set focused element
            _set_next_focused();
        } else if (focusedElement){
            //send any other key press to the focused element
            focusedElement->OnKeyUp(ScanCode, ShiftState, Ascii);
        }
    }

    void Close(){
        closed = 1;
    }
    void CloseAndDestroy(){
        closed = 1;
        destroy = 1;
    }
    void Open(){
        closed = 0;
        destroy = 0;
    }

    UIWindow(int drawWidth,int drawHeight) : UIDrawable(drawWidth, drawHeight) {
    }
};


class UIAppScreen : public UIDrawable {
private:
	void draw_internal(){
		GrClearContextC(ctx,GrAllocColor(10,10,10));
	}

	void Vsync()
	{
	  /* wait until any previous retrace has ended */
	  do {
	  } while (inportb(0x3DA) & 8);

	  /* wait until a new retrace has just begun */
	  do {
	  } while (!(inportb(0x3DA) & 8));
	}

	int _mouse_enabled = 0;

    void updateTree(UIDrawable *parentNode){
        int i,o;
        for (o = 0; o < parentNode->childDisplayOrderCount; o++){
            i = parentNode->childDisplayOrder[o];
            parentNode->children[i]->Update();
            updateTree(parentNode->children[i]);
        }
    }

protected:

public:
    void Update(){
        updateTree((UIDrawable*) this);
    }
	//top of the render tree
	void render(){
		Draw(NULL);
		Vsync();
		GrBitBlt(GrScreenContext(),x,y,ctx,0,0,width-1,height-1,GrIMAGE);
	}

	void SetMouseEnabled(){
		_mouse_enabled = 1;
	}

	UIAppScreen (int screenWidth, int screenHeight) : UIDrawable(screenWidth, screenHeight){

	}
};


class UIPointer : public UIDrawable {

private:

	GrPattern *cursor;
	GrColor cols[4];
	char ptr12x16bits[192] = {
		0,1,0,0,0,0,0,0,0,0,0,0,
		1,2,1,0,0,0,0,0,0,0,0,0,
		1,2,2,1,0,0,0,0,0,0,0,0,
		1,2,2,2,1,0,0,0,0,0,0,0,
		1,2,2,2,2,1,0,0,0,0,0,0,
		1,2,2,2,2,2,1,0,0,0,0,0,
		1,2,2,2,2,2,2,1,0,0,0,0,
		1,2,2,2,2,2,2,2,1,0,0,0,
		1,2,2,2,2,2,2,2,2,1,0,0,
		1,2,2,2,2,2,2,2,2,2,1,0,
		1,2,2,2,2,2,2,2,2,2,2,1,
		1,2,2,2,2,1,1,1,1,1,1,0,
		1,2,2,2,1,0,0,0,0,0,0,0,
		1,2,2,1,0,0,0,0,0,0,0,0,
		1,2,1,0,0,0,0,0,0,0,0,0,
		0,1,0,0,0,0,0,0,0,0,0,0,
	};

	int initialized = 0;
	void init(){
		cols[0] = 3;
		cols[1] = GrAllocColor(0,0,0);
		cols[2] = GrAllocColor(10,10,10);
		cols[3] = GrAllocColor(255,255,255);

		cursor = GrBuildPixmap(ptr12x16bits, 12, 16, cols);
		initialized = 1;
	}
	void draw_internal(){
		if (!initialized){ init(); }
		GrSetContext(ctx);
		GrPatternFilledBox(0, 0, width-1, height-1, cursor);
	}

public:

	UIPointer() : UIDrawable(12,16){

	}

	~UIPointer(){
		if (cursor == NULL){
			return;
		}
		GrImageDestroy(cursor);
	}

};

class UIPanel : public UIDrawable {
private:
	GrColor backgroundColor;
	void draw_internal(){
		GrClearContextC(ctx, backgroundColor);
	}
public:

	UIPanel(GrColor bgColor, int drawWidth, int drawHeight) : UIDrawable(drawWidth, drawHeight) {
		backgroundColor = bgColor;
	}
};

class UITextArea : public UIDrawable {
private:
	std::string text = std::string();
	GrTextOption textOptions;
	GrColor backgroundColor;
	std::vector<int> lineWidths = std::vector<int>(1);
	std::vector<std::string> lines = std::vector<std::string>(1);
	int _charHeight = 0;

	char _horizAlign;
	char _vertAlign;
	bool drawCursor = false;
	int cursorPosition = 0;
	int cursorLine = 0;
	int cursorColumn = 0;

	void _parse_text(){
		lineWidths.clear();
		lines.clear();
		if (text.size() == 0){
			return;
		}
		//does not parse at word level
		int lineWidth, charWidth, i;
		_charHeight = GrFontCharHeight(textOptions.txo_font,(int) text[0]);
		lineWidth = 0;
		int curLine = 0;
		lines.push_back(std::string(""));
		for (i = 0; i < text.size(); i++){
			if (text.at(i) == '\n'){
				lineWidths.push_back(lineWidth);
				lines.push_back(std::string(""));
				curLine++;
				lineWidth = 0;
				if (cursorPosition - 1 == i){
					cursorLine = curLine;
					cursorColumn = 0;
				}
				continue;
			}
			if (cursorPosition - 1 == i){
				cursorLine = curLine;
				cursorColumn = lines[curLine].size();
			}
			charWidth = GrFontCharWidth(textOptions.txo_font, (int) text.at(i));
			if (lineWidth + charWidth > width){
				lineWidths.push_back(lineWidth);
				lines.push_back(std::string(""));
				curLine++;
				lineWidth = 0;
			}
			lines[curLine].append(text,i,1);
			lineWidth+= charWidth;
		}
		lineWidths.push_back(lineWidth); //last one
	}

	void draw_internal(){
		GrContext *prevCtx = GrCurrentContext();
		GrSetContext(ctx);
		GrClearContextC(ctx, GrAllocColor(0,0,0));
		GrClearContextC(ctx, backgroundColor);

		int numLines = lines.size();
		int i, x, y, firstLine;
		x = 0;
		y = 0;
		firstLine = 0;
		//tbd line spacing
		if (_vertAlign == GR_ALIGN_CENTER) {
			int blockHeight = numLines * _charHeight;
			y = (height/2) - (blockHeight / 2);
			if (y < 0){
				//some clipping will occur, find first visible line, TODO: Better solution for this scenario?
				firstLine = (0-y) / _charHeight;
			}
		}
		for (i = firstLine; i < numLines; i++){
			if (y > height){ break; }
			x = 0;
			if (_horizAlign == GR_ALIGN_CENTER){
				x = (width / 2) - (lineWidths[i] / 2);
			} else if (_horizAlign == GR_ALIGN_RIGHT){
				x = width - (lineWidths[i]);
			}
			GrDrawString((void *) lines[i].c_str(), lines[i].length(), x, y, &textOptions);
			if (drawCursor && cursorLine == i){
				int cursorX = 0;
				if (lines[i].size() > 0){
					int charWidth = lineWidths[i] / lines[i].size();
					cursorX = cursorColumn * charWidth + charWidth;
				}
				
				GrLine(cursorX, y, cursorX, y + _charHeight, textOptions.txo_fgcolor.v);
			}
			y+=_charHeight;
		}

	}

protected:

public:

	void SetText(char *txt){
		SetText(std::string(txt));
	}

	void SetText(std::string txt){
		cursorPosition = txt.size();
		text = txt;
		_parse_text();
	}

	std::string GetText(){
		return text;
	}

	void CharAdd(char character){
		text.insert(cursorPosition, 1, character);
		cursorPosition++;
		_parse_text();
	}

	void CharBackspace(){
		if (cursorPosition == 0){
			return;
		}
		text.erase(cursorPosition - 1, 1);
		cursorPosition--;
		_parse_text();
	}

	void CharDelete(){
		if (cursorPosition >= text.size() - 1){
			return;
		}
		text.erase(cursorPosition, 1);
		_parse_text();
	}

	void CursorLeft(){
		if (cursorPosition > 0){
			cursorPosition--;
			_parse_text();
		}
	}

	void CursorRight(){
		if (cursorPosition < text.size()){
			cursorPosition++;
			_parse_text();
		}
	}

	void CursorUp(){
		if (cursorLine > 0){
			cursorLine--;
			if (lines[cursorLine].size() -1 < cursorColumn){
				cursorColumn = lines[cursorLine].size() - 1;
			}
		}
	}

	void CursorDown(){
		if (cursorLine < lines.size() - 1){
			cursorLine++;
			if (lines[cursorLine].size() - 1 < cursorColumn){
				cursorColumn = lines[cursorLine].size() - 1;
			}
		}
	}

	void SetFont(GrFont *font){
		textOptions.txo_font = font;
		_parse_text(); //change in font affects line breaks
	}

	void SetColor(GrColor foreColor, GrColor backColor){
		textOptions.txo_fgcolor.v = foreColor;
		textOptions.txo_bgcolor.v = backColor;
		backgroundColor = backColor;
	}

	void SetAlign(char horizAlign, char vertAlign){
		_horizAlign = horizAlign;
		_vertAlign = vertAlign;
	}

	void ShowCursor(){
		drawCursor = true;
	}

	void HideCursor(){
		drawCursor = false;
	}

	UITextArea(int drawWidth, int drawHeight) : UIDrawable(drawWidth, drawHeight) {
		text = "";
		backgroundColor = GrNOCOLOR;
		textOptions.txo_font = &GrFont_PC6x8;
		textOptions.txo_fgcolor.v = GrWhite();
		textOptions.txo_bgcolor.v = GrNOCOLOR;
		textOptions.txo_direct = GR_TEXT_RIGHT;
		textOptions.txo_xalign = GR_ALIGN_LEFT;
		textOptions.txo_yalign = GR_ALIGN_TOP;
		textOptions.txo_chrtype = GR_BYTE_TEXT;
		_horizAlign = GR_ALIGN_LEFT;
		_vertAlign = GR_ALIGN_TOP;
	}
	~UITextArea(){
	    lines.clear();
	    lineWidths.clear();
	}

};

class UIScrollingText : public UIDrawable {
private:
	UITextArea* _innerText;
	void draw_internal(){
		GrClearContextC(ctx, GrAllocColor(0,0,0));
		_innerText->y = 0-ScrollTop;
	}
	
	int ScrollTop = 0;
public:

	bool ScrollDown(){
		int newScroll = ScrollTop+1;
		if (newScroll > _innerText->height - height){
			return false;
		}
		ScrollTop++;
		return true;
	}

	bool ScrollUp(){
		int newScroll = ScrollTop - 1;
		if (newScroll < 0){
			return false;
		}
		ScrollTop--;
		return true;
	}

	UIScrollingText(UITextArea* innerText, int height) : UIDrawable(innerText->width, height){
		_innerText = innerText;
		AddChild(_innerText);
	}
};

class UIMsgBox : public UIWindow {
private:
    UITextArea *textArea = NULL;
public:
    static UIMsgBox* Create(char *text, int drawWidth, int drawHeight);
    void OnKeyUp(int ScanCode, int ShiftState, int Ascii){
        UIWindow::OnKeyUp(ScanCode, ShiftState, Ascii);
        if (ScanCode == KEY_ENTER){
            CloseAndDestroy();
        }
    }
    UIMsgBox(char *text, int drawWidth, int drawHeight) : UIWindow(drawWidth, drawHeight){
        textArea = new UITextArea(drawWidth, drawHeight);
        textArea->SetText(text);
        textArea->SetAlign(GR_ALIGN_CENTER,GR_ALIGN_CENTER);
        AddChild((UIDrawable*) textArea);
    }
    ~UIMsgBox(){
        delete textArea;
    }
};

class UIWindowController;
UIWindowController *_windowController = NULL;

class UIWindowController {
private:

protected:
	UIWindow *windows[255] = {NULL};
    int windowCount = 0;
	UIWindow *focusedWindow = NULL;

public:

    static UIWindowController *Get();

    UIAppScreen *screen = NULL;

    void Update(){
        int i;
        //check for window state changes
        for (i = windowCount-1; i >= 0; i--){
            if (windows[i]->closed && focusedWindow == windows[i]){
                //window was closed
                focusedWindow->Hide();
                if (windows[i]->destroy){
                    RemoveWindow(focusedWindow);
                    screen->RemoveChild(focusedWindow);
                    delete focusedWindow;
                    focusedWindow = NULL;
                }
                //focus on the first window (main)
                SetFocusedWindow(windows[0]);
            } else if (!(windows[i]->closed) && focusedWindow != windows[i]){
                //window was just opened
                focusedWindow->Show();
            }
        }
        //call update on the UIDrawable tree
        screen->Update();
    }

	UIAppScreen *GetScreen(){
		return screen;
	}

	void AddWindow(UIWindow *window){
	    windows[windowCount] = window;
        windowCount++;
        screen->AddChild((UIDrawable*) window);
        if (!focusedWindow){
            focusedWindow = window;
        }
	}
	void AddWindow(UIWindow *window, int focused){
        AddWindow(window);
        if (focused){
            SetFocusedWindow(window);
        }
	}
	void RemoveWindow(UIWindow *window){
        int i, z, removed;
        removed = 0;
        for (i = windowCount-1; i >= 0; i--){
            if (windows[i] == window){
                for (z = i+1; z < windowCount; z++){
                    windows[z-1] = windows[z];
                    windows[z] = NULL;
                }
                windowCount--;
            }
        }
	}

	UIWindow *GetFocusedWindow(){
        return focusedWindow;
	}

	void SetFocusedWindow(UIWindow *window){
        focusedWindow = window;
        focusedWindow->BringToFront();
	}

	UIWindowController(int screenWidth, int screenHeight, int bitDepth){

		//screen
		screen = new UIAppScreen(screenWidth, screenHeight);
		_windowController = this;
	}
	~UIWindowController(){
        delete screen;
	}
};

UIWindowController *UIWindowController::Get(){
    return _windowController;
}


UIMsgBox* UIMsgBox::Create(char *text, int drawWidth, int drawHeight){
    UIMsgBox *msgBox = new UIMsgBox(text, drawWidth, drawHeight);
    msgBox->x = (UIWindowController::Get()->GetScreen()->width / 2) - (drawWidth/2);
    msgBox->y = (UIWindowController::Get()->GetScreen()->height / 2) - (drawHeight/2);
    UIWindowController::Get()->AddWindow((UIWindow*) msgBox, 1);
    return msgBox;
}

class UITextWindow : public UIWindow {
private:
    UITextArea *textArea = NULL;
public:

	void SetText(char* text){
		textArea->SetText(text);
	}

    UITextWindow(char *text, int drawWidth, int drawHeight) : UIWindow(drawWidth, drawHeight){
        textArea = new UITextArea(drawWidth, drawHeight);
        textArea->SetText(text);
        textArea->SetAlign(GR_ALIGN_CENTER,GR_ALIGN_CENTER);
        AddChild((UIDrawable*) textArea);
    }

	UITextWindow(char *text) : UIWindow(UIWindowController::Get()->GetScreen()->width, UIWindowController::Get()->GetScreen()->height) {
		UITextWindow(text, UIWindowController::Get()->GetScreen()->width, UIWindowController::Get()->GetScreen()->height);
	}

    ~UITextWindow(){
        delete textArea;
    }
};

#endif
