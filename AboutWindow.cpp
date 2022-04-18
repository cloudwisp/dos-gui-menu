#ifndef AboutWindow_CPP
#define AboutWindow_CPP

#include "AppUI.cpp"

class AboutWindow : public UITitledWindow {
private:
    UIScrollingText* textPanel = NULL;
    UIImagePanel* logo = NULL;
    UITextArea* textArea = NULL;
    UIStackedPanel* mainPanel = NULL;
    std::string *aboutText = NULL;
    int padding = 2;
public:

    AboutWindow(int width, int height, bool highRes) : UITitledWindow(width, height, "Cloudwisp Graphical Menu"){
        mainPanel = new UIStackedPanel(THEME_COLOR_TRANSPARENT, innerWidth, innerHeight);
        AddChild(mainPanel);

        int imgWidth = innerWidth;
        int imgHeight = 35;
        if (highRes){
            imgWidth = innerWidth;
            imgHeight = 53;
        }
        logo = new UIImagePanel(imgWidth, imgHeight);
        logo->SetBackgroundColor(THEME_COLOR_BLACK);
        logo->progressive = false;
        logo->preserveColors = true;
        logo->scaleToWidth = false;
        mainPanel->AddChild(logo);
        if (highRes){
            logo->SetImage("cloud.png", 0);
        } else {
            logo->SetImage("cloudsm.png", 0);
        }
        

        textPanel = new UIScrollingText(mainPanel->innerWidth, mainPanel->innerHeight - logo->height);
        textArea = textPanel->GetText();
        aboutText = AppResources::Current()->GetReadme("about.txt");
        textArea->SetText(*aboutText);
        mainPanel->AddChild(textPanel);

        textArea->SizeHeightToContent();
        textPanel->ScrollToTop();
        textPanel->Focus();

    }
    ~AboutWindow(){
        delete aboutText;
        delete textArea;
        delete textPanel;
        delete logo;
        delete mainPanel;
    }
};

#endif