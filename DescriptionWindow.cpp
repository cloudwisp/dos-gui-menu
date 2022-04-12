#ifndef DescriptionWindow_CPP
#define DescriptionWindow_CPP

#include "AppUI.cpp"

class DescriptionWindow : public UITitledWindow {
private:
    UIScrollingText* textPanel = NULL;
    UITextArea* textArea = NULL;
public:

    void SetText(std::string text, GrFont *font){
        if (textPanel == NULL){
            textPanel = new UIScrollingText(innerWidth - 4, innerHeight - 4);
            textPanel->x = 2;
            textPanel->y = 2;
            textArea = textPanel->GetText();
            AddChild(textPanel);
        }
        
        textArea->SetText(text);
        textArea->SetFont(font);
        textArea->SizeHeightToContent();
        textPanel->ScrollToTop();
        textPanel->Focus();
    }

    DescriptionWindow(int width, int height, std::string title) : UITitledWindow(width, height, title){
        
    }
};

#endif