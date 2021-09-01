#ifndef UIPointer_cpp
#define UIPointer_cpp

#include <grx20.h>
#include "AppUI.h"
#include "UIDrawable.cpp"

class UIPointer : public UIDrawable {

private:

	GrPattern *cursor = NULL;
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
		cols[1] = THEME_COLOR_TRANSPARENT;
		cols[2] = THEME_COLOR_BLACK;
		cols[3] = GrWhite();

		cursor = GrBuildPixmap(ptr12x16bits, 12, 16, cols);
		initialized = 1;
	}
	void draw_internal(){
		if (!initialized){ init(); }
		GrSetContext(ctx);
		GrPatternFilledBox(0, 0, width-1, height-1, cursor);
	}

public:

	void SetPosition(int posX, int posY){
		x = posX;
		y = posY;
		needsRedraw = true;
	}

	UIPointer() : UIDrawable(12,16){
	}

	~UIPointer(){
		if (cursor == NULL){
			return;
		}
		GrImageDestroy(cursor);
	}

};

#endif