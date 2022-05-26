//
// Created by Administrator on 2022/5/22.
//


#include "TextLayout.h"
#include "utf8/utf8.h"
#include "CGWindowBackend.h"
#include "ChainGui.h"
void TextLayout::onResize() {
    double allSize = 0;
    this->pageLayout(this->cacheLineNumber, this->lineTextList.size() - 1, 0, this->yOffset,
                     allSize);
    this->allSize = allSize;

    this->updateCursor();
    lastCursorXPixer = this->hitPos1.LineOffsetPixelX;
}

void TextLayout::updateCursor() {//fixme
    {
        auto hitPos = this->hitPos1;
        std::vector<int> Line{hitPos.pos.realLineNumber};
        std::vector<int> TextPos{hitPos.pos.realLineOffset};
        std::vector<int> subLine{hitPos.pos.subLineNumber};
        std::vector<int> subTextPos{hitPos.pos.subLineOffset};
        std::vector<double> LineHeightPixel{hitPos.LineHeightPixel};


        auto pos = getPosBySubLinePos(Line, TextPos, false);

        if (Line[0] >= this->cacheLineNumber && Line[0] <= this->ViewEyeTextEndLine) {
            if (pos[0].pos.subLineOffset == 0 && pos[0].pos.subLineNumber != 0)
                pos = getPosBySubLinePos(Line, TextPos, false);
            pos[0].pos.subLineOffset = subTextPos[0];
            this->hitPos1 = pos[0];
        }


    }
    {
        auto hitPos = this->hitPos2;
        std::vector<int> Line{hitPos.pos.realLineNumber};
        std::vector<int> TextPos{hitPos.pos.realLineOffset};
        std::vector<int> subLine{hitPos.pos.subLineNumber};
        std::vector<int> subTextPos{hitPos.pos.subLineOffset};
        std::vector<double> LineHeightPixel{hitPos.LineHeightPixel};
        bool NextBegin = false;

        auto pos = getPosBySubLinePos(Line, TextPos, false);

        if (Line[0] >= this->cacheLineNumber && Line[0] <= this->ViewEyeTextEndLine) {
            if (pos[0].pos.subLineOffset == 0 && pos[0].pos.subLineNumber != 0)
                pos = getPosBySubLinePos(Line, TextPos, false);

            pos[0].pos.subLineOffset = subTextPos[0];
            this->hitPos2 = pos[0];
        }
    }
}



std::list<LineStruct>::iterator
TextLayout::lineLayout(std::list<LineStruct>::iterator lineStructIt, int lineNumber, double pageW, bool writeData) {
    auto gui = ChainGui::get();
    LineStruct &lineStruct = *lineStructIt;
    double rate = (double) this->font->fontSize / (double) this->font->fontSizeReal;
    auto &lineStr = lineStruct.lineStr;


    const char *twochars = &lineStr[0];
    const char *widthLineStart = twochars;
    const char *w = twochars;
    const char *oldW = w;
    int len = lineStr.length();
    double curw = 0;
    int cutpos = 0;
    std::string widthLineStr = lineStr;
    double PreLinHeightSum = 0.0;
    bool isFirstLine = true;
    int subLine = 0;
    int realLineOffset = 0;
    lineStruct.lineTextListAutoWidth.clear();
    if (lineStr.empty()) {
        AutoChangeLineStruct al;
        al.lineHight = font_height;
        al.lineStr = "";
        al.realLineOffset = 0;
        lineStruct.lineTextListAutoWidth.emplace_back(al);


        if (writeData) {
            if (ViewTextVec.size() + ViewTextBeginLine <= lineNumber)
                ViewTextVec.resize(1 + lineNumber - ViewTextBeginLine);
            ViewLine vl;
            vl.subLine = al;
            vl.realLineNumber = lineNumber;
            vl.subLineNumber = lineStruct.lineTextListAutoWidth.size() - 1;
            vl.realLineOffset = realLineOffset;
            vl.lineCharCount = 0;
            if (ViewTextVec[lineNumber - ViewTextBeginLine].size() <= vl.subLineNumber)
                ViewTextVec[lineNumber - ViewTextBeginLine].resize(1 + vl.subLineNumber);
            ViewTextVec[lineNumber - ViewTextBeginLine][vl.subLineNumber] = vl;
        }


        PreLinHeightSum += al.lineHight;
        return lineStructIt;
    }
    lineStruct.lineHeight = 0;
    double lineMaxHeight = 0;
    double curH = 0.0;
    bool addEnd = false;
    int oldI = 0;
    for (int i = 0;; i++) {
        oldW = w;
        char32_t codepoint = utf8::next(w, twochars + 1 + len);
        int64_t x = 0;
        if (codepoint == 0) {
            break;
        }

        memcpy(&x, &codepoint, 4);
        int size2 = w - oldW;
        double standardWidth = gui->canvas.getTextWidth((char *) oldW, size2, this->gb, this->font);
        if (standardWidth == 0) {
            double blankWidth = gui->canvas.getTextWidth((char *) "_", 1, this->gb, this->font);
            standardWidth = blankWidth;
        }

        this->textWidthMap[codepoint] = standardWidth;
        double realWidth = standardWidth;
        lineMaxHeight = std::max(lineMaxHeight,
                                 this->font_height);


        if (!autoChangeLine || curw + realWidth <= pageW) {
            curw += realWidth;
        } else {
            int offsetLen = oldW - widthLineStart;
            std::string cutStr(widthLineStart, offsetLen);

            int realLineOffset = widthLineStart - twochars;
            curw = realWidth;
            widthLineStart = oldW;
            if (false) {

            } else {
                AutoChangeLineStruct al;
                al.lineHight = lineMaxHeight;
                al.lineStr = cutStr;
                al.realLineOffset = realLineOffset;
                lineStruct.lineTextListAutoWidth.emplace_back(al);

                if (writeData) {

                    if (ViewTextVec.size() + ViewTextBeginLine <= lineNumber)
                        ViewTextVec.resize(1 + lineNumber - ViewTextBeginLine);
                    ViewLine vl;
                    vl.subLine = al;
                    vl.realLineNumber = lineNumber;
                    vl.subLineNumber = lineStruct.lineTextListAutoWidth.size() - 1;
                    vl.realLineOffset = realLineOffset;
                    vl.lineCharCount = i - oldI;
                    if (ViewTextVec[lineNumber - ViewTextBeginLine].size() <= vl.subLineNumber)
                        ViewTextVec[lineNumber - ViewTextBeginLine].resize(1 + vl.subLineNumber);
                    ViewTextVec[lineNumber - ViewTextBeginLine][vl.subLineNumber] = vl;
                }


                oldI = i;
                lineStruct.lineHeight += lineMaxHeight;
                if (isFirstLine) {
                    this->firstLineHeight = lineMaxHeight;
                }

            }
            lineMaxHeight = 0;
        }

    }

    if (widthLineStart < w) {
        std::string cutStr(widthLineStart, w - widthLineStart - 1);
        lineMaxHeight = std::max(lineMaxHeight,
                                 double(this->font->fm.ascent + this->font->fm.descent + this->font->fm.lineGap));
        if (false) {

        } else {

            int endCount = 0;
            int len2 = cutStr.length();
            const char *twochars1 = &cutStr[0];
            const char *w = &cutStr[0];
            for (;; endCount++) {
                char32_t codepoint = utf8::next(w, twochars1 + 1 + len2);
                if (codepoint == 0) {
                    break;
                }

            }
            AutoChangeLineStruct al;
            al.lineHight = lineMaxHeight;
            al.lineStr = cutStr;
            al.realLineOffset = widthLineStart - twochars;
            lineStruct.lineTextListAutoWidth.emplace_back(al);
            if (writeData) {
                if (ViewTextVec.size() + ViewTextBeginLine <= lineNumber)
                    ViewTextVec.resize(1 + lineNumber - ViewTextBeginLine);
                ViewLine vl;
                vl.subLine = al;
                vl.realLineNumber = lineNumber;
                vl.subLineNumber = lineStruct.lineTextListAutoWidth.size() - 1;
                vl.realLineOffset = widthLineStart - twochars1;
                vl.lineCharCount = endCount;
                if (ViewTextVec[lineNumber - ViewTextBeginLine].size() <= vl.subLineNumber)
                    ViewTextVec[lineNumber - ViewTextBeginLine].resize(1 + vl.subLineNumber);
                ViewTextVec[lineNumber - ViewTextBeginLine][vl.subLineNumber] = vl;

            }
            lineStruct.lineHeight += lineMaxHeight;
            if (isFirstLine) {
                this->firstLineHeight = lineMaxHeight;
            }
        }
        lineMaxHeight = 0;

    }


    return lineStructIt;
}


void TextLayout::loadString(const std::string &str) {
    this->lineTextList = split(str, '\n');
    this->cacheLineIterator = this->lineTextList.begin();
}

void TextLayout::getViewLines() {
    this->yOffset;
    auto LineIt = this->cacheLineIterator;
    double proHeight = 0;

    int lineNumber = cacheLineNumber;

    double heightSum = 0;
    int viewBegin = 0;
    for (; LineIt != this->lineTextList.end(); LineIt++, lineNumber++) {
        int subLineNumber = 0;
        for (auto &E :LineIt->lineTextListAutoWidth) {
            if (heightSum + E.lineHight > yOffset)
                viewBegin = subLineNumber;
            subLineNumber++;
        }
    }


}

