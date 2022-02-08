#ifndef AppOptions_CPP
#define AppOptions_CPP

#include "AppUI.cpp"

class AppOptionsWindow : public UITitledWindow {
private:

public:
    AppOptionsWindow(int width, int height) : UITitledWindow(width, height, "Options"){
    }
};

#endif