//
// Created by Administrator on 2022/5/22.
//

#ifndef SKIUI_TEXTLAYOUT_H
#define SKIUI_TEXTLAYOUT_H

#include <string>
#include <vector>
#include "VgBackend.h"
#include <unordered_map>
#include <list>

struct AutoChangeLineStruct {
    double lineHight = 0;
    std::string lineStr = "";
    int realLineOffset = 0;
    std::vector<std::string> lineTextListAutoWidthVec;//TODO 后面的文本和图片之类的其他元素放在vec中
};

struct LineStruct {
    double Yoffset = 0;
    double lineHeight;
    std::string lineStr;
    std::list<AutoChangeLineStruct> lineTextListAutoWidth;
};

static std::list<LineStruct> split(const std::string &str_, char c) {
    std::string str = str_ + "\n";
//    str.push_back('\n');
    std::list<LineStruct> resVec;
    const char *strPtr = &str[0];
    const char *strPtrOld = strPtr;
    char cc = *strPtr;
    while (cc != 0) {
        cc = *strPtr;
        strPtr++;
        if (cc == c) {
            int len = strPtr - strPtrOld;
            if (len == 0) {
                resVec.emplace_back(LineStruct{});
            } else {

                if (len >= 2 && strPtrOld[len - 2] == '\r') {
                    resVec.emplace_back(LineStruct{.lineStr=std::string(strPtrOld, strPtr - 2 - strPtrOld)});
                    strPtrOld = strPtr;
                } else {

                    resVec.emplace_back(LineStruct{.lineStr=std::string(strPtrOld, strPtr - 1 - strPtrOld)});
                    strPtrOld = strPtr;
                }
            }

        } else {

        }


    }

    return resVec;
}


class TextLayout {
public:
    double lastCursorXPixer = 0.0;
    double font_height = 0;
    int blink_time = 0;
    Font* font = nullptr;
    int MoveOffsetTag = -1;

    struct Pos {
        void init() {
            int id = 0;
            int realLineNumber = 0;
            int realLineOffset = 0;
            int subLineNumber = 0;
            int subLineOffset = 0;
            bool isBegin = false;
        }

        int id = 0;
        int realLineNumber = 0;
        int realLineOffset = 0;
        int subLineNumber = 0;
        int subLineOffset = 0;
        bool isBegin = false;

        bool operator==(const Pos &x) const {
            return (this->realLineNumber == x.realLineNumber &&
                    this->realLineOffset == x.realLineOffset);
        }

        bool operator!=(const Pos &x) const {
            return !(*this == x);
        }

        bool operator<(const Pos &x) const {
            if (this->realLineNumber != x.realLineNumber) {
                return this->realLineNumber < x.realLineNumber;
            }

            if (this->realLineOffset != x.realLineOffset) {
                return this->realLineOffset < x.realLineOffset;
            }
            return false;
        }

        bool operator>(const Pos &x) const {
            if (*this == x)
                return false;

            return !(*this < x);
        }

        bool operator>=(const Pos &x) const {
            if (*this == x)
                return true;
            return *this > x;
        }

        bool operator<=(const Pos &x) const {
            if (*this == x)
                return true;
            return *this < x;
        }
    };

    struct PosEx {
        Pos pos;
        int subLineU8offset = 0;
        double LineOffsetPixelX = 0;
        double LineOffsetPixelY = 0;
        double LineHeightPixel = 0;
        int overflow = 0;


        bool operator==(const PosEx &x) const {
            return (this->pos.realLineNumber == x.pos.realLineNumber &&
                    this->pos.realLineOffset == x.pos.realLineOffset);
        }

        bool operator!=(const PosEx &x) const {
            return !(*this == x);
        }

        bool operator<(const PosEx &x) const {
            if (this->pos.realLineNumber != x.pos.realLineNumber) {
                return this->pos.realLineNumber < x.pos.realLineNumber;
            }
            return this->pos.realLineOffset < x.pos.realLineOffset;
        }

        bool operator>(const PosEx &x) const {
            if (*this == x)
                return false;

            return !(*this < x);
        }

        bool operator>=(const PosEx &x) const {
            if (*this == x)
                return true;
            return *this > x;
        }

        bool operator<=(const PosEx &x) const {
            if (*this == x)
                return true;
            return *this < x;
        }
    };

