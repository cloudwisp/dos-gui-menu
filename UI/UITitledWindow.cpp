#ifndef UITitledWindow_cpp
#define UITitledWindow_cpp

#include <grx20.h>
#include <string>
#include <cstring>
#include "../AppEvent.h"
#include "AppUITheme.h"
#include "UIWindow.cpp"
#include "UIHelpers.cpp"

class UITitledWindow : public UIWindow {
private:

	std::string _title;

	GrTextOption titleTextOptions;

	GrFont* titleFont;

	void draw_internal() override{
		GrClearContextC(ctx, THEME_WINDOW_BACKGROUND_COLOR);
		GrSetContext(ctx);
		if (THEME_WINDOW_BORDER_WIDTH > 0){
			GrBox(0,0, width-1, height-1, THEME_WINDOW_BORDER_COLOR);
		}
		
		GrFilledBox(THEME_WINDOW_BORDER_WIDTH, THEME_WINDOW_BORDER_WIDTH, innerWidth, THEME_WINDOW_TITLE_HEIGHT, THEME_WINDOW_TITLE_BACKGROUND_COLOR);
		int titleTextSize = GrFontStringWidth(titleFont, _title.c_str(), _title.size(), GR_BYTE_TEXT);
		int fontHeight = GrFontCharHeight(titleFont, "A");
		int centeredX = (innerWidth / 2);// - (titleTextSize / 2);
		int centeredY = (THEME_WINDOW_TITLE_HEIGHT / 2) - (fontHeight / 2);
		GrDrawString((void*)_title.c_str(), _title.size(), THEME_WINDOW_BORDER_WIDTH + centeredX, THEME_WINDOW_BORDER_WIDTH + centeredY, &titleTextOptions);
	}

protected:

public:

	void SetTitle(std::string title){
		_title = title;
		needsRedraw = true;
	}
	
	UITitledWindow(int drawWidth, int drawHeight, std::string title) : UIWindow(drawWidth, drawHeight, drawWidth - (THEME_WINDOW_BORDER_WIDTH * 2), drawHeight - THEME_WINDOW_TITLE_HEIGHT - (THEME_WINDOW_BORDER_WIDTH * 2), false){
		_title = title;
		innerWidth = drawWidth - (THEME_WINDOW_BORDER_WIDTH * 2);
		innerHeight = drawHeight - THEME_WINDOW_TITLE_HEIGHT - (THEME_WINDOW_BORDER_WIDTH * 2);
		innerContextY = THEME_WINDOW_TITLE_HEIGHT + THEME_WINDOW_BORDER_WIDTH;
		innerContextX = THEME_WINDOW_BORDER_WIDTH;

		titleFont = UIHelpers::ResolveFont(THEME_WINDOW_TITLE_FONT);

		titleTextOptions.txo_font = titleFont;
		titleTextOptions.txo_fgcolor.v = THEME_WINDOW_TITLE_TEXT_COLOR;
		titleTextOptions.txo_bgcolor.v = GrNOCOLOR;
		titleTextOptions.txo_direct = GR_TEXT_RIGHT;
		titleTextOptions.txo_xalign = GR_ALIGN_CENTER;
		titleTextOptions.txo_yalign = GR_ALIGN_CENTER;
		titleTextOptions.txo_chrtype = GR_BYTE_TEXT;

	}
};
 

#endif