std::list<LineStruct>::iterator
TextLayout::pageLayout(int lineBegin, int lineEnd, double offsetX, double offsetY, double &allSize) {
    auto it = this->lineTextList.begin();
    std::list<LineStruct>::iterator TextBeginLine = this->lineTextList.begin();

    for (int i = 0; i < lineBegin; i++) {
        it++;
    }
    int realLineBeginNum = lineBegin;

    ViewTextVec.clear();
    ViewTextBeginLine = realLineBeginNum;
    ViewTextCurLine = realLineBeginNum;
    ViewEyeTextBeginLine = -1;
    ViewEyeTextBeginSubLine = -1;

    double proHeight = 0;

    int lineNumber = realLineBeginNum;
    bool breakall = false;


    for (; lineNumber <= lineEnd; lineNumber++) {
        it->lineTextListAutoWidth.clear();
        auto LineIt = this->lineLayout(it, lineNumber, pageW);
        int subLineNumber = 0;
        int viewLinePos = 0;

        int viewEyeTextBeginPos = 0;


        for (auto E:LineIt->lineTextListAutoWidth) {
            proHeight += E.lineHight;
            int viewSubLinePos = E.lineStr.length();

            if (proHeight > offsetY && ViewEyeTextBeginLine == -1) {
                ViewEyeTextBeginLine = lineNumber;
                ViewEyeTextBeginSubLine = subLineNumber;
                this->ViewEyeTextBeginLinePos = viewLinePos;
                this->ViewEyeTextBeginSubLinePos = viewSubLinePos;
                this->viewEyeTextBeginPos.realLineNumber = lineNumber;
                this->viewEyeTextBeginPos.realLineOffset = viewEyeTextBeginPos;
            }
            viewLinePos += viewSubLinePos;
            viewEyeTextBeginPos += viewSubLinePos;


            ViewEyeTextEndLine = lineNumber;
            ViewEyeTextEndSubLine = subLineNumber;
            this->ViewEyeTextEndLinePos = viewLinePos;
            this->ViewEyeTextEndSubLinePos = viewSubLinePos;
            this->viewEyeTextEndPos.realLineNumber = lineNumber;
            this->viewEyeTextEndPos.realLineOffset = viewEyeTextBeginPos;

            if (proHeight > pageH + offsetY) {
                //TODO
                break;
            }
            subLineNumber++;

        }
        if (proHeight > pageH + offsetY) {
            lineNumber++;
            //TODO
            break;
        }

        it++;

    }

    allSize = proHeight;
//    std::cout << "pageLayout Over, eyeView: Line " << ViewEyeTextBeginLine << ":" << ViewEyeTextBeginSubLine
//              << " eyeView: EndLine " << ViewEyeTextEndLine << ":" << ViewEyeTextEndSubLine << std::endl;
//

    return TextBeginLine;
}


std::vector<TextLayout::PosEx>
TextLayout::getPosBySubLinePos(std::vector<int> Line, std::vector<int> TextPos, bool nextBegin,
                               bool LineOffsetPixelXAddWidth) {
    std::vector<Rects> rects;
    Rects head;
    Rects mid;
    Rects end;
    int beginID = -1;
    int EndID = -1;
    int offset = this->yOffset;
    int n = 0;
    double height = 0;
    int pos = 0;
    int oldLineNumber = 0;
    std::vector<TextLayout::PosEx> retVec;
    int pSize = Line.size();
    bool cont = true;
    const char *oldW = NULL;
    auto it = this->cacheLineIterator;


    int beginPos = 0;

    for (int i = 0; i < this->ViewTextVec.size() && cont; i++, it++) {
        auto subIt = it->lineTextListAutoWidth.begin();

        for (int j = 0; j < this->ViewTextVec[i].size() && cont; j++, subIt++) {
            int tinyLine = j;
            ViewLine &vl = ViewTextVec[i][j];
            int len = vl.subLine.lineStr.length();
            if (vl.realLineNumber < this->cacheLineNumber)
                continue;
            if (vl.realLineNumber != Line[n]) {
                height += vl.subLine.lineHight;
                if (oldLineNumber != vl.realLineNumber) {
                    oldLineNumber = vl.realLineNumber;
                    pos = 0;
                }

                continue;
            }

            bool nextSameLine = false;
            do {
                if (n >= pSize) {
                    return retVec;
                }

                if (Line[n] == vl.realLineNumber) {
                    if (pos + len >= TextPos[n]) {


                        auto &lineStr = vl.subLine.lineStr;
                        if (!lineStr.empty()) {
                            const char *twochars = &lineStr[0];
                            const char *widthLineStart = twochars;
                            const char *w = twochars;
                            oldW = w;
                            int len = lineStr.length();
                            double widthSum = 0;
                            bool ok = false;
                            int i2 = 0;
                            bool breakTag = false;
                            for (;; i2++) {

                                char32_t codepoint = utf8::next(w, twochars + 1 + len);
                                int64_t tlCodePoint = 0;
                                if (codepoint == 0) {
                                    break;
                                }
                                memcpy(&tlCodePoint, &codepoint, 4);
                                double width = this->textWidthMap[tlCodePoint];
                                if ((int) (w - oldW + pos) >= TextPos[n]) {
                                    if (TextPos[n] == 0) {
                                        TextLayout::PosEx p;
                                        p.LineOffsetPixelY = height;
                                        p.LineOffsetPixelX = widthSum;
                                        p.pos.realLineNumber = Line[n];
                                        p.pos.realLineOffset = TextPos[n];

                                        p.pos.subLineOffset = TextPos[n] - subIt->realLineOffset;
                                        p.pos.subLineNumber = j;
                                        p.LineHeightPixel = vl.subLine.lineHight;
                                        retVec.push_back(p);
                                        height += vl.subLine.lineHight;

                                        n++;
                                        break;
                                    } else {
                                        if (nextBegin) {
                                            widthSum += width;
                                            TextLayout::PosEx p;
                                            p.LineHeightPixel = vl.subLine.lineHight;
                                            p.LineOffsetPixelY = height + vl.subLine.lineHight;
                                            p.LineOffsetPixelX = 0;

                                            p.pos.realLineNumber = Line[n];
                                            p.pos.realLineOffset = TextPos[n];
                                            auto it2 = subIt;
                                            it2++;
                                            p.pos.subLineOffset = 0;
                                            p.pos.subLineNumber = j + 1;
                                            p.pos.isBegin = true;
                                            p.LineHeightPixel = vl.subLine.lineHight;
                                            retVec.push_back(p);
                                            height += vl.subLine.lineHight;

                                            n++;
                                        } else {
                                            widthSum += width;
                                            TextLayout::PosEx p;
                                            p.LineHeightPixel = vl.subLine.lineHight;

                                            p.LineOffsetPixelY = height;

                                            p.LineOffsetPixelX = widthSum;
                                            p.pos.realLineNumber = Line[n];
                                            p.pos.realLineOffset = TextPos[n];
                                            p.pos.subLineNumber = j;
                                            p.pos.subLineOffset = TextPos[n] - subIt->realLineOffset;

                                            p.LineHeightPixel = vl.subLine.lineHight;

                                            retVec.push_back(p);
                                            height += vl.subLine.lineHight;

                                            n++;
                                        }


                                        break;
                                    }


                                }
                                widthSum += width;

                            }


                        } else {
                            TextLayout::PosEx p;
                            p.LineHeightPixel = vl.subLine.lineHight;
                            p.LineOffsetPixelY = height;
                            p.LineOffsetPixelX = 0;
                            p.pos.realLineNumber = Line[n];
                            p.pos.realLineOffset = TextPos[n];
                            p.pos.subLineOffset = TextPos[n] - subIt->realLineOffset;
                            p.pos.subLineNumber = j;
                            p.LineHeightPixel = vl.subLine.lineHight;
                            retVec.push_back(p);
                            height += vl.subLine.lineHight;
                        }


                    } else {
                        pos += len;
                        height += vl.subLine.lineHight;
                        continue;
                    }
                }
            } while (nextSameLine);
            if (n >= pSize) {
                return retVec;
            }

        }
    }
    if (retVec.empty()) {

    }
    return retVec;
}

void TextLayout::pageMovePixel(double pixel) {
    if (pixel < 0) {
        if (yOffset >= -pixel) {
            yOffset += pixel;
        } else {
            if (cacheLineNumber == 0) {
                return;
            }
            this->cacheLineIterator--;
            this->cacheLineNumber--;
            ViewTextBeginLine = 0;
            auto preLineIt = lineLayout(this->cacheLineIterator, 0, this->pageW);//TODO +1line
            double pageOffsetY = std::max(preLineIt->lineHeight + pixel, 0.0);

            double allSize = 0;
            auto lineIt = pageLayout(this->cacheLineNumber, this->lineTextList.size() - 1, this->xOffset, pageOffsetY,
                                     allSize);//TODO +1 end line
            this->allSize = allSize;


            this->yOffset = pageOffsetY;
        }
    } else if (pixel > 0) {
        if (allSize - yOffset < pageH) {
            return;
        }
        if (yOffset + pixel > cacheLineIterator->lineHeight) {

            yOffset = yOffset + pixel - cacheLineIterator->lineHeight;
            this->cacheLineIterator++;
            this->cacheLineNumber++;
        } else {
            ViewTextBeginLine = 0;
            auto preLineIt = lineLayout(this->cacheLineIterator, 0, this->pageW);//TODO +1line
            double pageOffsetY = std::max(this->yOffset + pixel, 0.0);


            double allSize = 0;
            auto lineIt = pageLayout(this->cacheLineNumber, this->lineTextList.size() - 1, this->xOffset, pageOffsetY,
                                     allSize);//TODO +1 end line
            this->allSize = allSize;
            this->yOffset = pageOffsetY;
        }
    }


    double allSize = 0;
    this->pageLayout(this->cacheLineNumber, this->lineTextList.size() - 1, 0, this->yOffset,
                     allSize);
    this->allSize = allSize;


    {
        auto hitPos = this->hitPos1;
        std::vector<int> Line{hitPos.pos.realLineNumber};
        std::vector<int> TextPos{hitPos.pos.realLineOffset};
        std::vector<int> subLine{hitPos.pos.subLineNumber};
        std::vector<int> subTextPos{hitPos.pos.subLineOffset};
        std::vector<double> LineHeightPixel{hitPos.LineHeightPixel};

        cursorPos = hitPos.pos.realLineOffset;
        auto pos = getPosBySubLinePos(Line, TextPos, hitPos.pos.isBegin);
        this->canSeeBeginCursor = pos.empty();


        if (Line[0] >= this->cacheLineNumber && Line[0] <= this->ViewEyeTextEndLine) {
            pos[0].pos.subLineOffset = subTextPos[0];
            this->hitPos1 = pos[0];
        }


    }
    {
        auto hitPos = this->hitPos2;
        std::vector<int> Line{hitPos.pos.realLineNumber};
        std::vector<int> TextPos{hitPos.pos.realLineOffset};
        std::vector<int> subTextPos{hitPos.pos.subLineOffset};
        bool NextBegin = false;
        cursorPos = hitPos.pos.realLineOffset;
        auto pos = getPosBySubLinePos(Line, TextPos, hitPos.pos.isBegin);
        this->canSeeEndCursor = pos.empty();
        if (Line[0] >= this->cacheLineNumber && Line[0] <= this->ViewEyeTextEndLine) {
            pos[0].pos.subLineOffset = subTextPos[0];
            this->hitPos2 = pos[0];
        }
    }
}

