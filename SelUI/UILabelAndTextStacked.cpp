#ifndef UILabelAndTextStacked_cpp
#define UILabelAndTextStacked_cpp

#include "../AppUI.cpp"

class UILabelAndTextStacked: public UIDrawable {
private:
    UITextArea* label = NULL;
    UITextArea* text = NULL;
    UIStackedPanel* pnl = NULL;
    void draw_internal(){
        GrClearContextC(ctx, THEME_COLOR_TRANSPARENT);
    }
protected:

public:

    void SetTextFont(GrFont *font){
        text->SetFont(font);
        needsRedraw = true;
    }

    void SetText(std::string txt){
        text->SetText(txt);
        needsRedraw = true;
    }

    void SetLabelFont(GrFont *font){
        label->SetFont(font);
        needsRedraw = true;
    }

    void SetLabel(std::string lbl){
        label->SetText(lbl);
        needsRedraw = true;
    }

    UILabelAndTextStacked(int width, int labelHeight, int textHeight, int textAreaPadding) : UIDrawable(width, labelHeight + textHeight){
        pnl = new UIStackedPanel(THEME_COLOR_TRANSPARENT, width, labelHeight + textHeight);
        label = new UITextArea(width, labelHeight, textAreaPadding);
        label->SetColor(THEME_PANEL_TEXT_PRIMARY, THEME_COLOR_TRANSPARENT);
        text = new UITextArea(width, textHeight, textAreaPadding);
        text->SetColor(THEME_PANEL_TEXT_PRIMARY, THEME_COLOR_TRANSPARENT);
        pnl->AddChild(label);
        pnl->AddChild(text);
        AddChild(pnl);
    }
    ~UILabelAndTextStacked(){
        delete label;
        delete text;
        delete pnl;
    }
};

#endif