    void select_text(Pos pos, int offset, int Type);

    void RefreshLineHeight(int beginLine, int endLine, int level = 0);

    void updateCursor();

    void onResize();

    std::list<LineStruct>::iterator
    lineLayout(std::list<LineStruct>::iterator lineStructIt, int lineNumber, double pageW, bool writeData = true);

    std::list<LineStruct>::iterator
    pageLayout(int lineBegin, int lineEnd, double offsetX, double offsetY, double &allSize);

    void getViewLines();

    struct Rects {
        double a;
        double b;
        double c;
        double d;
    };
    struct ViewLine {
        AutoChangeLineStruct subLine;
        int realLineNumber = 0;
        int subLineNumber = 0;
        int lineCharCount = 0;
        int realLineOffset = 0;
    };

    std::vector<TextLayout::PosEx> getPosBySubLinePos(std::vector<int> Line, std::vector<int> TextPos, bool NextBegin,
                                                      bool LineOffsetPixelXAddWidth = false);

    void pageMovePixel(double pixel);

    void pageMoveSubLine(int lineCount, double offsetX, double offsetY);


    void loadString(const std::string &str);

    void onLButtonDown(double x, double y, bool inner = false);

    TextLayout::PosEx getPos(double x, double y);

    void onLButtonUp(double x, double y);

    void onMove(double x, double y);

    void onDrag(double x, double y);

    void modifyText(int lineBegin, int lineEnd, int beginPos, int endPos, std::list<LineStruct> &newText);

    void copy();

    void paste();

    void input(const std::string& ss);

    std::string getSelectText();

    void viewToCursor();

    std::list<LineStruct> lineTextList;

    int ViewTextBeginLine = -1;
    int ViewTextCurLine = -1;

    int ViewEyeTextBeginLine = -1;
    int ViewEyeTextBeginSubLine = -1;
    int ViewEyeTextEndLine = -1;
    int ViewEyeTextEndSubLine = -1;
    int ViewEyeTextBeginLinePos = 0;
    int ViewEyeTextBeginSubLinePos = 0;
    int ViewEyeTextEndLinePos = 0;
    int ViewEyeTextEndSubLinePos = 0;
    Pos viewEyeTextBeginPos;
    Pos viewEyeTextEndPos;

    bool canSeeBeginCursor = false;
    bool canSeeEndCursor = false;
    int cursorPos = -1;
    bool front = false;
    std::vector<std::vector<ViewLine>> ViewTextVec;

    int showLineNum = 0;
    double xOffset = 0;
    double yOffset = 0;//滚动条向下拉offset++
    double firstLineHeight = 0;
    int cacheLineNumber = -1;
    double allSize;
    std::list<LineStruct>::iterator cacheLineIterator;

    double pageW = 0;
    double pageH = 0;

    BLGlyphBuffer gb;
    std::unordered_map<char32_t, double> textWidthMap;
    TextLayout::PosEx hitPos1;
    TextLayout::PosEx hitPos2;

    std::vector<std::pair<TextLayout::Pos, TextLayout::Pos>> SelectVec = std::vector<std::pair<TextLayout::Pos, TextLayout::Pos>>(
            1, std::pair<TextLayout::Pos, TextLayout::Pos>(TextLayout::Pos(), TextLayout::Pos()));

//    TextLayout::Pos hitPos1 = TextLayout::Pos{.RealLineNumber = 0,
//            .TinyLineNumber = 0,
//            .RealLineOffset = 0,
//            .TinyLineOffset = 0,
//            .LineOffsetPixelX = 0,
//            .LineOffsetPixelY = 0,
//            .LineHeightPixel = 0};
//    TextLayout::Pos hitPos2 = TextLayout::Pos{.RealLineNumber = 0,
//            .TinyLineNumber = 0,
//            .RealLineOffset = 0,
//            .TinyLineOffset = 0,
//            .LineOffsetPixelX = 0,
//            .LineOffsetPixelY = 0,
//            .LineHeightPixel = 0};
    bool press = false;


    LineStruct *getLineStruct(int lineNumber);

    std::list<LineStruct>::iterator getLineIt(int lineNumber);

    TextLayout::Pos logic_left_move(TextLayout::Pos pos);

    void left_move(bool inner = false);

    void right_move(bool inner = false);