TextLayout::PosEx TextLayout::getPos(double x, double y) {
    bool hit = false;
    double head = 0;
    bool overEnd = false;
    auto it = cacheLineIterator;
    double viewOffsetY = 0;

    PosEx hitPos;
    hitPos.pos.realLineNumber = -1;
    hitPos.pos.subLineNumber = -1;
    hitPos.pos.realLineOffset = -1;
    hitPos.pos.subLineOffset = -1;
    hitPos.LineOffsetPixelX = -1;
    hitPos.LineOffsetPixelY = -1;
    hitPos.LineHeightPixel = -1;
    int miniLineNumber = 0;
    int miniLineHitNumber = -1;
    int RealLineNumber = this->cacheLineNumber;
    int textPos = 0;

    std::list<AutoChangeLineStruct>::iterator tinyCurIt;
    do {
        if (it != this->lineTextList.end()) {
            head = it->lineHeight - this->yOffset;
            if (head < 0) {

            }
            if (y <= head) {
                hit = true;
                double miniLineHeightSum = 0;
                miniLineNumber = 0;
                for (tinyCurIt = it->lineTextListAutoWidth.begin();
                     tinyCurIt !=
                     it->lineTextListAutoWidth.end(); textPos += tinyCurIt->lineStr.length(), tinyCurIt++, miniLineNumber++) {
                    if (y <= miniLineHeightSum + tinyCurIt->lineHight - this->yOffset) {
                        hitPos.pos.realLineNumber = RealLineNumber;

                        hitPos.pos.subLineNumber = miniLineNumber;
                        hitPos.LineOffsetPixelY = miniLineHeightSum - this->yOffset;
                        hitPos.LineHeightPixel = tinyCurIt->lineHight;
                        hitPos.pos.realLineOffset = textPos;
                        hit = true;
                        break;
                    }
                    miniLineHeightSum += tinyCurIt->lineHight;
                }
            } else {
                viewOffsetY += head;
                it++;
                RealLineNumber++;
            }
        }
        if (!hit) {
            bool breakAll = false;
            for (; it != this->lineTextList.end(); it++, RealLineNumber++) {
                if (y <= it->lineHeight + viewOffsetY) {
                    hit = true;
                    double miniLineHeightSum = 0;
                    miniLineNumber = 0;
                    for (tinyCurIt = it->lineTextListAutoWidth.begin();
                         tinyCurIt !=
                         it->lineTextListAutoWidth.end(); textPos += tinyCurIt->lineStr.length(), tinyCurIt++, miniLineNumber++) {
                        auto sum = miniLineHeightSum + tinyCurIt->lineHight + viewOffsetY;
                        if (y <= sum) {

                            hitPos.pos.realLineNumber = RealLineNumber;
                            hitPos.pos.subLineNumber = miniLineNumber;
                            hitPos.LineOffsetPixelY = miniLineHeightSum + viewOffsetY;
                            hitPos.LineHeightPixel = tinyCurIt->lineHight;
                            hitPos.pos.realLineOffset = textPos;

                            breakAll = true;
                            break;
                        }

                        miniLineHeightSum += tinyCurIt->lineHight;
                    }
                    break;
                } else {

                    viewOffsetY += it->lineHeight;
                }

            }
        }

    } while (0);

    if (!hit) {
        overEnd = true;
    }
    if (overEnd) {
        if (!this->lineTextList.empty() && !this->lineTextList.rbegin()->lineTextListAutoWidth.empty()) {
            miniLineNumber = this->lineTextList.rbegin()->lineTextListAutoWidth.size() - 1;
            tinyCurIt = this->lineTextList.begin()->lineTextListAutoWidth.begin();
            for (int i = 0; i < this->lineTextList.begin()->lineTextListAutoWidth.size() - 1; i++, tinyCurIt++);
        } else {

        }
    }

    if (it != this->lineTextList.end() && tinyCurIt != it->lineTextListAutoWidth.end()) {
        auto &lineStr = tinyCurIt->lineStr;
        if (!lineStr.empty()) {
            const char *twochars = &lineStr[0];
            const char *widthLineStart = twochars;
            const char *w = twochars;
            const char *oldW = w;
            int len = lineStr.length();
            double widthSum = 0;
            bool ok = false;
            int i2 = 0;
            const char *curW = NULL;
            for (;; i2++) {
                const char *preW = w;
                char32_t codepoint = utf8::next(w, twochars + 1 + len);
                int64_t tlCodePoint = 0;
                if (codepoint == 0) {
                    break;
                }
                curW = w;
                memcpy(&tlCodePoint, &codepoint, 4);
                double width = this->textWidthMap[tlCodePoint];
                if (-this->xOffset+x <= widthSum + width / 2.0) {
                    hitPos.LineOffsetPixelX = widthSum;
                    hitPos.pos.subLineOffset = preW - oldW;
                    textPos += (preW - oldW);
                    hitPos.pos.realLineOffset = textPos;
                    ok = true;
                    break;
                } else {
                    widthSum += width;
                }
            }

            if (!ok) {
                hitPos.LineOffsetPixelX = widthSum;
                hitPos.subLineU8offset = i2;
                hitPos.pos.subLineOffset = curW - oldW;
                hitPos.pos.realLineOffset = textPos + curW - oldW;
            }
        } else {
            hitPos.LineOffsetPixelX = 0;
            hitPos.pos.subLineOffset = 0;
        }
    } else {
        hitPos.LineOffsetPixelX = 0;
        hitPos.pos.subLineOffset = 0;
    }


    return hitPos;
}

void TextLayout::onLButtonDown(double x, double y, bool inner) {

    this->press = true;
    this->SelectVec.clear();
    TextLayout::PosEx hitPos = getPos(x, y);
    if (hitPos.pos.realLineNumber == -1)
        return;

    this->blink_time = 1;
    std::vector<int> Line{hitPos.pos.realLineNumber};
    std::vector<int> TextPos{hitPos.pos.realLineOffset};
    std::vector<int> subLine{hitPos.pos.subLineNumber};
    std::vector<int> subTextPos{hitPos.pos.subLineOffset};
    std::vector<double> LineHeightPixel{hitPos.LineHeightPixel};
    bool NextBegin = false;
    cursorPos = hitPos.pos.realLineOffset;
    if (hitPos.pos.subLineOffset == 0 && hitPos.pos.subLineNumber != 0) {
        hitPos.pos.isBegin = true;
    } else {
        hitPos.pos.isBegin = false;
    }

    auto pos = getPosBySubLinePos(Line, TextPos, hitPos.pos.isBegin);
    pos[0].pos.subLineNumber = subLine[0];
    pos[0].pos.subLineOffset = subTextPos[0];
    pos[0].LineHeightPixel = LineHeightPixel[0];

    this->hitPos1 = pos[0];
    this->hitPos2 = pos[0];
    TextLayout::Pos pos1;
    pos1.realLineNumber = pos[0].pos.realLineNumber;
    pos1.realLineOffset = pos[0].pos.realLineOffset;
    pos1.subLineNumber = pos[0].pos.subLineNumber;
    pos1.subLineOffset = pos[0].pos.subLineOffset;
    pos1.isBegin = pos[0].pos.isBegin;

    TextLayout::Pos pos2 = pos1;
    this->SelectVec.push_back(std::pair(pos1, pos2));

    if (!inner) {
        lastCursorXPixer = this->hitPos1.LineOffsetPixelX;
    }
    std::cout << "onLButtonDown, Line " << hitPos.pos.realLineNumber << ":" << hitPos.pos.subLineNumber << ":"
              << hitPos.pos.realLineOffset << std::endl;


}

void TextLayout::onLButtonUp(double x, double y) {
    this->press = false;
}

void TextLayout::onMove(double x, double y) {

}


void TextLayout::onDrag(double x, double y) {
    tag:

    TextLayout::PosEx hitPos = getPos(std::min(this->pageW, x), std::min(this->pageH, y));
    if (hitPos.pos.realLineNumber == -1)
        return;
    this->blink_time = 1;
    std::vector<int> Line{hitPos.pos.realLineNumber};
    std::vector<int> TextPos{hitPos.pos.realLineOffset};
    std::vector<int> subLine{hitPos.pos.subLineNumber};
    std::vector<int> subTextPos{hitPos.pos.subLineOffset};
    std::vector<double> LineHeightPixel{hitPos.LineHeightPixel};
    bool NextBegin = false;
    cursorPos = hitPos.pos.realLineOffset;
    if (hitPos.pos.subLineOffset == 0 && hitPos.pos.subLineNumber != 0) {
        hitPos.pos.isBegin = true;
    } else {
        hitPos.pos.isBegin = false;
    }
    std::vector<TextLayout::PosEx> pos;

    pos = getPosBySubLinePos(Line, TextPos, hitPos.pos.isBegin);
    if (pos.empty()) {
        goto tag;
    }
    pos[0].pos.subLineOffset = subTextPos[0];
    this->hitPos2 = pos[0];
    if(!autoChangeLine)
    {
        if(x > this->pageW){
            int val = 3;
//        int absVal = std::abs(val);
//        for (int i = 0; i < absVal; i++) {
//            this->pageMovePixel(-val / absVal * 7);
//        }
            xOffset-=val;
        }
        if(x < 0){
            int val = 3;
//        int absVal = std::abs(val);
//        for (int i = 0; i < absVal; i++) {
//            this->pageMovePixel(-val / absVal * 7);
//        }
            xOffset+=val;
        }
    }
    if(y > this->pageH){
        int val = -3;
        int absVal = std::abs(val);
        for (int i = 0; i < absVal; i++) {
            this->pageMovePixel(-val / absVal * 7);
            if(this->yOffset < 7){
                this->pageMovePixel(-this->yOffset);
            }
        }
    }
    if(y < 0){
        int val = 3;
        int absVal = std::abs(val);
        for (int i = 0; i < absVal; i++) {
            this->pageMovePixel(-val / absVal * 7);
            if(this->yOffset < 7){
                this->pageMovePixel(-this->yOffset);
            }
        }
    }


//	std::cout << "onDrag, Line " << hitPos.pos.realLineNumber << ":" << hitPos.pos.subLineNumber << ":"
//			  << hitPos.pos.realLineOffset
//			  << std::endl;
}

