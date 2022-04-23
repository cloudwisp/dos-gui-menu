#ifndef UITextArea_cpp
#define UITextArea_cpp

#include <grx20.h>
#include <cstring>
#include <string>
#include <vector>
#include "AppUI.h"
#include "UIHelpers.cpp"
#include "UIDrawable.cpp"

class UITextArea : public UIDrawable {
private:
	std::string text = std::string();
	GrTextOption textOptions;
	GrColor backgroundColor;
	std::vector<int> lineWidths = std::vector<int>(0);
	std::vector<std::string> lines = std::vector<std::string>(0);
	int _charHeight = 0;

	char _horizAlign;
	char _vertAlign;
	bool drawCursor = false;
	int cursorPosition = 0;
	int cursorLine = 0;
	int cursorColumn = 0;
	int textHeight = 0;
	
	void _parse_text(){
		lineWidths.clear();
		lineWidths.resize(0);
		lines.clear();
		lines.resize(0);
		if (text.size() == 0){
			return;
		}
		
		int j, i;
		//parse the text into words
		std::vector<std::string> words = std::vector<std::string>(1);
		std::string current = std::string("");
		std::string spaceChars = std::string(" \t\n-");
		for (i = 0; i < text.size(); i++){
			char thisChar = text.at(i);
			std::size_t spaceIndex = spaceChars.find(thisChar);
			if (spaceIndex != std::string::npos){
				words.push_back(std::string(current));
				std::string ws = std::string("");
				ws.append(text,i,1);
				words.push_back(ws);
				current.clear();
				continue;
			}
			current.append(text,i, 1);
		}
		words.push_back(current);

		int lineWidth, charWidth, globalPos;
		_charHeight = GrFontCharHeight(textOptions.txo_font,(int) text[0]);
		lineWidth = 0;
		int curLine = 0;
		globalPos = 0;
		lines.push_back(std::string(""));
		for (j = 0; j < words.size(); j++){
			int wordChars = words[j].size();
			int wordWidth = GrFontStringWidth(textOptions.txo_font, words[j].c_str(), wordChars, GR_BYTE_TEXT);
			bool forceBreak = false;
			if (wordWidth < lineWidth && lineWidth + wordWidth > width){
				//line break to avoid splitting word.
				forceBreak = true;
			}
			for (i = 0; i < words[j].size(); i++){
				char thisChar = words[j].at(i);
				bool isNewLineChar = thisChar == '\n';
				bool lineBroke = false;
				if ((forceBreak && lineWidth > 0) || isNewLineChar){
					lineWidths.push_back(lineWidth);
					lines.push_back(std::string(""));
					curLine++;
					lineWidth = 0;
					if (cursorPosition - 1 == globalPos){
						cursorLine = curLine;
						cursorColumn = 0;
					}
					forceBreak = false;
					lineBroke = true;
				}
				if (isNewLineChar || (lineBroke && thisChar == ' ')){
					//don't render a space, if the space caused a line break.
					//on actual newline, always skip render
					continue;
				}
				if (cursorPosition - 1 == globalPos){
					cursorLine = curLine;
					cursorColumn = lines[curLine].size();
				}
				charWidth = GrFontCharWidth(textOptions.txo_font, (int) words[j].at(i));
				if (lineWidth + charWidth > width){
					lineWidths.push_back(lineWidth);
					lines.push_back(std::string(""));
					curLine++;
					lineWidth = 0;
				}
				lines[curLine].append(words[j],i,1);
				lineWidth+= charWidth;
				globalPos++;
			}
		}
		lineWidths.push_back(lineWidth); //last one
	}

