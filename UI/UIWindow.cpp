#ifndef UIWindow_cpp
#define UIWindow_cpp

#include <string>
#include <grx20.h>
#include "../AppEvent.cpp"
#include "../keyboard.h"
#include "AppUI.h"
#include "AppUITheme.h"
#include "UIDrawable.cpp"

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

	UIDrawable* findNextTabElementWithinTabContainer(UIDrawable *parent, int prevTabStop, UIDrawable** lowestNextTabStopSoFarElem, int* lowestNextTabStopSoFar){
		for (int i = 0; i < parent->childDisplayOrderCount; i++){
			UIDrawable* thisElem = parent->children[parent->childDisplayOrder[i]];
			if (thisElem->tabstop > prevTabStop){
				//candidate for next tab element
				if (!*lowestNextTabStopSoFarElem || thisElem->tabstop < *lowestNextTabStopSoFar){
					*lowestNextTabStopSoFarElem = thisElem;
					*lowestNextTabStopSoFar = thisElem->tabstop;
				}
			}
			if (thisElem->childCount > 0){
				//try to find the next one within the sub-tree (the elements may not be direct children of the tab container)
				UIDrawable* foundNextElement = findNextTabElementWithinTabContainer(thisElem, prevTabStop, lowestNextTabStopSoFarElem, lowestNextTabStopSoFar);
				if (foundNextElement && foundNextElement->tabstop < *lowestNextTabStopSoFar){
					//the one found within the tree is lower than the lowest discovered from previous elements in this tree, record it
					*lowestNextTabStopSoFarElem = foundNextElement;
					*lowestNextTabStopSoFar = foundNextElement->tabstop;
				}
			}
		}
		
		return *lowestNextTabStopSoFarElem;
	}

	UIDrawable* findNextTabContainer(UIDrawable *parent, int prevContainerTabStop, UIDrawable** lowestNextTabStopSoFarElem, int* lowestNextTabStopSoFar){
		for (int i = 0; i < parent->childDisplayOrderCount; i++){
			UIDrawable* thisElem = parent->children[parent->childDisplayOrder[i]];
			if (thisElem->containertabstop > prevContainerTabStop){
				//candidate for next tab element
				if (!lowestNextTabStopSoFarElem || thisElem->containertabstop < *lowestNextTabStopSoFar){
					*lowestNextTabStopSoFarElem = thisElem;
					*lowestNextTabStopSoFar = thisElem->containertabstop;
				}
			}
			if (thisElem->childCount > 0){
				//try to find the next one within the sub-tree (the elements may not be direct children of the tab container)
				UIDrawable* foundNextElement = findNextTabContainer(thisElem, prevContainerTabStop, lowestNextTabStopSoFarElem, lowestNextTabStopSoFar);
				if (foundNextElement && foundNextElement->containertabstop < *lowestNextTabStopSoFar){
					//the one found within the tree is lower than the lowest discovered from previous elements in this tree, record it
					*lowestNextTabStopSoFarElem = foundNextElement;
					*lowestNextTabStopSoFar = foundNextElement->containertabstop;
				}
			}
		}

		return *lowestNextTabStopSoFarElem;
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
		UIDrawable* lowestTabStopElm = NULL;
		UIDrawable* nextTabWithinParent = findNextTabElementWithinTabContainer(tabContainer, lastStop, &lowestTabStopElm, &lowestTabStop);
		if (nextTabWithinParent){
			//found one, focus it
			nextTabWithinParent->Focus();
		} else {
			//didn't find one within the container, need to find the next container, or wrap around to the beginning.
			UIDrawable* nextTabContainer = NULL;
			int lowestSoFar = 255;
			UIDrawable *lowestTabStopElm2 = NULL;
			nextTabContainer = findNextTabContainer(this, parentStop, &lowestTabStopElm2, &lowestSoFar);
			UIDrawable* focusFirst;
			lowestSoFar = 255;
			UIDrawable* searchContainer;
			if (nextTabContainer){
				//found one, focus first element in this container
				searchContainer = nextTabContainer;
			} else {
				UIDrawable *lowestTabStopElm3 = NULL;
				searchContainer = GetFirstContainerStop(this, &lowestTabStopElm3, &lowestSoFar);
				if (!searchContainer){
					searchContainer = this; //fallback to window, if there is no tab stop container.
				}
			}
			UIDrawable *lowestTabStopElm4;
			lowestSoFar = 255;
			focusFirst = GetFirstStopInParent(searchContainer, &lowestTabStopElm4, &lowestSoFar);
			if (focusFirst){
				focusFirst->Focus();
			}
		}
	}

	UIDrawable* GetFirstContainerStop(UIDrawable *parent, UIDrawable** lowestTabStopSoFarElem, int* lowestTabStopSoFar){
		for (int i = 0; i < parent->childCount; i++){
			UIDrawable* thisItem = parent->children[i];
			if (thisItem->containertabstop > 0 && thisItem->containertabstop < *lowestTabStopSoFar){
				*lowestTabStopSoFarElem = thisItem;
				*lowestTabStopSoFar = thisItem->containertabstop;
			};
			if (thisItem->childCount > 0){
				UIDrawable* foundChild = GetFirstContainerStop(thisItem, lowestTabStopSoFarElem, lowestTabStopSoFar);
				if (foundChild){
					*lowestTabStopSoFarElem = foundChild;
				}
			}
		}

		return *lowestTabStopSoFarElem;
	}

	UIDrawable* GetFirstStopInParent(UIDrawable* parent, UIDrawable** lowestTabStopSoFarElem, int* lowestTabStopSoFar){
		for (int i = 0; i < parent->childCount; i++){
			UIDrawable* thisItem = parent->children[i];
			if (thisItem->tabstop > 0 && thisItem->tabstop < *lowestTabStopSoFar){
				*lowestTabStopSoFarElem = thisItem;
				*lowestTabStopSoFar = thisItem->tabstop;
			};
			if (thisItem->childCount > 0){
				UIDrawable* foundChild = GetFirstStopInParent(thisItem, lowestTabStopSoFarElem, lowestTabStopSoFar);
				if (foundChild){
					*lowestTabStopSoFarElem = foundChild;
				}
			}
		}
		return *lowestTabStopSoFarElem;
	}

	void _set_next_focused(){
		FocusNextTabStop();
	}

	void draw_internal(){
		GrClearContextC(ctx,THEME_WINDOW_BACKGROUND_COLOR);
	}

	void FocusNotify(UIDrawable* focusedDrawable){
		focusedElement = focusedDrawable;
	}

public:
    int closed = 0;
    int destroy = 0;

	//classes inheriting from UIWindow should make sure to call this method as well if they override
	void CheckInputs(){
		if (focusedElement){
			focusedElement->CheckInputs();
		}
	}

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

	void OnEvent(EventEmitter* source, std::string event, EventData data){
		if (event == "GotFocus"){
			//the cast here is a little hacky, but we know we've registered GotFocus for UIDrawables only;
			focusedElement = (UIDrawable*) source;
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
		EmitEvent("RequestOpen");
    }

	void DescendentAdded(UIDrawable* descendent){
		descendent->BindEvent("GotFocus", this);
	}

	void DescendentRemoved(UIDrawable* descendent){
		descendent->UnbindAllEventsForConsumer(this);
	}

    UIWindow(int drawWidth,int drawHeight) : UIWindow(drawWidth, drawHeight, drawWidth, drawHeight, true) {
    }

	UIWindow(int drawWidth, int drawHeight, int innerWidth, int innerHeight, bool asSingleContext) : UIDrawable(drawWidth, drawHeight, innerWidth, innerHeight, 0, 0, asSingleContext){
		window = this;
	}
};

#endif