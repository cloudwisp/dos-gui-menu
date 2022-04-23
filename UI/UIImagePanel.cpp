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
	bool imageRendered = false;
	UITextArea *loadingText = NULL;
	int scaledWidth = 0;
	int scaledHeight = 0;
	int currentScanLine = 0;
	int scanRow = 0;
	int scanRowCount = 0;
	int scanRowOuter = 0;
	int scanRowOuterCount = 0;
	int linesPerUpdate = 5;
	GrColor bgColor = THEME_COLOR_TRANSPARENT;
	
	int orderedColorCount = 0;
	bool progressiveInterlaced = true;
	int interlaceSliceCount = 30;


	PNGScanLines currentImage = {0,0,0,NULL};

	void draw_internal(){
		if (!hasImage){
			return;
		}
		if (progressive && !progressiveInterlaced){
			if (currentScanLine < loadedHeight){
				for (int i = 0; i <= linesPerUpdate; i++){
					if (currentScanLine >= loadedHeight){
						break;
					}
					RenderScanline(currentScanLine + 1);
					currentScanLine++;
				}
				ScaleImage();
				//needsRedraw = true;
			}
		} else if (progressive && progressiveInterlaced){
			if (scanRow < scanRowCount){
				for (int i = 0; i <= linesPerUpdate; i++){
					if (scanRow > scanRowCount){
						break;
					}
					int thisLine = scanRowCount * scanRowOuter + scanRow;
					if (thisLine >= loadedHeight){
						break;
					}
					RenderScanline(thisLine);
					scanRowOuter++;
					if (scanRowOuter >= scanRowOuterCount){
						scanRowOuter = 0;
						scanRow++;
					}
				}
				ScaleImage();
			}
		} else if (!imageRendered) {
			for (int i = 0; i < loadedHeight; i++){
				RenderScanline(i);
				currentScanLine++;
			}
			ScaleImage();
			imageRendered = true;
			needsRedraw = true;
		}

	    GrSetContext(ctx);
		GrClearContextC(ctx, bgColor);

		if (scaleToWidth || scaleToHeight){
			if (sizedImg == NULL){ return; }
			GrImageDisplay(0,0,sizedImg);
			return;
		}
		
		if (imctx == NULL){ return; }
		GrBitBlt(ctx, 0, 0, imctx, 0, 0, loadedWidth, loadedHeight, GrIMAGE);
	}

	void _load_image(bool refresh){
		currentScanLine = 0;
		scanRow = 0;
		scanRowCount = 0;
		scanRowOuter = 0;
		scanRowOuterCount = 0;
		loadedWidth = 0;
		loadedHeight = 0;
		imageRendered = false;
        if (!hasImage){ return; }
        if (sizedImg != NULL){
            GrImageDestroy(sizedImg);
			sizedImg = NULL;
        }
		if (img != NULL){
			GrImageDestroy(img);
			img = NULL;
		}
		if (!refresh){
			ResetColors();
			RefreshOtherImages(); //load other images, so they are allocated colors.
		}
		if (currentImage.pixelCount > 0 && currentImage.pixels != NULL){
			free(currentImage.pixels);
		}
		currentImage = AppResources::LoadImageScanLines(imagePath);
		loadedWidth = currentImage.width;
		loadedHeight = currentImage.height;
		if (currentImage.pixelCount == 0){
			hasImage = false;
			return;
		}
		
		if (imctx != NULL){
			GrDestroyContext(imctx);
		}
		imctx = GrCreateContext(currentImage.width, currentImage.height, NULL, NULL);
		GrClearContextC(imctx, THEME_COLOR_TRANSPARENT);
        //imctx = AppResources::LoadImage(imagePath);
		if (imctx == NULL){
			hasImage = false;
			return;
		}
		
		if (progressive && progressiveInterlaced){
			scanRowCount = loadedHeight / (double)interlaceSliceCount;
			scanRowOuterCount = interlaceSliceCount;
		}
		if (!scaleToWidth && !scaleToHeight){
			return;
		}
		img = GrImageFromContext(imctx);
		scaledHeight = height;
		scaledWidth = width;
		if (!scaleToHeight){
			//fit to width proportionally
			double hwratio = (double)loadedHeight/(double)loadedWidth;
			scaledHeight = width*hwratio;
		} else if (!scaleToWidth) {
			//fit to height proportionally
			double whratio = (double)loadedWidth/(double)loadedHeight;
			scaledWidth = height*whratio;
		}

		// if (!progressive){
		// 	ScaleImage();
		// }
		
		//sizedImg = GrImageStretch(img, newWidth, newHeight);
	}

	void RenderScanline(int lineNumber){
		if (!hasImage || currentImage.pixelCount == 0){
			return;
		}
		GrColor *pColors = (GrColor*)malloc(currentImage.width * sizeof(GrColor));
		
		int lineStart = lineNumber * currentImage.width;
		int lineEnd = ((lineNumber + 1) * currentImage.width) - 1;
		int a = 0;
		for (int i = lineStart; i <= lineEnd; i++){
			if (i > currentImage.pixelCount - 1){
				break;
			}
			PNGRGB c = currentImage.pixels[i];
			pColors[a] = GrAllocColor(c.r, c.g, c.b);
			a++;
		}
		GrSetContext(imctx);
		GrPutScanline(0,currentImage.width - 1, lineNumber, pColors, GrIMAGE);
		free(pColors);
	}

	void ScaleImage(){
		if (sizedImg != NULL){
			GrImageDestroy(sizedImg);
		}
		if (scaleToWidth || scaleToHeight){
			sizedImg = GrImageStretch(img, scaledWidth, scaledHeight);
		}
	}

	void RefreshOtherImages(){
		for (UIImagePanel* img : loadedPanels){
			if (img == this){
				continue;
			}
			img->ResetColorsExt();
		}
	}

	void ClearImage(){
		hasImage = false;
		loadedWidth = 0;
		loadedHeight = 0;
		currentScanLine = 0;
		scanRow = 0;
		scanRowCount = 0;
		scanRowOuter = 0;
		scanRowOuterCount = 0;
		if (imctx != NULL){
			GrDestroyContext(imctx);
			imctx = NULL;
		}
		if (sizedImg != NULL){
			GrImageDestroy(sizedImg);
			sizedImg = NULL;
		}
		needsRedraw = true;
	}

protected:

	void ResetColorsExt(){
		if (!hasImage){
			return;
		}
		int count = currentImage.pixelCount;
		for (int i = 0; i < count; i++){
			GrAllocColor(currentImage.pixels[i].r, currentImage.pixels[i].g, currentImage.pixels[i].b);
		}
		//_load_image(true);
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
		} else if (currentScanLine < loadedHeight){
			needsRedraw = true;
		}
	}

public:

	//disable this only if you know this image is the last to be added of all of the visible images on the screen.
	bool preserveColors = true;
	bool scaleToWidth = true;
	bool scaleToHeight = false;
	bool progressive = false;

	void SetLoadingFont(GrFont *font){
		loadingText->SetFont(font);
	}

	void SetBackgroundColor(GrColor color){
		bgColor = color;
		needsRedraw = true;
	}

    void SetImage(std::string filename, int delay = 0){
		ClearImage();
		if (filename == ""){
			//ClearImage();
			return;
		}
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
			imageLoaded = true;
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