#ifndef UIImagePanel_cpp
#define UIImagePanel_cpp

#include <grx20.h>
#include <vector>
#include "UIDrawable.cpp"
#include "../Res.cpp"

class UIImagePanel : public UIDrawable {
private:
	static std::vector<UIImagePanel*> loadedPanels;
	GrImage* img = NULL;
	GrImage* sizedImg = NULL;
	std::string imagePath;
	bool hasImage = false;
	GrContext* imctx = NULL;
	int loadedWidth = 0;
	int loadedHeight = 0;
	clock_t loadImageStart;
	int loadDelay = 0;
	bool imageLoaded = false;
	UITextArea *loadingText = NULL;

	void draw_internal(){
	    GrSetContext(ctx);
		GrClearContextC(ctx, THEME_COLOR_TRANSPARENT);
		if (scaleToWidth || scaleToHeight){
			if (!sizedImg){ return; }
			GrImageDisplay(0,0,sizedImg);
			return;
		}
		
		if (!imctx){ return; }
		GrBitBlt(ctx, 0, 0, imctx, 0, 0, loadedWidth, loadedHeight, GrIMAGE);
	}

	void _load_image(bool refresh){
        if (!hasImage){ return; }
        if (sizedImg){
            GrImageDestroy(sizedImg);
			sizedImg = NULL;
        }
		if (img){
			GrImageDestroy(img);
			img = NULL;
		}
		if (!refresh){
			ResetColors();
			RefreshOtherImages(); //load other images, so they are allocated colors.
		}
        imctx = AppResources::LoadImage(imagePath);
		if (!imctx){
			hasImage = false;
			return;
		}
		GrContext *prevCtx = GrCurrentContext();
		GrSetContext(imctx);
		loadedWidth = GrSizeX();
		loadedHeight = GrSizeY();
		GrSetContext(prevCtx);
		if (!scaleToWidth && !scaleToHeight){
			return;
		}
		img = GrImageFromContext(imctx);
		int newHeight = height;
		int newWidth = width;
		if (!scaleToHeight){
			//fit to width proportionally
			double hwratio = (double)loadedHeight/(double)loadedWidth;
			newHeight = width*hwratio;
		} else if (!scaleToWidth) {
			//fit to height proportionally
			double whratio = (double)loadedWidth/(double)loadedHeight;
			newWidth = height*whratio;
		}
		
		sizedImg = GrImageStretch(img, newWidth, newHeight);
	}

	void RefreshOtherImages(){
		for (UIImagePanel* img : loadedPanels){
			if (img == this){
				continue;
			}
			img->ReloadImageExt();
		}
	}

protected:

	void ReloadImageExt(){
		_load_image(true);
	}

	void Update(){
		if (hasImage && !imageLoaded && loadDelay > 0){
			clock_t now = clock();
			if (clockToMilliseconds(now-loadImageStart) > loadDelay){
				imageLoaded = true;
				loadingText->Hide();
				_load_image(false);
				needsRedraw = true;
			}
		}
	}

public:

	bool scaleToWidth = true;
	bool scaleToHeight = false;

    void SetImage(std::string filename, int delay = 0){
        imagePath = std::string(filename);
		hasImage = true;
		imageLoaded = false;
		loadDelay = delay;
		if (sizedImg){
			GrImageDestroy(sizedImg);
			sizedImg = NULL;
		}

		if (delay == 0){
			loadingText->Hide();
			_load_image(false);
			needsRedraw = true;
			return;
		}

		loadImageStart = clock();
		loadingText->Show();
		needsRedraw = true;
    }

	UIImagePanel(int drawWidth, int drawHeight) : UIDrawable(drawWidth, drawHeight) {
		loadingText = new UITextArea(drawWidth, drawHeight);
		loadingText->SetAlign(GR_ALIGN_CENTER, GR_ALIGN_CENTER);
		loadingText->SetColor(GrWhite(), THEME_COLOR_TRANSPARENT);
		loadingText->SetText("Loading...");
		AddChild(loadingText);
		loadingText->Hide();
		loadedPanels.push_back(this);
	}
	~UIImagePanel(){
		int myIndex = -1;
		for (int i = 0; i < loadedPanels.size(); i++){
			if (loadedPanels.at(i) == this){
				myIndex = i;
			}
		}
		loadedPanels.erase(loadedPanels.begin()+myIndex);
		delete loadingText;
	}
};

std::vector<UIImagePanel*> UIImagePanel::loadedPanels = std::vector<UIImagePanel*>();

#endif