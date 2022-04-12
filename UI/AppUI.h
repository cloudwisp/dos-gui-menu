#ifndef _cw_ui_
#define _cw_ui_

class UIDrawable;
class UIPointer;
class UIAppScreen;
class UIPanel;
class UIStackedPanel;
class UITextArea;
class UIWindow;
class UITextWindow;
class UITitledWindow;
class UIModalWindow;
class UIMsgBox;
class UIScrollingText;
class UIButton;
class UITextBox;

const int CW_ALIGN_LEFT = 0;
const int CW_ALIGN_RIGHT = 1;
const int CW_ALIGN_CENTER = 2;
const int CW_ALIGN_BOTTOM = 3;
const int CW_ALIGN_TOP = 4;

struct CWRGB {
    UINT8 r;
    UINT8 g;
    UINT8 b;
};

struct BoxCoords {
    int x1;
    int y1;
    int x2;
    int y2;
};

struct Coord {
    int x;
    int y;
};

#endif