void TextLayout::modifyText(int lineBegin, int lineEnd, int beginPos, int endPos, std::list<LineStruct> &insertText) {
    auto it = lineTextList.begin();
    for (int i = 0; i < lineBegin; i++) {
        it++;
    }
    if (lineBegin == lineEnd) {
        std::string strBegin = it->lineStr.substr(0, beginPos);
        std::string lineRightStr = it->lineStr.substr(endPos, it->lineStr.length() - endPos);

        if (insertText.empty()) {
            LineStruct be;

            be.lineStr = strBegin + lineRightStr;
            insertText.push_back(be);
        } else {
            insertText.begin()->lineStr = strBegin + insertText.begin()->lineStr;
            {
                std::string &insertEndLineStr = insertText.rbegin()->lineStr;

                insertEndLineStr += lineRightStr;

            }
        }


        it = lineTextList.erase(it++);

        lineTextList.insert(it, insertText.begin(), insertText.end());

        it--;
        cacheLineIterator = lineTextList.begin();
		for (int i = 0; i < cacheLineNumber; i++) {
			cacheLineIterator++;
		}
		//if(lineBegin == cacheLineNumber)
		{
//			cacheLineIterator = it;
		}
    } else {
        std::string strBegin = it->lineStr.substr(0, beginPos);

        if (insertText.empty()) {
            LineStruct be;
            be.lineStr = strBegin;
            insertText.push_back(be);
            for (int lineNumber = lineBegin; lineNumber <= lineEnd - 1; lineNumber++) {
                it = lineTextList.erase(it++);
            }

            std::string lineRightStr = it->lineStr.substr(endPos, it->lineStr.length() - endPos);
            std::string &insertEndLineStr = insertText.rbegin()->lineStr;
            insertEndLineStr += lineRightStr;
            it = lineTextList.erase(it++);

            lineTextList.insert(it--, insertText.begin(), insertText.end());
        } else {

            insertText.begin()->lineStr = strBegin + insertText.begin()->lineStr;
            for (int lineNumber = lineBegin; lineNumber <= lineEnd - 1; lineNumber++) {
                it = lineTextList.erase(it++);
            }

            std::string lineRightStr = it->lineStr.substr(endPos, it->lineStr.length() - endPos);
            std::string &insertEndLineStr = insertText.rbegin()->lineStr;
            insertEndLineStr += lineRightStr;
            it = lineTextList.erase(it++);

            lineTextList.insert(it--, insertText.begin(), insertText.end());
        }

    }


}

LineStruct *TextLayout::getLineStruct(int lineNumber) {
    auto it = lineTextList.begin();
    for (int i = 0; i < lineNumber; i++) {
        it++;
    }
    return &*it;
}

std::list<LineStruct>::iterator TextLayout::getLineIt(int lineNumber) {
    auto it = lineTextList.begin();
    for (int i = 0; i < lineNumber; i++) {
        it++;
    }
    return it;
}

void TextLayout::pageMoveSubLine(int lineCount, double offsetX, double _offsetY) {

    if (lineCount < 0) {

        double offsetY = 0;
        bool breakall = false;
        double lastHeight = 0;
        for (int i = 0; i < this->ViewTextVec.size(); i++) {
            for (int j = 0; j < this->ViewTextVec[i].size(); j++) {
                if (ViewTextVec[i][j].realLineNumber == this->ViewEyeTextBeginLine &&
                    ViewTextVec[i][j].subLineNumber == this->ViewEyeTextBeginSubLine) {
                    breakall = true;
                    break;
                }
                offsetY += ViewTextVec[i][j].subLine.lineHight;
                lastHeight = ViewTextVec[i][j].subLine.lineHight;
            }
            if (breakall)
                break;
        }
        if (offsetY > 0) {
            this->pageLayout(this->cacheLineNumber, this->lineTextList.size() - 1, 0, offsetY - lastHeight,
                             allSize);
            this->yOffset = offsetY - lastHeight;
        } else {
            auto lineNumber = cacheLineNumber - 1;
            auto it = cacheLineIterator;
            it--;

            ViewTextVec.clear();
            ViewTextBeginLine = lineNumber;
            ViewTextCurLine = lineNumber;
            ViewEyeTextBeginLine = -1;
            ViewEyeTextBeginSubLine = -1;
            auto preLineIt = lineLayout(it, lineNumber, this->pageW);
            double offsetY2 = preLineIt->lineHeight - preLineIt->lineTextListAutoWidth.rbegin()->lineHight;
            this->cacheLineNumber--;
            this->cacheLineIterator--;
            this->pageLayout(this->cacheLineNumber, this->lineTextList.size() - 1, 0, offsetY2,
                             allSize);
            this->yOffset = offsetY2;
        }
    } else if (lineCount > 0) {
        double offsetY = 0;
        bool nextOnce = false;
        bool breakall = false;
        double lastHeight = 0;
        int nextLinei = -1;
        int nextLinej = -1;
        for (int i = 0; i < this->ViewTextVec.size(); i++) {
            for (int j = 0; j < this->ViewTextVec[i].size(); j++) {
                if (ViewTextVec[i][j].realLineNumber == this->ViewEyeTextEndLine &&
                    ViewTextVec[i][j].subLineNumber == this->ViewEyeTextEndSubLine) {
                    nextOnce = true;
                    continue;
                }
                if (nextOnce) {
                    nextLinei = i;
                    nextLinej = j;
                    breakall = true;
                    break;
                }

            }
            if (breakall) {
                break;
            }
        }
        if (breakall && this->yOffset + ViewTextVec[nextLinei][nextLinej].subLine.lineHight > 0) {
            this->yOffset += ViewTextVec[nextLinei][nextLinej].subLine.lineHight;
            if (this->yOffset > 0) {
                this->pageLayout(this->cacheLineNumber, this->lineTextList.size() - 1, 0, this->yOffset,
                                 allSize);
            } else {

            }
        } else {
            auto lineNumber = this->ViewEyeTextEndLine + 1;
            auto it = cacheLineIterator;
            for (int i = 0; i < this->ViewEyeTextEndLine + 1 - cacheLineNumber; i++)
                it++;

            ViewTextVec.clear();
            ViewTextBeginLine = lineNumber;
            ViewTextCurLine = lineNumber;
            ViewEyeTextBeginLine = -1;
            ViewEyeTextBeginSubLine = -1;
            auto preLineIt = lineLayout(it, lineNumber, this->pageW);
            double offsetY2 = preLineIt->lineTextListAutoWidth.begin()->lineHight;

            this->yOffset += offsetY2;
            this->pageLayout(this->cacheLineNumber, this->lineTextList.size() - 1, 0, this->yOffset,
                             allSize);
        }
    }
}

void TextLayout::ViewToPos(int lineBegin, int lineOffset, int nextBegin) {
    double offsetY = 0.0;
    auto it = this->lineTextList.begin();
    std::list<LineStruct>::iterator TextBeginLine = this->lineTextList.begin();

    for (int i = 0; i < lineBegin; i++) {
        it++;
    }
    int realLineBeginNum = lineBegin;

    ViewTextVec.clear();
    ViewTextBeginLine = realLineBeginNum;
    ViewTextCurLine = realLineBeginNum;
    ViewEyeTextBeginLine = -1;
    ViewEyeTextBeginSubLine = -1;

    double proHeight = 0;
    int offset = 0;
    int lineNumber = realLineBeginNum;
    bool breakall = false;
    double beginOffsetY = 99999;

    bool end = false;
    for (; lineNumber <= this->lineTextList.size() - 1; lineNumber++) {
        it->lineTextListAutoWidth.clear();
        auto LineIt = this->lineLayout(it, lineNumber, pageW);
        int subLineNumber = 0;
        int viewLinePos = 0;

        int viewEyeTextBeginPos = 0;


        for (auto E:LineIt->lineTextListAutoWidth) {
            proHeight += E.lineHight;
            offset += E.lineStr.length();
            int viewSubLinePos = E.lineStr.length();


            if (!end) {
                if (offset >= lineOffset && ViewEyeTextBeginLine == -1) {
                    if (nextBegin) {
                        nextBegin = false;
                        subLineNumber++;
                        continue;
                    }

                    this->cacheLineNumber = lineNumber;
                    this->cacheLineIterator = it;
                    beginOffsetY = proHeight - E.lineHight;
                    this->yOffset = beginOffsetY;
                    ViewEyeTextBeginLine = lineNumber;
                    ViewEyeTextBeginSubLine = subLineNumber;
                    this->ViewEyeTextBeginLinePos = viewLinePos;
                    this->ViewEyeTextBeginSubLinePos = viewSubLinePos;
                    this->viewEyeTextBeginPos.realLineNumber = lineNumber;
                    this->viewEyeTextBeginPos.realLineOffset = viewEyeTextBeginPos;


                }
                viewLinePos += viewSubLinePos;
                viewEyeTextBeginPos += viewSubLinePos;
                ViewEyeTextEndLine = lineNumber;
                ViewEyeTextEndSubLine = subLineNumber;
                this->ViewEyeTextEndLinePos = viewLinePos;
                this->ViewEyeTextEndSubLinePos = viewSubLinePos;
                this->viewEyeTextEndPos.realLineNumber = lineNumber;
                this->viewEyeTextEndPos.realLineOffset = viewEyeTextBeginPos;
            }
            if (proHeight - beginOffsetY > pageH + offsetY && end == false) {
                end = true;
            }
            subLineNumber++;

        }
        if (end) {
            lineNumber++;
            //TODO
            break;
        }

        it++;

    }

    allSize = proHeight;
//	std::cout << "pageLayout Over, eyeView: Line " << ViewEyeTextBeginLine << " " << ViewEyeTextBeginSubLine
//			  << " eyeView: EndLine " << ViewEyeTextEndLine << " " << ViewEyeTextEndSubLine << " " << yOffset
//			  << std::endl;
}

