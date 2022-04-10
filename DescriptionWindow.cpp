#ifndef DescriptionWindow_CPP
#define DescriptionWindow_CPP

#include "AppUI.cpp"

class DescriptionWindow : public UITitledWindow {
private:
    UIScrollingText* textPanel = NULL;
    UITextArea* textArea = NULL;
public:

    void SetText(std::string text, GrFont *font){
        if (textPanel != NULL){
            RemoveChild(textPanel);
            delete textPanel;
            textArea = NULL;
        }
        textPanel = new UIScrollingText(innerWidth - 4, innerHeight - 4);
        textPanel->x = 2;
        textPanel->y = 2;
        textArea = textPanel->GetText();
        textArea->SetText(text);
        textArea->SetFont(font);
        textArea->SizeHeightToContent();
        AddChild(textPanel);

        textPanel->Focus();
    }

    DescriptionWindow(int width, int height, std::string title) : UITitledWindow(width, height, title){
        
    }
};

#endif