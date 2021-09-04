#ifndef UIAppScreen_cpp
#define UIAppScreen_cpp

#include <grx20.h>
#include <pc.h>
#include "AppUI.h"
#include "UIDrawable.cpp"


UIAppScreen* currentScreen;
class UIAppScreen : public UIDrawable {
private:
	void draw_internal(){
		//GrClearContextC(ctx,THEME_COLOR_BLACK);
	}

	static UIAppScreen* current;

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
		ClearRedrawBoxes();
		DrawNew(false);
		BlitBoxes(ctx);
		//temporarily highlight blit boxes
		// for (int i = 0; i < redrawBoxes.size(); i++){
		// 	BoxCoords coords = redrawBoxes.at(i);
		// 	GrSetContext(GrScreenContext());
		// 	GrBox(coords.x1, coords.y1, coords.x2, coords.y2, THEME_HIGHLIGHT_BORDER);
		// }
		Vsync();
		GrBitBlt(GrScreenContext(),x,y,ctx,0,0,width-1,height-1,GrIMAGE);
	}

	void SetMouseEnabled(){
		_mouse_enabled = 1;
	}

	UIAppScreen (int screenWidth, int screenHeight) : UIDrawable(screenWidth, screenHeight){
		currentScreen = this;
	}

	static UIAppScreen* Get();
};

UIAppScreen* UIAppScreen::Get(){
	return currentScreen;
}

#endif