void TextLayout::ViewToPos2(int lineEnd, int lineOffset, int nextBegin) {

    auto it = this->lineTextList.begin();
    std::list<LineStruct>::iterator TextBeginLine = this->lineTextList.begin();

    for (int i = 0; i < lineEnd; i++) {
        it++;
    }
    int offset = 0;
    int lineNumber = lineEnd;
    double endSubHeightSum = 0;

    it->lineTextListAutoWidth.clear();
    auto LineIt = this->lineLayout(it, lineNumber, pageW, false);
    for (auto E:LineIt->lineTextListAutoWidth) {
        offset += E.lineStr.length();
        endSubHeightSum += E.lineHight;
        if (offset >= lineOffset) {
            if (nextBegin) {
                nextBegin = false;
                continue;
            }

            break;
        }
    }
    double offsetY = -1.0;
    int lineBegin = 0;
    auto beginit = it;
    if (endSubHeightSum >= this->pageH) {
        lineBegin = lineEnd;
        offsetY = endSubHeightSum - this->pageH;
        lineBegin = lineNumber;
        beginit = it;
    } else {
        lineNumber--;
        it--;

        double proHeight = 0.0;


        for (; lineNumber >= 0; lineNumber--, it--) {
            it->lineTextListAutoWidth.clear();


            auto LineIt = this->lineLayout(it, lineNumber, pageW, false);
            proHeight += it->lineHeight;
            if (proHeight + endSubHeightSum > this->pageH) {
                offsetY = proHeight + endSubHeightSum - this->pageH;
                lineBegin = lineNumber;
                beginit = it;
                //std::cout <<lineNumber <<" " << proHeight <<" " << endSubHeightSum << this->pageH;
                break;
            }
        }
    }

    double _allSize = 0;

    if (offsetY == -1) {
        this->cacheLineNumber = 0;
        this->cacheLineIterator = this->lineTextList.begin();
        pageLayout(0, this->lineTextList.size() - 1, 0.0, 0, _allSize);
        this->yOffset = 0;
    } else {
        this->cacheLineNumber = lineBegin;
        this->cacheLineIterator = beginit;
        pageLayout(lineBegin, lineEnd, 0.0, offsetY, _allSize);
        this->yOffset = offsetY;
    }

    this->allSize = _allSize;
    std::cout << this->viewEyeTextEndPos.realLineNumber << ":" << this->viewEyeTextEndPos.realLineOffset << std::endl;


    {
        auto hitPos = this->hitPos1;
        std::vector<int> Line{hitPos.pos.realLineNumber};
        std::vector<int> TextPos{hitPos.pos.realLineOffset};
        std::vector<int> subLine{hitPos.pos.subLineNumber};
        std::vector<int> subTextPos{hitPos.pos.subLineOffset};
        std::vector<double> LineHeightPixel{hitPos.LineHeightPixel};

        cursorPos = hitPos.pos.realLineOffset;
        auto pos = getPosBySubLinePos(Line, TextPos, hitPos.pos.isBegin);
        this->canSeeBeginCursor = pos.empty();


        if (Line[0] >= this->cacheLineNumber && Line[0] <= this->ViewEyeTextEndLine) {
            pos[0].pos.subLineOffset = subTextPos[0];
            this->hitPos1 = pos[0];
        }


    }
    {
        auto hitPos = this->hitPos2;
        std::vector<int> Line{hitPos.pos.realLineNumber};
        std::vector<int> TextPos{hitPos.pos.realLineOffset};
        std::vector<int> subTextPos{hitPos.pos.subLineOffset};
        bool NextBegin = false;
        cursorPos = hitPos.pos.realLineOffset;
        auto pos = getPosBySubLinePos(Line, TextPos, hitPos.pos.isBegin);
        this->canSeeEndCursor = pos.empty();
        if (Line[0] >= this->cacheLineNumber && Line[0] <= this->ViewEyeTextEndLine) {
            pos[0].pos.subLineOffset = subTextPos[0];
            this->hitPos2 = pos[0];
        }
    }
}

std::vector<TextLayout::PosEx> TextLayout::parseSelector(int move, bool nextBegin) {
    std::vector<int> selectors = getViewSelector();
    bool jump = false;
    if (selectors.size() == 0) {
        selectors.push_back(this->SelectVec.size() - 1);
    }

    if (this->SelectVec[selectors[0]].first < this->viewEyeTextBeginPos) {
        jump = true;
        ViewToPos(this->SelectVec[selectors[0]].first.realLineNumber,
                  this->SelectVec[selectors[0]].first.realLineOffset, nextBegin);
    }
    if (this->SelectVec[selectors[0]].first > this->viewEyeTextEndPos) {
        jump = true;
        ViewToPos2(this->SelectVec[selectors[0]].first.realLineNumber,
                   this->SelectVec[selectors[0]].first.realLineOffset, nextBegin);
    }
    std::vector<TextLayout::PosEx> posret;
    for (auto id: selectors) {
        {
            TextLayout::Pos &begin = this->SelectVec[id].first;
            std::vector<int> lvec;
            lvec.push_back(begin.realLineNumber);
            std::vector<int> posvec;
            posvec.push_back(begin.realLineOffset);
            auto pos = getPosBySubLinePos(lvec, posvec, nextBegin);
            this->MoveOffsetTag = -1;
            this->canSeeBeginCursor = pos.empty();
            this->hitPos1 = pos[0];
            if (move < 0 && this->hitPos1.LineOffsetPixelY < this->yOffset &&
                this->yOffset < this->hitPos1.LineOffsetPixelY + this->hitPos1.LineHeightPixel) {
                if ((this->ViewEyeTextBeginLine == 0 && this->ViewEyeTextBeginSubLine > 0) ||
                    this->ViewEyeTextBeginLine != 0) {
                    if (jump == false)
                        ViewToPos(this->SelectVec[selectors[0]].first.realLineNumber,
                                  this->SelectVec[selectors[0]].first.realLineOffset, nextBegin);
                } else {
                    this->hitPos1.LineOffsetPixelY = 0;
                    this->yOffset = 0;
                }

            } else if (move < 0 && this->yOffset >= this->hitPos1.LineOffsetPixelY + this->hitPos1.LineHeightPixel) {
                if (((this->ViewEyeTextBeginLine == 0 && this->ViewEyeTextBeginSubLine > 0) ||
                     this->ViewEyeTextBeginLine != 0)) {
                    pageMoveSubLine(-1, 0.0, 0.0);//光标（或部分）位于屏幕上方
                } else {
                    this->hitPos1.LineOffsetPixelY = 0;
                    this->yOffset = 0;

                }
            }

            if (move > 0 &&
                this->hitPos1.LineOffsetPixelY - this->yOffset + this->hitPos1.LineHeightPixel > this->pageH) {
                double hoffset =
                        (this->hitPos1.LineOffsetPixelY - this->yOffset + this->hitPos1.LineHeightPixel) - this->pageH;
                this->pageMovePixel(hoffset);
            }
        }

        {
            TextLayout::Pos &end = this->SelectVec[id].second;
            std::vector<int> lvec;
            lvec.push_back(end.realLineNumber);
            std::vector<int> posvec;
            posvec.push_back(end.realLineOffset);
            auto pos = getPosBySubLinePos(lvec, posvec, nextBegin);
            this->canSeeEndCursor = pos.empty();
            this->hitPos2 = pos[0];

        }
    }
    return posret;
}

std::vector<int> TextLayout::getViewSelector() {
    std::vector<int> ret;
    for (int i = 0; i < this->SelectVec.size(); i++) {
        if (this->SelectVec[i].second < this->viewEyeTextBeginPos || this->SelectVec[i].first > this->viewEyeTextEndPos)
            continue;

        ret.push_back(i);
    }
    return ret;
}


