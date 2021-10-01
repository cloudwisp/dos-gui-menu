#ifndef UIImagePanel_cpp
#define UIImagePanel_cpp

#include <grx20.h>
#include <vector>
#include "UIDrawable.cpp"
#include "../Res.cpp"

class UIImagePanel : public UIDrawable {
private:
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
	bool scaling = true; //If false, render whatever dimensions are input, with clipping possible.

	void draw_internal(){
	    GrSetContext(ctx);
		GrClearContextC(ctx, THEME_COLOR_BLACK);
		if (scaling){
			if (!sizedImg){ return; }
			GrImageDisplay(0,0,sizedImg);
			return;
		}
		
		if (!imctx){ return; }
		GrBitBlt(ctx, 0, 0, imctx, 0, 0, loadedWidth, loadedHeight, GrIMAGE);
		
		
	}

	void _load_image(){
        if (!hasImage){ return; }
        if (sizedImg){
            GrImageDestroy(sizedImg);
        }
        ResetColors();
        imctx = AppResources::LoadImage(imagePath);
		GrContext *prevCtx = GrCurrentContext();
		GrSetContext(imctx);
		loadedWidth = GrSizeX();
		loadedHeight = GrSizeY();
		GrSetContext(prevCtx);
		if (!scaling){
			return;
		}
		img = GrImageFromContext(imctx);
		//fit to width
		double ratio = (double)loadedHeight/(double)loadedWidth;
		int newHeight = width*ratio;
		sizedImg = GrImageStretch(img, width, newHeight);
	}

protected:

	void Update(){
		if (hasImage && !imageLoaded && loadDelay > 0){
			clock_t now = clock();
			if (clockToMilliseconds(now-loadImageStart) > loadDelay){
				imageLoaded = true;
				loadingText->Hide();
				_load_image();
				needsRedraw = true;
			}
		}
	}

public:

    void SetImage(std::string filename, int delay = 0){
        imagePath = std::string(filename);
		hasImage = true;
		imageLoaded = false;
		loadDelay = delay;

		if (delay == 0){
			loadingText->Hide();
			_load_image();
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
		loadingText->SetColor(GrWhite(), THEME_COLOR_BLACK);
		loadingText->SetText("Loading...");
		AddChild(loadingText);
		loadingText->Hide();

	}
	~UIImagePanel(){
		delete loadingText;
	}
};

#endif