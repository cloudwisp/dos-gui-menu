#ifndef UITextArea_cpp
#define UITextArea_cpp

#include <grx20.h>
#include <cstring>
#include <string>
#include <vector>
#include "AppUI.h"
#include "UIDrawable.cpp"

class UITextArea : public UIDrawable {
private:
	std::string text = std::string();
	GrTextOption textOptions;
	GrColor backgroundColor;
	std::vector<int> lineWidths = std::vector<int>(1);
	std::vector<std::string> lines = std::vector<std::string>(1);
	int _charHeight = 0;

	char _horizAlign;
	char _vertAlign;
	bool drawCursor = false;
	int cursorPosition = 0;
	int cursorLine = 0;
	int cursorColumn = 0;

	void _parse_text(){
		lineWidths.clear();
		lines.clear();
		if (text.size() == 0){
			return;
		}
		//does not parse at word level
		int lineWidth, charWidth, i;
		_charHeight = GrFontCharHeight(textOptions.txo_font,(int) text[0]);
		lineWidth = 0;
		int curLine = 0;
		lines.push_back(std::string(""));
		for (i = 0; i < text.size(); i++){
			if (text.at(i) == '\n'){
				lineWidths.push_back(lineWidth);
				lines.push_back(std::string(""));
				curLine++;
				lineWidth = 0;
				if (cursorPosition - 1 == i){
					cursorLine = curLine;
					cursorColumn = 0;
				}
				continue;
			}
			if (cursorPosition - 1 == i){
				cursorLine = curLine;
				cursorColumn = lines[curLine].size();
			}
			charWidth = GrFontCharWidth(textOptions.txo_font, (int) text.at(i));
			if (lineWidth + charWidth > width){
				lineWidths.push_back(lineWidth);
				lines.push_back(std::string(""));
				curLine++;
				lineWidth = 0;
			}
			lines[curLine].append(text,i,1);
			lineWidth+= charWidth;
		}
		lineWidths.push_back(lineWidth); //last one
	}

	void draw_internal(){
		GrContext *prevCtx = GrCurrentContext();
		GrSetContext(ctx);
		GrClearContextC(ctx, GrAllocColor(0,0,0));
		GrClearContextC(ctx, backgroundColor);

		int numLines = lines.size();
		int i, x, y, firstLine;
		x = 0;
		y = 0;
		firstLine = 0;
		//tbd line spacing
		if (_vertAlign == GR_ALIGN_CENTER) {
			int blockHeight = numLines * _charHeight;
			y = (height/2) - (blockHeight / 2);
			if (y < 0){
				//some clipping will occur, find first visible line, TODO: Better solution for this scenario?
				firstLine = (0-y) / _charHeight;
			}
		}
		for (i = firstLine; i < numLines; i++){
			if (y > height){ break; }
			x = 0;
			if (_horizAlign == GR_ALIGN_CENTER){
				x = (width / 2) - (lineWidths[i] / 2);
			} else if (_horizAlign == GR_ALIGN_RIGHT){
				x = width - (lineWidths[i]);
			}
			GrDrawString((void *) lines[i].c_str(), lines[i].length(), x, y, &textOptions);
			if (drawCursor && cursorLine == i){
				int cursorX = 0;
				if (lines[i].size() > 0){
					int charWidth = lineWidths[i] / lines[i].size();
					cursorX = cursorColumn * charWidth + charWidth;
				}
				
				GrLine(cursorX, y, cursorX, y + _charHeight, textOptions.txo_fgcolor.v);
			}
			y+=_charHeight;
		}

	}

protected:

public:

	void SetText(char *txt){
		SetText(std::string(txt));
	}

	void SetText(std::string txt){
		cursorPosition = txt.size();
		text = txt;
		_parse_text();
	}

	std::string GetText(){
		return text;
	}

	void CharAdd(char character){
		text.insert(cursorPosition, 1, character);
		cursorPosition++;
		_parse_text();
	}

	void CharBackspace(){
		if (cursorPosition == 0){
			return;
		}
		text.erase(cursorPosition - 1, 1);
		cursorPosition--;
		_parse_text();
	}

	void CharDelete(){
		if (cursorPosition >= text.size() - 1){
			return;
		}
		text.erase(cursorPosition, 1);
		_parse_text();
	}

	void CursorLeft(){
		if (cursorPosition > 0){
			cursorPosition--;
			_parse_text();
		}
	}

	void CursorRight(){
		if (cursorPosition < text.size()){
			cursorPosition++;
			_parse_text();
		}
	}

	void CursorUp(){
		if (cursorLine > 0){
			cursorLine--;
			if (lines[cursorLine].size() -1 < cursorColumn){
				cursorColumn = lines[cursorLine].size() - 1;
			}
		}
	}

	void CursorDown(){
		if (cursorLine < lines.size() - 1){
			cursorLine++;
			if (lines[cursorLine].size() - 1 < cursorColumn){
				cursorColumn = lines[cursorLine].size() - 1;
			}
		}
	}

	void SetFont(GrFont *font){
		textOptions.txo_font = font;
		_parse_text(); //change in font affects line breaks
	}

	void SetColor(GrColor foreColor, GrColor backColor){
		textOptions.txo_fgcolor.v = foreColor;
		textOptions.txo_bgcolor.v = backColor;
		backgroundColor = backColor;
	}

	void SetAlign(char horizAlign, char vertAlign){
		_horizAlign = horizAlign;
		_vertAlign = vertAlign;
	}

	void ShowCursor(){
		drawCursor = true;
	}

	void HideCursor(){
		drawCursor = false;
	}

	UITextArea(int drawWidth, int drawHeight) : UIDrawable(drawWidth, drawHeight) {
		text = "";
		backgroundColor = GrNOCOLOR;
		textOptions.txo_font = &GrFont_PC6x8;
		textOptions.txo_fgcolor.v = GrWhite();
		textOptions.txo_bgcolor.v = GrNOCOLOR;
		textOptions.txo_direct = GR_TEXT_RIGHT;
		textOptions.txo_xalign = GR_ALIGN_LEFT;
		textOptions.txo_yalign = GR_ALIGN_TOP;
		textOptions.txo_chrtype = GR_BYTE_TEXT;
		_horizAlign = GR_ALIGN_LEFT;
		_vertAlign = GR_ALIGN_TOP;
	}
	~UITextArea(){
	    lines.clear();
	    lineWidths.clear();
	}

};

#endif