#ifndef UILabelAndText_cpp
#define UILabelAndText_cpp

#include "../AppUI.cpp"

class UILabelAndText: public UIDrawable {
private:
    UITextArea* label = NULL;
    UITextArea* text = NULL;
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

    UILabelAndText(int width, int labelHeight, int textHeight, double labelPercentOfWidth) : UIDrawable(width, textHeight){
        int labelWidth = (double)innerWidth * (labelPercentOfWidth/100);
        int textWidth = innerWidth - labelWidth;
        
        label = new UITextArea(labelWidth, labelHeight, 2);
        label->SetColor(THEME_PANEL_TEXT_PRIMARY, THEME_COLOR_TRANSPARENT);
        text = new UITextArea(textWidth, textHeight, 2);
        text->SetColor(THEME_PANEL_TEXT_PRIMARY, THEME_COLOR_TRANSPARENT);
        text->x = labelWidth;
        AddChild(label);
        AddChild(text);
    }
    ~UILabelAndText(){
        delete label;
        delete text;
    }
};

#endif