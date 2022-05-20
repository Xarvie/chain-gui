//
// Created by Administrator on 2022/5/18.
//

#include "ChainGui.h"
#include "CGWindowBackend.h"
#include "fnv1a.h"
;
void ChainGui::init(int w, int h) {
    windowMgr->init();
    canvas.init(w, h);
    wc.init(10000, 10000);
    startTick = ChainGui::timeNow();
    this->w = w;
    this->h = h;

//    std::string fontFile = "../Res/AdobeArabic-Regular.otf";
    this->defaultFontSize = 16;
    defaultFontHash = fnv32("SourceHanSans-Normal.otf")*1000+this->defaultFontSize;
    std::string fontFile = "../Res/SourceHanSans-Normal.otf";
    auto & font = this->fontMap[defaultFontHash];
    this->defaultFont = &font;
    BLResult err = font.fontFace.createFromFile(fontFile.c_str());
    if (err) {
        printf("Failed to load a font-face (err=%u)\n", err);
        return ;
    }
    font.fontSize = 1000;
    font.fontSizeReal = this->defaultFontSize;
    font.font.createFromFace(font.fontFace, font.fontSizeReal);
    font.fm = font.font.metrics();
}

void ChainGui::pollEvent(void * event) {
    windowMgr->pollEvent(event);
}

Font* ChainGui::getFont(const std::string fontName, int fontSize) {
    int64_t code = fnv32(fontName.c_str())*1000+fontSize;
    auto it = this->fontMap.find(code);
    Font * font = nullptr;
    if(it != this->fontMap.end()){
        font = &it->second;
    }else{
        auto defaultSizeFontHash = fnv32(fontName.c_str())*1000+this->defaultFontSize;
        auto fontDefaultIt = this->fontMap.find(defaultSizeFontHash);

        std::string fontFile = std::string("../Res/")+fontName;

        auto & defaultSizeFont = this->fontMap[defaultSizeFontHash];

        if(fontDefaultIt == this->fontMap.end())
        {
            BLResult err = defaultSizeFont.fontFace.createFromFile(fontFile.c_str());
            if (err) {
                printf("Failed to load a font-face (err=%u)\n", err);
                this->fontMap.erase(defaultSizeFontHash);
                return this->defaultFont;
            }
            defaultSizeFont.fontSize = 1000;
            defaultSizeFont.fontSizeReal = this->defaultFontSize;
            defaultSizeFont.font.createFromFace(defaultSizeFont.fontFace, defaultSizeFont.fontSizeReal);
            defaultSizeFont.fm = defaultSizeFont.font.metrics();
        }
        if(fontSize==this->defaultFontSize){
            return &defaultSizeFont;
        }else{
            auto fontHash = fnv32(fontName.c_str())*1000+fontSize;
            auto & font = this->fontMap[fontHash];
            font.fontFace = defaultSizeFont.fontFace;
            font.fontSize = 1000;
            font.fontSizeReal = fontSize;
            font.font.createFromFace(font.fontFace, fontSize);
            font.fm = font.font.metrics();
            return &font;
        }
    }
    return font;
}

unsigned char* ChainGui::draw(){
    auto curTick = ChainGui::timeNow();
    static uint64_t fpsCounter = ChainGui::timeNow();
    static int fps = 0;
    static std::string curFps = "";
    fps++;
    if(curTick - fpsCounter >= 1000){
        curFps = std::to_string(fps);
        fps = 0;
        fpsCounter = curTick;
    }

    canvas.drawBegin(NULL);
    canvas.setFillStyle(0x00FFFFFF);
    canvas.fillAll();

    for(auto window:this->rootWindows){
        window->draw(curTick);
    }

    canvas.setFillStyle(0xFF990000);
    canvas.drawText(curFps.c_str()
            , *defaultFont, 20,20,100,100);

    canvas.drawEnd();
    return canvas.getPixels();
}

void ChainGui::setCanvasSize(int w, int h){
    this->w = w;
    this->h = h;
    canvas.setCanvasSize(w, h);
}

ChainGui *ChainGui::get() {
    static ChainGui gui;
    return &gui;
}

int64_t ChainGui::timeNow() {
#ifdef _WIN32
        _timeb timebuffer;
        _ftime(&timebuffer);
        int64_t ret = timebuffer.time;
        ret = ret * 1000 + timebuffer.millitm;
        return ret;
#else
        timeval tv;
    ::gettimeofday(&tv, 0);
    int64_t ret = tv.tv_sec;
    return ret * 1000 + tv.tv_usec / 1000;
#endif
}