	void draw_internal(){
		GrContext *prevCtx = GrCurrentContext();
		GrClearContextC(ctx, THEME_COLOR_TRANSPARENT);
		GrClearContextC(ctx, backgroundColor);
		int limitWidth = width;
		int limitHeight = height;
		GrContext* drawContext = ctx;
		
		if (innerContext){
			drawContext = innerContext;
			GrClearContextC(drawContext, THEME_COLOR_TRANSPARENT);
			GrClearContextC(drawContext, backgroundColor);
			limitWidth = innerWidth;
			limitHeight = innerHeight;
		}

		GrSetContext(drawContext);
	
		int numLines = lines.size();
		int i, x, y, firstLine;
		x = 0;
		y = 0;
		firstLine = 0;
		//tbd line spacing
		if (_vertAlign == GR_ALIGN_CENTER) {
			int blockHeight = numLines * _charHeight;
			y = (limitHeight/2) - (blockHeight / 2);
			if (y < 0){
				//some clipping will occur, find first visible line, TODO: Better solution for this scenario?
				firstLine = (0-y) / _charHeight;
			}
		}
		for (i = firstLine; i < numLines; i++){
			if (y > limitHeight){ break; }
			x = 0;
			if (_horizAlign == GR_ALIGN_CENTER){
				x = (limitWidth / 2) - (lineWidths[i] / 2);
			} else if (_horizAlign == GR_ALIGN_RIGHT){
				x = limitWidth - (lineWidths[i]);
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
		needsRedraw = true;
	}

	std::string GetText(){
		return text;
	}

	void CharAdd(char character){
		text.insert(cursorPosition, 1, character);
		cursorPosition++;
		_parse_text();
		needsRedraw = true;
	}

	void CharBackspace(){
		if (cursorPosition == 0){
			return;
		}
		text.erase(cursorPosition - 1, 1);
		cursorPosition--;
		_parse_text();
		needsRedraw = true;
	}

	void CharDelete(){
		if (cursorPosition >= text.size() - 1){
			return;
		}
		text.erase(cursorPosition, 1);
		_parse_text();
		needsRedraw = true;
	}

	void CursorLeft(){
		if (cursorPosition > 0){
			cursorPosition--;
			_parse_text();
			needsRedraw = true;
		}
	}

	void CursorRight(){
		if (cursorPosition < text.size()){
			cursorPosition++;
			_parse_text();
			needsRedraw = true;
		}
	}

	void CursorUp(){
		if (cursorLine > 0){
			cursorLine--;
			if (lines[cursorLine].size() -1 < cursorColumn){
				cursorColumn = lines[cursorLine].size() - 1;
			}
			needsRedraw = true;
		}
	}

	void CursorDown(){
		if (cursorLine < lines.size() - 1){
			cursorLine++;
			if (lines[cursorLine].size() - 1 < cursorColumn){
				cursorColumn = lines[cursorLine].size() - 1;
			}
			needsRedraw = true;
		}
	}

	void SetFont(GrFont *font){
		textOptions.txo_font = font;
		_parse_text(); //change in font affects line breaks
		needsRedraw = true;
	}

	void SetColor(GrColor foreColor, GrColor backColor){
		textOptions.txo_fgcolor.v = foreColor;
		backgroundColor = backColor;
		needsRedraw = true;
	}

	void SetAlign(char horizAlign, char vertAlign){
		_horizAlign = horizAlign;
		_vertAlign = vertAlign;
		needsRedraw = true;
	}

	void ShowCursor(){
		drawCursor = true;
		needsRedraw = true;
	}

	void HideCursor(){
		drawCursor = false;
		needsRedraw = true;
	}

	int GetLineHeight(){
		return _charHeight;
	}

	void SizeHeightToContent(){
		int numLines = lines.size();
		int blockHeight = numLines * _charHeight;
		if (blockHeight <= 0){
			blockHeight = 20;
		}
		if (!singleContext){
			int diff = height - innerHeight;
			int newOuterHeight = blockHeight + diff;
			if (newOuterHeight < 0){
				newOuterHeight = blockHeight;
			}

			SetInnerDimensions(innerWidth, blockHeight, innerContextX, innerContextY);
			SetDimensions(width, newOuterHeight);
		} else {
			SetDimensions(width, blockHeight);
		}
		needsRedraw = true;
	}

	UITextArea(int drawWidth, int drawHeight) : UITextArea(drawWidth, drawHeight, 0){
	}

	UITextArea(int drawWidth, int drawHeight, int padding) : UIDrawable(drawWidth, drawHeight, padding) {
		text = "";
		backgroundColor = THEME_COLOR_TRANSPARENT;
        textOptions.txo_font = UIHelpers::ResolveFont(THEME_DEFAULT_FONT);
		textOptions.txo_fgcolor.v = THEME_WINDOW_TEXT_COLOR;
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
		lines.resize(0);
	    lineWidths.clear();
		lineWidths.resize(0);
	}

};

#endif