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
	void draw_internal(){
	    if (!sizedImg){ return; }
		GrSetContext(ctx);
		GrClearContextC(ctx, GrNOCOLOR);
		GrImageDisplay(0,0,sizedImg);
		Freeze();
	}

	void _load_image(){
        if (!hasImage){ return; }
        if (sizedImg){
            GrImageDestroy(sizedImg);
        }
        GrContext* imctx = AppResources::LoadImage(imagePath);
		img = GrImageFromContext(imctx);
		//fit to width
		int imWidth, imHeight;
		GrContext *prevCtx = GrCurrentContext();
		GrSetContext(imctx);
		imWidth = GrSizeX();
		imHeight = GrSizeY();
		GrSetContext(prevCtx);
		//GrDestroyContext(imctx);
		double ratio = (double)imHeight/(double)imWidth;
		int newHeight = width*ratio;
		sizedImg = GrImageStretch(img, width, newHeight);
	}

public:

    void Show(){
        if (!sizedImg && hasImage){
            _load_image();
        }
        UIDrawable::Show();
    }

    void SetImage(std::string filename){
        imagePath = std::string(filename);
        hasImage = true;
        _load_image();
		Unfreeze();
    }

	UIImagePanel(int drawWidth, int drawHeight) : UIDrawable(drawWidth, drawHeight) {

	}
};

#endif