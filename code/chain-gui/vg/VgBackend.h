//
// Created by Administrator on 2022/5/19.
//

#ifndef CHAIN_GUI_VGBACKEND_H
#define CHAIN_GUI_VGBACKEND_H

#include "blend2d.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <vector>

typedef std::vector<unsigned char> bytes;

class BlendBackend;

template<typename T>
struct DrawBackendInterface {
	void drawBegin() {
	}

	void drawEnd() {

	}

	unsigned char *getOutputArrayData() {
		return nullptr;
	}

	unsigned char *getOutputBMPData() {
		return nullptr;
	}

};

class BlendBackendFont {
public:
    BlendBackendFont(){

    }
    ~BlendBackendFont(){

    }
	BLFont font;
	double fontSize;
	double fontSizeReal;
	std::string fontName;
	BLFontFace fontFace;
	BLFontMetrics fm;
};
typedef BlendBackendFont Font;
struct BlendBackendImage {
	BLImage img;
	bool empty = true;
};

typedef BlendBackendImage Image;
struct BlendBackendData {
	BLContext ctx;
	BLFontFace face;
//    BLFontMetrics fm;
	BLTextMetrics tm;
	BLImageCodec codec;
	Image img;
	BLGlyphBuffer gb;
	int w;
	int h;
};

#include <list>

struct BlendBackend{
public:
	BlendBackend() {

	}
    void init(int w, int h){
        this->data.w = w;
        this->data.h = h;
    }
    void setCanvasSize(int w, int h){
        this->data.w = w;
        this->data.h = h;
    }
	double getTextWidth(const char *utf8Text, int len, BLGlyphBuffer &gb, const Font &font) {
		double blankW = 0;
		if (utf8Text[0] == 32 || utf8Text[0] == '\t') {

			if (blankW != 0)
				return blankW;
			gb.setUtf8Text("i i", 3);
			font.font.shape(gb);

			BLTextMetrics tm1;
			font.font.getTextMetrics(gb, tm1);
			float x1 = tm1.advance.x;
			gb.setUtf8Text("ii", 2);
			font.font.shape(gb);

			BLTextMetrics tm;
			font.font.getTextMetrics(gb, tm);
			float x2 = tm.advance.x;
			blankW = x1 - x2;
			if(utf8Text[0] == '\t')
                blankW = blankW*4;
			return blankW;
		}

		gb.setUtf8Text(utf8Text, len);
		font.font.shape(gb);

		BLTextMetrics tm;
		font.font.getTextMetrics(gb, tm);
		//p.x = (w - (data.tm.boundingBox.x1 - data.tm.boundingBox.x0)) / 2.0;
		return tm.advance.x;
	}

	void drawText(const char *text, const Font &font, int x, int y, int w, int h) {
		BLPoint p(x, y + font.fm.ascent);

		for (;;) {
			const char *end = strchr(text, '\n');
			data.gb.setUtf8Text(text, end ? (size_t) (end - text) : SIZE_MAX);
			font.font.shape(data.gb);


			font.font.getTextMetrics(data.gb, data.tm);
			//p.x = (w - (data.tm.boundingBox.x1 - data.tm.boundingBox.x0)) / 2.0;
			p.x = x;
			data.ctx.fillGlyphRun(p, font.font, data.gb.glyphRun());
			p.y += font.fm.ascent + font.fm.descent + font.fm.lineGap;

			if (!end) break;
			text = end + 1;

		}
	}

	void drawText(std::vector<std::string> &strVec, const Font &font, int x, int y, int w, int d) {
		BLPoint p(x, y + font.fm.ascent);

		for (std::string &E: strVec) {

			data.gb.setUtf8Text(&E[0], E.length());
			font.font.shape(data.gb);


			font.font.getTextMetrics(data.gb, data.tm);
			//p.x = (w - (data.tm.boundingBox.x1 - data.tm.boundingBox.x0)) / 2.0;
			p.x = x;
			data.ctx.fillGlyphRun(p, font.font, data.gb.glyphRun());
			p.y += font.fm.ascent + font.fm.descent + font.fm.lineGap;

		}
	}

	void drawEnd() {

//        painter().drawText(text.c_str(), this->collider.x, this->collider.y, this->collider.w, this->collider.h);
//
//        static int xx = 0;
//        if (xx++ % 500 == 0) {
//            std::cout << 500000.0 / (SDL_GetTicks() - timestart) << std::endl;
//        }


		data.ctx.end();
		//data.codec.findByName("BMP");

		//data.dstx.resize(data.w * data.h * 8 + 200,0);
		//data.img->writeToData(data.dstx, data.codec);
	}

	void setFillStyle(uint32_t rgba) {
		data.ctx.setFillStyle(BLRgba32(rgba));
	}

	void setStrokeStyle(uint32_t rgba) {
		data.ctx.setStrokeStyle(BLRgba32(rgba));
	}

	void setStrokeWidth(double width) {
		data.ctx.setStrokeWidth(width);
	}

	void drawImg(double x, double y, double w, double h, Image *img) {

		data.ctx.setCompOp(BL_COMP_OP_SRC_OVER);
		auto xs = BLPattern(img->img);

		xs.setExtendMode(BL_EXTEND_MODE_PAD);
		xs.postTranslate(x, y);

		data.ctx.setFillStyle(xs);

		data.ctx.fillRoundRect(x, y, w, h, 0.0f);
	}

	void drawLine(double x1, double y1, double x2, double y2) {
		data.ctx.strokeLine(x1, y1, x2, y2);
	}
    void drawRect(double x, double y, double w, double h) {
        data.ctx.strokeRect(x, y, w, h);
    }
    void drawRectFilled(double x, double y, double w, double h) {
        data.ctx.fillRect(x, y, w, h);
    }
    void drawCircleFilled(double x, double y, double r) {
        data.ctx.fillCircle(x, y, r);
    }

	void fillAll() {
		data.ctx.fillAll();
	}

	int drawBegin(const Font *pfont) {
		const auto &font = pfont;
		data.img.img = BLImage(data.w, data.h, BL_FORMAT_PRGB32);

		data.ctx = BLContext(data.img.img);
		data.ctx.setCompOp(BL_COMP_OP_SRC_COPY);




//        BLResult err = data.face.createFromFile("../simkai.ttf");
//        if (err) {
//            printf("Failed to load a font-face (err=%u)\n", err);
//            return 1;
//        }
//

		{

			BLGlyphBuffer gb;
			data.gb.reset();
		}


		return 0;
	}

//	unsigned char *getOutputArrayData() {
//		const int bmpHead = 2 + 12 + 108;
//		if (data.pixels.empty())
//			return NULL;
//        std::cout << data.pixels.size() << std::endl;
//		return (unsigned char *) &data.pixels[0] + bmpHead;
//	}
//
//	unsigned char *getOutputBMPData() {
//		if (data.pixels.empty())
//			return NULL;
//		return (unsigned char *) &data.pixels[0];
//	}
    unsigned char *getPixels() {
        BLImageData dataOut;
        data.img.img.getData(&dataOut);
        return (unsigned char *)dataOut.pixelData;
    }

    unsigned char *getOutputBMPData() {
        return nullptr;
    }
	BlendBackendData data;
};

typedef BlendBackend VGBackend;
typedef BlendBackendData DrawBackendData;
#endif //APP_DRAWBACKENDIMPL_H