void TextLayout::right_move(bool inner) {
    this->blink_time = 1;
    bool lessThan = this->hitPos1 < this->hitPos2;

    auto &startPos = lessThan ? this->hitPos1 : this->hitPos2;
    auto &endPos = lessThan ? this->hitPos2 : this->hitPos1;
    bool moveUp = false;
    bool nextBegin = false;
    bool moveUpCrossLine = false;
    int singleChar = 0;
    TextLayout::Pos oldView1;
    TextLayout::Pos oldView2;
    auto &pos = this->SelectVec[0];
    oldView1 = pos.first;
    oldView2 = pos.second;
    auto &posf = this->SelectVec[0].first;
    TextLayout::Pos ret;
    auto it1 = getLineIt(posf.realLineNumber);
    int len = (int) it1->lineStr.length();
    lineLayout(it1, posf.realLineNumber, this->pageW, false);

    bool nextLine = false;
    if (posf.realLineOffset < len) {
        const char *LineoffsetPtr = &it1->lineStr[posf.realLineOffset];
        const char *oldPtr = LineoffsetPtr;

        utf8::advance(LineoffsetPtr, 1, (const char *) &it1->lineStr[0] + len + 1);
        ret.realLineOffset = LineoffsetPtr - &it1->lineStr[0];
        ret.realLineNumber = posf.realLineNumber;
    } else {
        if (posf.realLineNumber + 1 >= this->lineTextList.size()) {
            if (this->SelectVec[0].first < this->viewEyeTextBeginPos) {
                ViewToPos(this->SelectVec[0].first.realLineNumber, this->SelectVec[0].first.realLineOffset,
                          this->SelectVec[0].first.isBegin);
            } else if (this->SelectVec[0].first > this->viewEyeTextEndPos &&
                       this->SelectVec[0].first.subLineNumber < it1->lineTextListAutoWidth.size() - 1) {
                ViewToPos2(this->SelectVec[0].first.realLineNumber, this->SelectVec[0].first.realLineOffset,
                           this->SelectVec[0].first.isBegin);
            }
            parseSelector(1, false);
            if (!inner)
                lastCursorXPixer = this->hitPos1.LineOffsetPixelX;
            return;
        }
        auto it2 = getLineIt(posf.realLineNumber + 1);//TODO it > maxLineNumber;
        ret.realLineOffset = 0;
        ret.realLineNumber = posf.realLineNumber + 1;
        ret.subLineOffset = 0;
        ret.subLineNumber = 0;
        nextLine = true;
    }


    TextLayout::Pos pos1 = ret;
    pos.first = pos1;

    TextLayout::Pos pos2 = pos1;
    pos.second = pos2;


    int sublineNumber_ = 0;
    int subLinerealLineOffset = 0;
    int sublineOffset = 0;
    if (pos1.realLineNumber == startPos.pos.realLineNumber) {
        for (auto &subLine : it1->lineTextListAutoWidth) {
            if (sublineNumber_ == startPos.pos.subLineNumber) {
                if (pos1.realLineOffset <= subLine.realLineOffset + subLine.lineStr.length()) {
                    this->SelectVec[0].first.subLineOffset = pos1.realLineOffset - subLine.realLineOffset;
                    this->SelectVec[0].first.subLineNumber = startPos.pos.subLineNumber;
                    this->SelectVec[0].first.isBegin = false;
                    this->SelectVec[0].second = this->SelectVec[0].first;

                } else if (pos1.realLineOffset > subLine.realLineOffset + subLine.lineStr.length()) {
                    this->SelectVec[0].first.subLineOffset = pos1.realLineOffset - startPos.pos.realLineOffset;
                    this->SelectVec[0].first.subLineNumber = sublineNumber_ + 1;
                    this->SelectVec[0].first.isBegin = false;
                    this->SelectVec[0].second = this->SelectVec[0].first;

                    if (this->SelectVec[0].first < this->viewEyeTextBeginPos) {
                        ViewToPos(this->SelectVec[0].first.realLineNumber, this->SelectVec[0].first.realLineOffset,
                                  this->SelectVec[0].first.isBegin);
                    } else if (this->SelectVec[0].first > this->viewEyeTextEndPos &&
                               sublineNumber_ < it1->lineTextListAutoWidth.size() - 1) {
                        ViewToPos2(this->SelectVec[0].first.realLineNumber, this->SelectVec[0].first.realLineOffset,
                                   this->SelectVec[0].first.isBegin);
                    }
                }
                break;
            }
            sublineNumber_++;
        }


    } else {
        this->SelectVec[0].first.subLineOffset = 0;
        this->SelectVec[0].first.subLineNumber = 0;
        this->SelectVec[0].first.isBegin = false;
        this->SelectVec[0].second = this->SelectVec[0].first;
        if (this->SelectVec[0].first < this->viewEyeTextBeginPos) {
            ViewToPos(this->SelectVec[0].first.realLineNumber, this->SelectVec[0].first.realLineOffset,
                      this->SelectVec[0].first.isBegin);
        } else if (this->SelectVec[0].first > this->viewEyeTextEndPos &&
                   sublineNumber_ < it1->lineTextListAutoWidth.size() - 1) {
            ViewToPos2(this->SelectVec[0].first.realLineNumber, this->SelectVec[0].first.realLineOffset,
                       this->SelectVec[0].first.isBegin);
        }

    }

    parseSelector(1, false);
    if (!inner)
        lastCursorXPixer = this->hitPos1.LineOffsetPixelX;

}


TextLayout::Pos TextLayout::logic_left_move(TextLayout::Pos pos) {
    TextLayout::Pos ret;

    if(pos.realLineOffset == 0 && pos.realLineNumber == 0){
        return pos;
    }

    if (pos.realLineOffset > 0) {
        auto it = getLineIt(pos.realLineNumber);
        lineLayout(it, pos.realLineNumber, this->pageW, false);
        const char *LineoffsetPtr = &it->lineStr[pos.realLineOffset];
        const char *oldPtr = LineoffsetPtr;

        utf8::advance(LineoffsetPtr, -1, (const char *) &it->lineStr[0]);
        ret.realLineOffset = LineoffsetPtr - &it->lineStr[0];
        ret.realLineNumber = pos.realLineNumber;

        return ret;
    }
    auto it = getLineIt(pos.realLineNumber - 1);
    ret.realLineOffset = std::max((int) it->lineStr.length(), 0);//TODO
    ret.realLineNumber = std::max(pos.realLineNumber - 1, 0);


    return ret;
}

void TextLayout::getSubLineNumberAndOffset(TextLayout::Pos pos) {

}

void TextLayout::left_move(bool inner) {
    this->blink_time = 1;
    bool lessThan = this->hitPos1 < this->hitPos2;

    auto &startPos = lessThan ? this->hitPos1 : this->hitPos2;
    auto &endPos = lessThan ? this->hitPos2 : this->hitPos1;
    bool moveUp = false;
    bool nextBegin = false;
    bool moveUpCrossLine = false;
    int singleChar = 0;
    TextLayout::Pos oldView1;
    TextLayout::Pos oldView2;
    for (auto &E:this->SelectVec) {
        oldView1 = E.first;
        TextLayout::Pos pos1 = logic_left_move(E.first);
        E.first = pos1;
        oldView2 = E.second;
        TextLayout::Pos pos2 = logic_left_move(E.second);
        E.second = pos2;
    }
    if (this->SelectVec.size() == 1) {
        //
        bool MoveToSameLine = this->SelectVec[0].first.realLineNumber == startPos.pos.realLineNumber;
        int offsetBytes = (startPos.pos.realLineOffset - this->SelectVec[0].first.realLineOffset);
        if (MoveToSameLine && startPos.pos.subLineNumber > 0 && offsetBytes > 0) {
            if (startPos.pos.subLineOffset - offsetBytes == 0) {
                //移动到了子行的开头 当前Pos在子行的开头，或者上一子行的末尾
                this->SelectVec[0].first.subLineOffset = 0;
                this->SelectVec[0].first.subLineNumber = startPos.pos.subLineNumber;
                this->SelectVec[0].second.subLineOffset = 0;
                this->SelectVec[0].second.subLineNumber = startPos.pos.subLineNumber;
                nextBegin = true;

            }
            if (startPos.pos.subLineOffset > offsetBytes) {
                //子行内部移动
                this->SelectVec[0].first.subLineOffset = startPos.pos.subLineOffset - offsetBytes;
                this->SelectVec[0].first.subLineNumber = startPos.pos.subLineNumber;
                this->SelectVec[0].second.subLineOffset = startPos.pos.subLineOffset - offsetBytes;
                this->SelectVec[0].second.subLineNumber = startPos.pos.subLineNumber;
                nextBegin = false;

            }
            if (startPos.pos.subLineOffset < offsetBytes) {

                //this->SelectVec[0].first.subLineOffset = startPos.pos.subLineOffset - offsetBytes;
                this->SelectVec[0].first.subLineNumber = startPos.pos.subLineNumber - 1;
                this->SelectVec[0].second = this->SelectVec[0].first;

//                    moveUp = true;

                singleChar = offsetBytes - startPos.pos.subLineOffset;
                if (startPos.pos.subLineNumber > 0) {
                    auto it = getLineIt(startPos.pos.realLineNumber);
                    int sublineNumber_ = 0;
                    //lineLayout(it, startPos.pos.realLineNumber, this->pageW,false);

                    for (auto subLine :it->lineTextListAutoWidth) {
                        if (sublineNumber_ == startPos.pos.subLineNumber - 1) {
                            this->SelectVec[0].first.subLineOffset = subLine.lineStr.length() - offsetBytes;
                            if (subLine.lineStr.length() == offsetBytes) {

                                nextBegin = true;
                            }
                            break;
                        }


                        sublineNumber_++;
                    }
                } else {
                    std::abort();
                }


            } else if (startPos.pos.subLineOffset < offsetBytes && startPos.pos.subLineOffset == 0) {

            }
        } else {
            //跳到了上一行的末尾子行，不确定上一行是否多子行
            moveUpCrossLine = true;
        }

        if (this->viewEyeTextBeginPos.realLineOffset == 0 &&
            oldView1.realLineNumber == this->SelectVec[0].first.realLineNumber) {

        } else {
            if (moveUpCrossLine) {

                if (oldView1.realLineNumber > 0 &&
                    oldView1.realLineNumber == this->viewEyeTextBeginPos.realLineNumber &&
                    this->viewEyeTextBeginPos == oldView1) {
                    pageMoveSubLine(-1, 0.0, 0.0);


                    //原来跳到了上一行的末尾子行，不确定上一行是否多子行
                    //借助pageMoveSubLine布局好了上一行的所有子行，进行处理光标所在子行位置以及子行偏移
                    this->SelectVec[0].first.subLineOffset = this->ViewEyeTextBeginSubLinePos;
                    this->SelectVec[0].first.subLineNumber = this->ViewEyeTextBeginSubLine;
                    this->SelectVec[0].second = this->SelectVec[0].first;

                } else {
                    auto it = getLineIt(startPos.pos.realLineNumber - 1);
                    int sublineNumber_ = 0;
                    //lineLayout(it, startPos.pos.realLineNumber, this->pageW,false);

                    for (auto subLine :it->lineTextListAutoWidth) {
                        if (sublineNumber_ == it->lineTextListAutoWidth.size() - 1) {
                            nextBegin = false;

                            this->SelectVec[0].first.subLineOffset = subLine.lineStr.length() - offsetBytes;
                            this->SelectVec[0].first.subLineNumber = sublineNumber_;
                            this->SelectVec[0].first.isBegin = false;
                            this->SelectVec[0].second = this->SelectVec[0].first;
                            break;
                        }
                        sublineNumber_++;
                    }
                }
            }
        }

    }

    parseSelector(-1, nextBegin);
    if (!inner)
        lastCursorXPixer = this->hitPos1.LineOffsetPixelX;
}

