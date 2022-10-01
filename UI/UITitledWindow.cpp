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

	GrFont* titleFont = NULL;

	BoxCoords closeButton;
	BoxCoords windowBox;

	void draw_internal() override{
		GrClearContextC(ctx, THEME_WINDOW_BACKGROUND_COLOR);
		GrSetContext(ctx);
		if (THEME_WINDOW_BORDER_WIDTH > 0){
			Draw3dButton(ctx, BoxCoords {0,0,width-1,height-1},THEME_WINDOW_BACKGROUND_COLOR,false);
		}
		
		GrFilledBox(THEME_WINDOW_BORDER_WIDTH, THEME_WINDOW_BORDER_WIDTH, innerWidth, THEME_WINDOW_TITLE_HEIGHT, THEME_WINDOW_TITLE_BACKGROUND_COLOR);
		Draw3dButton(ctx, closeButton,THEME_WINDOW_TITLE_BACKGROUND_COLOR,false);
		
		int titleTextSize = GrFontStringWidth(titleFont, _title.c_str(), _title.size(), GR_BYTE_TEXT);
		int fontHeight = GrFontCharHeight(titleFont, "A");
		int centeredX = (innerWidth / 2);// - (titleTextSize / 2);
		int centeredY = (THEME_WINDOW_TITLE_HEIGHT / 2);
		GrDrawString((void*)_title.c_str(), _title.size(), centeredX, centeredY, &titleTextOptions);
	}

	void CheckMouseClick(int mouseX, int mouseY){
        if (CoordsIntersectBox(closeButton, mouseX, mouseY)){
			Close();
		}
    }

protected:

	void OnKeyUp(int ScanCode, int ShiftState, int Ascii){
		if (ScanCode == KEY_ESC){
			Close();
		}
	}

public:

    void OnEvent(EventEmitter *source, std::string event, EventData data) {
        if (event == "LeftMouseButtonUp"){
            CheckMouseClick(data.data1,data.data2);
        }
        UIWindow::OnEvent(source, event, data);
    }

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

		//titleFont = UIHelpers::ResolveFont(THEME_WINDOW_TITLE_FONT);
		titleFont = GrLoadFont(THEME_WINDOW_TITLE_FONT);

		titleTextOptions.txo_font = titleFont;
		titleTextOptions.txo_fgcolor.v = THEME_WINDOW_TITLE_TEXT_COLOR;
		titleTextOptions.txo_bgcolor.v = GrNOCOLOR;
		titleTextOptions.txo_direct = GR_TEXT_RIGHT;
		titleTextOptions.txo_xalign = GR_ALIGN_CENTER;
		titleTextOptions.txo_yalign = GR_ALIGN_CENTER;
		titleTextOptions.txo_chrtype = GR_BYTE_TEXT;

		closeButton = {drawWidth - THEME_WINDOW_TITLE_HEIGHT - (THEME_WINDOW_BORDER_WIDTH * 2), THEME_WINDOW_BORDER_WIDTH * 2, drawWidth - (THEME_WINDOW_BORDER_WIDTH * 4), THEME_WINDOW_TITLE_HEIGHT - THEME_WINDOW_BORDER_WIDTH};

	}
};
 

#endif