    std::vector<TextLayout::PosEx> parseSelector(int move, bool nextBegin = false);

    void ViewToPos(int lineBegin, int lineOffset, int nextBegin = false);

    void ViewToPos2(int lineEnd, int lineOffset, int nextBegin = false);

    std::vector<int> getViewSelector();
//

    TextLayout::Pos logic_up_move(TextLayout::Pos pos);

    void up_move();

    TextLayout::Pos logic_down_move(TextLayout::Pos pos);

    void down_move();

    void getSubLineNumberAndOffset(TextLayout::Pos pos);

//
//	vector<begin, end> get_selection()
//	{
//
//	}
//
    void remove_text(Pos begin, Pos end, int removeType);

    void keyBackspace();

    void keyDelete();
//
//	void insert_text(vector<begin, end>){
//
//	}
//
//	void opSelect(vector<begin, end> ){
//
//	}
//
//
//	void opDelete(){
//		s = get_selection();
//		begin_op();
//		if (s.begin == s.end) {
//			s.begin = caret;
//			s.end = logic_right_move(caret);
//		}
//		remove_text(s);
//		end_op();
//	}
//
//	void opBackspace(){
//		s = get_selection();
//		begin_op();
//		if (s.begin == s.end) {
//			s.end = caret;
//			s.begin = logic_left_move(caret);
//		}
//		remove_text(s);
//		caret = s.begin;
//		end_op();
//	}
//
//	void opWrite(text){
//		begin_op();
//		remove_text(get_selection());
//		insert_text(caret, text);
//		end_op();
//	}
//

};


static std::list<LineStruct> split(const std::string &str, const std::string &pattern) {
    std::list<LineStruct> resVec;

    if (str.empty()) {
        return resVec;
    }
    std::string strs = str + pattern;

    size_t pos = strs.find(pattern);
    size_t size = strs.size();

    while (pos != std::string::npos) {
        std::string x = strs.substr(0, pos);
        LineStruct st;
        st.lineStr = x;
        resVec.push_back(st);
        strs = strs.substr(pos + 1, size);
        pos = strs.find(pattern);
    }

//    if(*str.rbegin() == '\n')
//    {
//        resVec.pop_back();
//    }
    if (resVec.empty()) {
        LineStruct line;
        line.lineStr = str;
        resVec.push_back(line);
    }

    return resVec;
}

static inline bool PosLineGT(const TextLayout::PosEx &a, const TextLayout::PosEx &b) {
    if (a.pos.realLineNumber != b.pos.realLineNumber)
        return a.pos.realLineNumber > b.pos.realLineNumber;
    if (a.pos.subLineNumber != b.pos.subLineNumber)
        return a.pos.subLineNumber > b.pos.subLineNumber;
    return false;
}

static inline bool PosLineLT(const TextLayout::PosEx &a, const TextLayout::PosEx &b) {
    if (a.pos.realLineNumber != b.pos.realLineNumber)
        return a.pos.realLineNumber < b.pos.realLineNumber;
    if (a.pos.subLineNumber != b.pos.subLineNumber)
        return a.pos.subLineNumber < b.pos.subLineNumber;
    return false;
}

static inline bool PosLinEQ(const TextLayout::PosEx &a, const TextLayout::PosEx &b) {
    return (a.pos.realLineNumber == b.pos.realLineNumber && a.pos.subLineNumber == b.pos.subLineNumber);
}

static inline bool PosLineGTE(const TextLayout::PosEx &a, const TextLayout::PosEx &b) {
    if (PosLinEQ(a, b))
        return true;
    if (a.pos.realLineNumber != b.pos.realLineNumber)
        return a.pos.realLineNumber > b.pos.realLineNumber;
    if (a.pos.subLineNumber != b.pos.subLineNumber)
        return a.pos.subLineNumber > b.pos.subLineNumber;
    return false;
}

static inline bool PosLineLTE(const TextLayout::PosEx &a, const TextLayout::PosEx &b) {
    if (PosLinEQ(a, b))
        return true;
    if (a.pos.realLineNumber != b.pos.realLineNumber)
        return a.pos.realLineNumber < b.pos.realLineNumber;
    if (a.pos.subLineNumber != b.pos.subLineNumber)
        return a.pos.subLineNumber < b.pos.subLineNumber;
    return false;
}


#endif //SKIUI_TEXTLAYOUT_H