TextLayout::Pos TextLayout::logic_up_move(TextLayout::Pos pos) {
    bool overUpViewBox = this->SelectVec[0].first <= this->viewEyeTextBeginPos;
    bool overDownViewBox = this->SelectVec[0].first > this->viewEyeTextEndPos ||
                           (this->SelectVec[0].first == this->viewEyeTextEndPos &&
                            this->SelectVec[0].first.isBegin > this->viewEyeTextEndPos.isBegin);

    auto it = getLineIt(pos.realLineNumber);
    if (overUpViewBox) {
        ViewToPos(this->hitPos1.pos.realLineNumber, this->hitPos1.pos.realLineOffset, this->hitPos1.pos.isBegin);
    } else if (overDownViewBox) {
        ViewToPos2(this->hitPos1.pos.realLineNumber, this->hitPos1.pos.realLineOffset, this->hitPos1.pos.isBegin);
    }
    const char *LineoffsetPtr = &it->lineStr[pos.realLineOffset];
    const char *beginPtr = &it->lineStr[0];


    int lineNumber = pos.realLineNumber;
    //屏幕上已经溢出的情况
    it->lineTextListAutoWidth.clear();
    auto LineIt = this->lineLayout(it, lineNumber, pageW, false);
    int curRealLineOffset = -1;
    int subLineNumber = 0;
    int preRealLineOffset = -1;
    for (auto subLineE : it->lineTextListAutoWidth) {
        preRealLineOffset = curRealLineOffset;
        if (subLineE.realLineOffset + subLineE.lineStr.length() >= (pos.realLineOffset + pos.isBegin * 1)) {
            curRealLineOffset = subLineE.realLineOffset;
            break;
        }

        subLineNumber++;
    }

    int utfLen = 0;
    auto oldPos = this->hitPos1;
    if (subLineNumber > 0) {
        utfLen = utf8::distance(&it->lineStr[curRealLineOffset], &it->lineStr[pos.realLineOffset]);
        if (SelectVec[0].first.subLineOffset >= 0) {
            this->hitPos1.pos.subLineOffset = 0;
            this->hitPos1.pos.realLineOffset = curRealLineOffset;
            this->hitPos1.pos.isBegin = true;
            this->hitPos2 = this->hitPos1;

            SelectVec[0].first.realLineOffset = curRealLineOffset;
            SelectVec[0].first.subLineOffset = 0;
            SelectVec[0].first.isBegin = true;
            SelectVec[0].second = SelectVec[0].first;
        }
        this->left_move(true);
    } else {
        utfLen = utf8::distance(&it->lineStr[0], &it->lineStr[pos.realLineOffset]);

        SelectVec[0].first.realLineOffset = curRealLineOffset;
        SelectVec[0].second = SelectVec[0].first;
        this->left_move(true);
    }

    onLButtonDown(lastCursorXPixer, this->hitPos1.LineOffsetPixelY - this->yOffset + 0.1, true);
    TextLayout::PosEx hitPos = getPos(lastCursorXPixer, this->hitPos1.LineOffsetPixelY - this->yOffset + 0.1);
    std::cout << hitPos.pos.realLineNumber << " " << hitPos.pos.realLineOffset << " " << hitPos.pos.subLineNumber << " "
              << hitPos.pos.subLineOffset << " " << lastCursorXPixer << "pixer" << std::endl;
    return TextLayout::Pos();
}

void TextLayout::up_move() {
    this->blink_time = 1;
    bool lessThan = this->hitPos1 < this->hitPos2;
    auto &startPos = lessThan ? this->hitPos1 : this->hitPos2;
    auto &endPos = lessThan ? this->hitPos2 : this->hitPos1;

    if (this->SelectVec.size() == 1) {

        logic_up_move(this->SelectVec[0].first);
    }
}


TextLayout::Pos TextLayout::logic_down_move(TextLayout::Pos pos) {
    bool overUpViewBox = this->SelectVec[0].first <= this->viewEyeTextBeginPos;
    bool overDownViewBox = this->SelectVec[0].first > this->viewEyeTextEndPos ||
                           (this->SelectVec[0].first == this->viewEyeTextEndPos &&
                            this->SelectVec[0].first.isBegin > this->viewEyeTextEndPos.isBegin);

    auto it = getLineIt(pos.realLineNumber);
    if (overUpViewBox) {
        ViewToPos(this->hitPos1.pos.realLineNumber, this->hitPos1.pos.realLineOffset, this->hitPos1.pos.isBegin);
    } else if (overDownViewBox) {
        ViewToPos2(this->hitPos1.pos.realLineNumber, this->hitPos1.pos.realLineOffset, this->hitPos1.pos.isBegin);
    }
    const char *LineoffsetPtr = &it->lineStr[pos.realLineOffset];
    const char *beginPtr = &it->lineStr[0];


    int lineNumber = pos.realLineNumber;

    it->lineTextListAutoWidth.clear();
    auto LineIt = this->lineLayout(it, lineNumber, pageW, false);
    int target = -1;
    int subLineNumber = 0;
    int subLinelen = -1;
    for (auto subLineE : it->lineTextListAutoWidth) {
        if (subLineNumber == pos.subLineNumber) {
            subLinelen = subLineE.lineStr.length();
            target = subLineE.realLineOffset + subLinelen;
            break;
        }

        subLineNumber++;
    }

    int utfLen = 0;
    auto oldPos = this->hitPos1;
    SelectVec[0].first.realLineNumber = pos.realLineNumber;
    SelectVec[0].first.realLineOffset = target;
    SelectVec[0].first.subLineNumber = pos.subLineNumber;
    SelectVec[0].first.subLineOffset = subLinelen;
    SelectVec[0].first.isBegin = false;
    SelectVec[0].second = SelectVec[0].first;
    this->hitPos1.pos.realLineNumber = pos.realLineNumber;
    this->hitPos1.pos.realLineOffset = target;
    this->hitPos1.pos.subLineNumber = pos.subLineNumber;
    this->hitPos1.pos.subLineOffset = subLinelen;
    this->hitPos1.pos.isBegin = false;
    this->hitPos2 = this->hitPos1;
    this->right_move(true);


    onLButtonDown(lastCursorXPixer, this->hitPos1.LineOffsetPixelY - this->yOffset + 0.1, true);
    return TextLayout::Pos();
}

void TextLayout::down_move() {
    this->blink_time = 1;
    bool lessThan = this->hitPos1 < this->hitPos2;

    auto &startPos = lessThan ? this->hitPos1 : this->hitPos2;
    auto &endPos = lessThan ? this->hitPos2 : this->hitPos1;

    if (this->SelectVec.size() == 1) {
        logic_down_move(this->SelectVec[0].first);
    }
}

void TextLayout::RefreshLineHeight(int beginLine, int endLine, int level) {
    if (level == 0) {
        auto it = getLineIt(beginLine);
        for (int i = beginLine; it != this->lineTextList.end() && i <= endLine; it++) {
            double lineHeight = 0;
            for (auto E : it->lineTextListAutoWidth) {
                E.lineHight = std::max(font_height, E.lineHight);
                lineHeight += E.lineHight;
            }
            it->lineHeight = std::max(lineHeight, font_height);
        }
    }
}

void TextLayout::select_text(Pos pos, int u8offset, int Type/*, bool isbegin*/) {
    if (u8offset < 0) {
        if (pos.realLineOffset > 0) {
            auto it = getLineIt(pos.realLineNumber);
            const char *LineoffsetPtr = &it->lineStr[pos.realLineOffset];
            TextLayout::Pos ret;
            utf8::advance(LineoffsetPtr, -1, (const char *) &it->lineStr[0]);
            ret.realLineOffset = LineoffsetPtr - &it->lineStr[0];
            ret.realLineNumber = pos.realLineNumber;
            this->hitPos1.pos = ret;
            this->hitPos2.pos = pos;
        } else if (pos.realLineNumber > 0) {
            auto it = getLineIt(pos.realLineNumber - 1);
            TextLayout::Pos ret;
            ret.realLineOffset = it->lineStr.length();
            ret.realLineNumber = pos.realLineNumber - 1;
            this->hitPos1.pos = ret;
            this->hitPos2.pos = pos;
        } else {
            this->hitPos1.pos = pos;
            this->hitPos2.pos = pos;
        }
    } else if (u8offset == 0) {
        this->hitPos1.pos = pos;
        this->hitPos2.pos = pos;
    } else if (u8offset > 0) {
        auto it = getLineIt(pos.realLineNumber);
        if (pos.realLineOffset < it->lineStr.length()) {
            const char *LineoffsetPtr = &it->lineStr[pos.realLineOffset];
            TextLayout::Pos ret;
            utf8::advance(LineoffsetPtr, 1, (const char *) &it->lineStr[it->lineStr.length()]);
            ret.realLineOffset = LineoffsetPtr - &it->lineStr[0];
            ret.realLineNumber = pos.realLineNumber;
            this->hitPos1.pos = ret;
            this->hitPos2.pos = pos;
        } else if (pos.realLineNumber < this->lineTextList.size() - 1) {
            auto it = getLineIt(pos.realLineNumber + 1);
            TextLayout::Pos ret;
            ret.realLineOffset = 0;
            ret.realLineNumber = pos.realLineNumber + 1;
            this->hitPos1.pos = pos;
            this->hitPos2.pos = ret;
        } else {
            this->hitPos1.pos = pos;
            this->hitPos2.pos = pos;
        }
    }
}

void TextLayout::keyBackspace() {
    this->blink_time = 1;
    if (this->hitPos1 == this->hitPos2) {
        select_text(this->hitPos1.pos, -1, 0);
    }
    if (this->hitPos1 != this->hitPos2) {
        this->remove_text(this->hitPos1.pos, this->hitPos2.pos, 0);
    }
}

void TextLayout::keyDelete() {
    this->blink_time = 1;
    if (this->hitPos1 == this->hitPos2) {
        select_text(this->hitPos1.pos, 1, 0);
    }
    if (this->hitPos1 != this->hitPos2) {
        this->remove_text(this->hitPos1.pos, this->hitPos2.pos, 2);
    }
}

void TextLayout::remove_text(Pos pos1, Pos pos2, int removeType) {
    bool lessThan = pos1 < pos2;
    auto &startPos = lessThan ? pos1 : pos2;
    auto &endPos = lessThan ? pos2 : pos1;
	std::list<LineStruct> insertText;

    if (pos1 != pos2) {
        modifyText(startPos.realLineNumber, endPos.realLineNumber, startPos.realLineOffset, endPos.realLineOffset, insertText);
    }
    RefreshLineHeight(startPos.realLineNumber, startPos.realLineNumber + 1, 0);

    if (removeType == 0) {
        Pos pos;
        pos.init();

        this->hitPos1.pos = startPos;
        this->hitPos1.pos.isBegin = startPos.isBegin;
        this->hitPos2.pos = this->hitPos1.pos;
        this->SelectVec[0].first = this->hitPos1.pos;
        this->SelectVec[0].second = this->SelectVec[0].first;
        if (this->cacheLineNumber >= startPos.realLineNumber)
            this->cacheLineIterator = getLineIt(this->cacheLineNumber);

        if (this->SelectVec[0].first < this->viewEyeTextBeginPos) {
            ViewToPos(this->SelectVec[0].first.realLineNumber, this->SelectVec[0].first.realLineOffset,
                      this->SelectVec[0].first.isBegin);
        } else if (this->SelectVec[0].first > this->viewEyeTextEndPos) {
            ViewToPos2(this->SelectVec[0].first.realLineNumber, this->SelectVec[0].first.realLineOffset,
                       this->SelectVec[0].first.isBegin);
        }


        this->onResize();

    }
    if (removeType == 1) {
        Pos pos;
        pos.init();

        this->hitPos1.pos = startPos;
        this->hitPos2.pos = this->hitPos1.pos;
        this->SelectVec[0].first = this->hitPos1.pos;
        this->SelectVec[0].second = this->SelectVec[0].first;
        this->onResize();
    }
    if (removeType == 2) {
        Pos pos;
        pos.init();

        this->hitPos1.pos = startPos;
        this->hitPos1.pos.isBegin = startPos.isBegin;
        this->hitPos2.pos = this->hitPos1.pos;
        this->SelectVec[0].first = this->hitPos1.pos;
        this->SelectVec[0].second = this->SelectVec[0].first;

        if (this->cacheLineNumber >= startPos.realLineNumber)
            this->cacheLineIterator = getLineIt(this->cacheLineNumber);

        if (this->SelectVec[0].first < this->viewEyeTextBeginPos) {
            ViewToPos(this->SelectVec[0].first.realLineNumber, this->SelectVec[0].first.realLineOffset,
                      this->SelectVec[0].first.isBegin);
        } else if (this->SelectVec[0].first > this->viewEyeTextEndPos) {
            ViewToPos2(this->SelectVec[0].first.realLineNumber, this->SelectVec[0].first.realLineOffset,
                       this->SelectVec[0].first.isBegin);
        }


        this->onResize();

    }
//    if(this->hitPos1.pos>=pos1 && this->hitPos1.pos<=pos2)
//    {
//        this->hitPos1.pos = pos1;
//    }
//    else{
//
//    }

}

std::string TextLayout::getSelectText(){
    std::string ret = "";
    if (this->hitPos1 == this->hitPos2) {
        return ret;
    }
    bool lessThan = this->hitPos1 < this->hitPos2;

    auto &startPos = lessThan ? this->hitPos1 : this->hitPos2;
    auto &endPos = lessThan ? this->hitPos2 : this->hitPos1;
    auto it = getLineIt(startPos.pos.realLineNumber);

    if(startPos.pos.realLineNumber == endPos.pos.realLineNumber)
    {
        ret += it->lineStr.substr(startPos.pos.realLineOffset, endPos.pos.realLineOffset - startPos.pos.realLineOffset);
    }
    else
    {
        ret += it->lineStr.substr(startPos.pos.realLineOffset, it->lineStr.length()-startPos.pos.realLineOffset);
        ret += "\n";
        int i=startPos.pos.realLineNumber+1;
        it++;
        for(; i<endPos.pos.realLineNumber; i++,it++)
        {
            ret += it->lineStr;
            ret += "\n";
        }
        if(i == endPos.pos.realLineNumber)
            ret += it->lineStr.substr(0, endPos.pos.realLineOffset);
    }
    return ret;
}

void TextLayout::copy() {
	this->blink_time = 1;
	if (this->hitPos1 == this->hitPos2) {
        return;
	}
    std::string str = getSelectText();
	if(str.empty())
	    return ;

    windowMgr->setClipboardText(str.c_str());
}

void TextLayout::viewToCursor()
{
	if (this->hitPos1.pos < this->viewEyeTextBeginPos) {
		ViewToPos(this->hitPos1.pos.realLineNumber,this->hitPos1.pos.realLineOffset,
				  this->hitPos1.pos.isBegin);
	} else if (this->hitPos1.pos > this->viewEyeTextEndPos) {
		ViewToPos2(this->hitPos1.pos.realLineNumber, this->hitPos1.pos.realLineOffset,
				   this->hitPos1.pos.isBegin);
	}

}
void TextLayout::input(const std::string& ss){
    this->blink_time = 1;

    bool lessThan = this->hitPos1 < this->hitPos2;

    auto &startPos = lessThan ? this->hitPos1 : this->hitPos2;
    auto &endPos = lessThan ? this->hitPos2 : this->hitPos1;
    if (startPos == endPos) {

    }

    if (startPos != endPos) {
        this->remove_text(startPos.pos, endPos.pos, 2);
    }

    std::list<LineStruct> insertText;
    if (!ss.empty())
        insertText = split(ss, '\n');
    int realLineNumber = 0;
    int realLineOffset = 0;
    if(insertText.size() > 1){
        realLineNumber = startPos.pos.realLineNumber+insertText.size()-1;
        realLineOffset = insertText.rbegin()->lineStr.length();
    }else if(insertText.size() == 1){
        realLineNumber = startPos.pos.realLineNumber+insertText.size()-1;
        realLineOffset = startPos.pos.realLineOffset+insertText.begin()->lineStr.length();
    }
    modifyText(startPos.pos.realLineNumber, startPos.pos.realLineNumber, startPos.pos.realLineOffset, startPos.pos.realLineOffset, insertText);

    RefreshLineHeight(startPos.pos.realLineNumber, realLineNumber, 0);
    this->hitPos1.pos.realLineNumber = realLineNumber;
    this->hitPos1.pos.realLineOffset = realLineOffset;
    this->hitPos1.pos.isBegin = false;
    this->hitPos2.pos = this->hitPos1.pos;
    this->SelectVec[0].first = this->hitPos1.pos;
    this->SelectVec[0].second = this->SelectVec[0].first;
    for(int i = this->hitPos1.pos.realLineNumber; i <= realLineNumber; i++){
        auto it = this->getLineIt(this->hitPos1.pos.realLineNumber);
        lineLayout(it, this->hitPos1.pos.realLineNumber, this->pageW);
    }

    this->viewToCursor();
    this->onResize();
}
void TextLayout::paste() {
	this->blink_time = 1;
	std::string ss;
	if(windowMgr->hasClipboardText())
	{
		ss = windowMgr->getClipboardText();
	}
    bool lessThan = this->hitPos1 < this->hitPos2;

    auto &startPos = lessThan ? this->hitPos1 : this->hitPos2;
    auto &endPos = lessThan ? this->hitPos2 : this->hitPos1;
	if (startPos == endPos) {

	}

	if (startPos != endPos) {
		this->remove_text(startPos.pos, endPos.pos, 2);
	}

	std::list<LineStruct> insertText;
	if (!ss.empty())
		insertText = split(ss, '\n');
	int realLineNumber = 0;
	int realLineOffset = 0;
	if(insertText.size() > 1){
		realLineNumber = startPos.pos.realLineNumber+insertText.size()-1;
		realLineOffset = insertText.rbegin()->lineStr.length();
    }else if(insertText.size() == 1){
        realLineNumber = startPos.pos.realLineNumber+insertText.size()-1;
        realLineOffset = startPos.pos.realLineOffset+insertText.begin()->lineStr.length();
    }
    modifyText(startPos.pos.realLineNumber, startPos.pos.realLineNumber, startPos.pos.realLineOffset, startPos.pos.realLineOffset, insertText);

	RefreshLineHeight(startPos.pos.realLineNumber, realLineNumber, 0);
	this->hitPos1.pos.realLineNumber = realLineNumber;
	this->hitPos1.pos.realLineOffset = realLineOffset;
	this->hitPos1.pos.isBegin = false;
	this->hitPos2.pos = this->hitPos1.pos;
	this->SelectVec[0].first = this->hitPos1.pos;
	this->SelectVec[0].second = this->SelectVec[0].first;
	for(int i = this->hitPos1.pos.realLineNumber; i <= realLineNumber; i++){
        auto it = this->getLineIt(this->hitPos1.pos.realLineNumber);
        lineLayout(it, this->hitPos1.pos.realLineNumber, this->pageW);
	}

    this->viewToCursor();
	this->onResize();

}
