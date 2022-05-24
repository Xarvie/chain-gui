//
// Created by Administrator on 2022/5/18.
//

#include "UIControl.h"
#include "ChainGui.h"

#define GET_GUI() auto gui = ChainGui::get(); auto&canvas = gui->canvas
int idGen() {
    static uint64_t id = 0;
    id++;
    return id;
}

std::string m_replace(std::string strSrc,
                 const std::string &oldStr, const std::string &newStr, int count) {
    std::string strRet = strSrc;
    size_t pos = 0;
    int l_count = 0;
    if (-1 == count) // replace all
        count = strRet.size();
    while ((pos = strRet.find(oldStr, pos)) != std::string::npos) {
        strRet.replace(pos, oldStr.size(), newStr);
        if (++l_count >= count) break;
        pos += newStr.size();
    }
    return strRet;
}

UIRectF rectFIntersection(const UIRectF& left, const UIRectF& right)
{
    if  ((left.r  >  right.l)  &&
         (left.l <  right.r)  &&
         (left.b  >  right.t)  &&
         (left.t  <  right.b))
    {
        UIRectF temp;
        temp.l  =  (left.l  >  right.l)  ?  left.l : right.l;
        temp.r  =  (left.r  <  right.r)  ?  left.r : right.r;
        temp.t  =  (left.t  >  right.t)  ?  left.t : right.t;
        temp.b  =  (left.b  <  right.b)  ?  left.b : right.b;
        return  temp;
    }
    else
    {
        return UIRectF{ 0.0 ,  0.0 ,  0.0 ,  0.0};
    }
}

void WindowCollision::init(int w, int h) {
    double fatten = 0.001;
    std::vector<bool> periodicity({true, true});
    std::vector<double> boxSize({(double) w, (double) h});

    unsigned int AABBMaxNum = 1000;
    this->tree = aabb::Tree(2, fatten, periodicity, boxSize, AABBMaxNum, false);
}

void WindowCollision::insertAABB(AABBKey index, int x, int y, int w, int h) {
    std::vector<double> lowerBound({(double) x, (double) y});
    std::vector<double> upperBound({(double) (x + w) - 0.001, (double) (y + h) - 0.001});

    tree.insertParticle(index, lowerBound, upperBound);
}

void WindowCollision::updateAABB(AABBKey index, int x, int y, int w, int h) {
    std::vector<double> lowerBound({(double) x, (double) y});
    std::vector<double> upperBound({(double) (x + w) - 0.001, (double) (y + h) - 0.001});

    tree.updateParticle(index, lowerBound, upperBound);
}

void WindowCollision::removeAABB(AABBKey index) {
    tree.removeParticle(index);
}

void WindowCollision::queryByPoint(int x, int y, std::set<AABBKey> &processSet,
                                   std::set<AABBKey> &resultSet, bool isnear) {
    queryByAABB(x, y, 0.001, 0.001, processSet, resultSet, isnear);
}

void WindowCollision::queryByAABB(int x, int y, double w, double h, std::set<AABBKey> &processSet,
                                  std::set<AABBKey> &resultSet, bool isnear) {
    std::vector<double> lowerBound({(double) x, (double) y});
    std::vector<double> upperBound({((double) (x + w)) - 0.0001, ((double) (y + h)) - 0.0001});
    auto queryVec = this->tree.query(aabb::AABB(lowerBound, upperBound));


    for (auto E: queryVec) {
        auto baseControl = (BaseControl *) E;
        if (!baseControl->colliderOn)
            continue;
        if (processSet.count(E) == 0) {
            processSet.insert(E);
            resultSet.insert(E);
            if (isnear)
                findByIndex(E, processSet, resultSet);
        }
    }
}

int WindowCollision::findByIndex(AABBKey index, std::set<AABBKey> &processSet,
                                 std::set<AABBKey> &resultSet, bool isnear) {
    auto baseControl = (BaseControl *) index;
    if (!baseControl->colliderOn)
        return 0;
    auto queryVec = this->tree.query(index);
    for (auto E: queryVec) {
        auto baseControl = (BaseControl *) E;
        if (!baseControl->colliderOn)
            continue;
        if (processSet.count(E) == 0) {
            processSet.insert(E);
            resultSet.insert(E);
            if (isnear)
                findByIndex(E, processSet, resultSet);
        }
    }
    return 0;
}

int Box::init(Box *parent, int x, int y, int w, int h) {
    this->collider.x = x, this->collider.y = y;
    this->collider.w = w, this->collider.h = h;
    this->BaseControl::init(parent);
    return 0;
}

Box *Box::create(Box *parent, int x, int y, int w, int h) {
    return nullptr;
}

int Box::draw(int64_t tick) {
    GET_GUI();
    for (auto &E:this->childControlMap) {
        E->draw(tick);
    }
    return 0;
}

int Box::resize(int w, int h) {
    GET_GUI();
    this->collider.w = w;
    this->collider.h = h;
    canvas.data.w = w;
    canvas.data.h = h;
    return 0;
}

UIWindow *UIWindow::create(Box *parent, const std::string &title, int x, int y, int w, int h) {
    auto *self = new UIWindow;
    self->init(parent, x, y, w, h);
    self->title = title;
    return self;
}

int UIWindow::init(Box *parent, int x, int y, int w, int h) {
    GET_GUI();
    if (parent == NULL) {
        gui->rootWindows.insert(this);
    }
    return Box::init(parent, x, y, w, h);
}

int UIWindow::draw(int64_t tick) {
    GET_GUI();
    gui->canvas.clipDrawStart(clipRect.l, clipRect.t, clipRect.r-clipRect.l, clipRect.b-clipRect.t);
    canvas.setFillStyle(0xFF0000FF);
    canvas.drawRectFilled(ax, ay, this->collider.w, this->collider.h);

    canvas.setFillStyle(0xFF00FF00);
    canvas.drawRectFilled(ax, ay, this->collider.w, 20);

    canvas.setFillStyle(0xFF000000);
    canvas.drawText(this->title.c_str()
            , gui->defaultFont,this->ax+10,this->ay+1.0,100,100);
    canvas.setFillStyle(0xFFF20202);
    canvas.drawCircleFilled(this->ax + this->collider.w - 10.0, ay + 10.0, 5);


    Box::draw(tick);
    drawBound();
    gui->canvas.clipDrawEnd();

    return 0;
}

int BaseControl::init(Box *parent) {
    GET_GUI();
    this->layer = 0;
    this->id = idGen();
    this->parent = parent;
    if (parent != nullptr) {
        parent->childControlMap.insert(this);
    }

    this->ax = (parent ? parent->ax : 0) + this->collider.x;
    this->ay = (parent ? parent->ay : 0) + this->collider.y;
    this->calcClipRect();
    gui->wc.insertAABB((AABBKey)this, this->clipRect.l, this->clipRect.t, this->clipRect.r-this->clipRect.l, this->clipRect.b-this->clipRect.t);

    return 0;
}

int BaseControl::setDirty(bool dirty_) {
    GET_GUI();
    gui->dirty = dirty_;
    return 0;
}

int BaseControl::draw(int64_t tick) {
    return 0;
}

void BaseControl::drawBound() {
    GET_GUI();
    if (!drawBound_)
        return;
    canvas.setStrokeWidth(4.0);
    canvas.setStrokeStyle(0xFFFF0000);
    canvas.drawRect(this->ax, this->ay, this->collider.w, this->collider.h);
    drawBound_ = false;
}

void BaseControl::setPos(int x, int y) {
    calcClipRect();
}

void BaseControl::setSize(int w, int h) {
    calcClipRect();
}

void BaseControl::calcClipRect() {
    if(this->parent){
        this->clipRect = rectFIntersection(
                UIRectF{this->ax, this->ay,
                        this->ax+this->collider.w, this->ay + this->collider.h},
                this->parent->clipRect);
    }
    else{
        this->clipRect =
                UIRectF{this->ax, this->ay,
                        this->ax+this->collider.w, this->ay + this->collider.h};
    }
}

#if defined(TBOX)

EditBox::EditBox() {
}

EditBox::~EditBox() {
}

EditBox *EditBox::create(Box *parent, const std::string &text, int x, int y, int w, int h) {
    GET_GUI();
    auto *self = new EditBox;

    self->text = text;
    self->isInit = 1;
    self->colorBkg = 0;
    self->collider.x = x;
    self->collider.y = y;
    self->collider.w = w;
    self->collider.h = h;
    self->state = 0;
    self->BaseControl::init(parent);

    self->font = gui->defaultFont;
    self->tl.loadString(text);
    self->tl.font = self->font;
    for (auto &E: self->tl.lineTextList) {
        E.lineHeight = self->tl.font->fontSizeReal;
    }
    self->tl.yOffset = 0.0f;
    self->tl.pageW = self->collider.w;
    self->tl.pageH = self->collider.h;
    self->tl.cacheLineNumber = 0;
    for (int i = 0; i < self->tl.cacheLineNumber; i++) {
        self->tl.cacheLineIterator++;
    }

    {
        self->tl.font_height = double(self->tl.font->fm.ascent + self->tl.font->fm.descent + self->tl.font->fm.lineGap);
        for (auto &E: self->tl.lineTextList) {
            E.lineHeight = self->tl.font_height;
        }
    }

    double allSize = 0;
    self->tl.pageLayout(self->tl.cacheLineNumber, self->tl.lineTextList.size() - 1 - self->tl.cacheLineNumber, 0, 0,
                        allSize);
    self->tl.allSize = allSize;
    if (allSize == 0) {
        //        abort();
    }
    //    self->tl.pageMovePixel(-5.0);

    return self;
}

int EditBox::destroy() {
    if (this->isInit == 0)
        return -1;
    return 0;
}

int EditBox::setFont(const std::string &fontName, float fontSize) {
    GET_GUI();
    this->font = gui->getFont(fontName, fontSize);
    this->tl.font = this->font;

    for (auto &E: this->tl.lineTextList) {
        {
            E.lineHeight = double(this->font->fm.ascent + this->font->fm.descent + this->font->fm.lineGap);
        }
    }
    //    lineTextList
    double allSize = 0;
    this->tl.pageLayout(this->tl.cacheLineNumber, this->tl.lineTextList.size() - 1 - this->tl.cacheLineNumber, 0.0f,
                        this->tl.yOffset, allSize);
    this->tl.allSize = allSize;
    if (allSize == 0) {
        //        abort();
    }
    return 0;
}

int EditBox::draw(int64_t tick) {
    GET_GUI();

    gui->canvas.clipDrawStart(clipRect.l, clipRect.t, clipRect.r-clipRect.l, clipRect.b-clipRect.t);

    if(this->colorBkg)
    {
        canvas.setFillStyle(this->colorBkg);
        canvas.fillAll();
    }

    double showHeight = 0;
    std::vector<std::string> strings;
    for (auto it = this->tl.cacheLineIterator; it != this->tl.lineTextList.end(); it++) {
        showHeight += it->lineHeight;
        for (const auto &E:it->lineTextListAutoWidth) {
            strings.push_back(E.lineStr);
        }
    }
    bool lessThan = this->tl.hitPos1 < this->tl.hitPos2;

    auto &startPos = lessThan ? this->tl.hitPos1 : this->tl.hitPos2;
    auto &endPos = lessThan ? this->tl.hitPos2 : this->tl.hitPos1;

    canvas.setFillStyle(0xFFA6D2FF);//166,210,255

    TextLayout::PosEx view1;
    TextLayout::PosEx view2;
    view1.pos.realLineNumber = this->tl.ViewEyeTextBeginLine;
    view1.pos.subLineNumber = this->tl.ViewEyeTextBeginSubLine;
    view1.pos.realLineOffset = this->tl.ViewEyeTextBeginLinePos;
    view1.pos.subLineOffset = this->tl.ViewEyeTextBeginSubLinePos;
    view1.LineOffsetPixelX = -1;
    view2.pos.realLineNumber = this->tl.ViewEyeTextEndLine;
    view2.pos.subLineNumber = this->tl.ViewEyeTextEndSubLine;
    view2.pos.realLineOffset = this->tl.ViewEyeTextEndLinePos;
    view2.pos.subLineOffset = this->tl.ViewEyeTextEndSubLinePos;
    view2.LineOffsetPixelX = 9999999;
    bool startBTo1 = startPos < view1;
    bool start1To2 = startPos >= view1 && startPos <= view2;
    bool end1To2 = endPos >= view1 && endPos <= view2;
    bool end2ToE = endPos > view2;
    auto pageOffsetX = ax+tl.xOffset;
    auto pageOffsetY = ay;
    if (this->focus && start1To2 && end1To2) {
        if (startPos.LineOffsetPixelY != endPos.LineOffsetPixelY && startPos != endPos) {
            {
                {
                    auto fireLineRectStartX = (pageOffsetX + startPos.LineOffsetPixelX);

                    auto w = (double) this->collider.w;

                    if (tl.xOffset<0 && -tl.xOffset > startPos.LineOffsetPixelX) {
                        fireLineRectStartX = ax;
                        w = this->collider.w;
                    }
                    canvas.drawRectFilled(fireLineRectStartX,
                                          ay + startPos.LineOffsetPixelY - this->tl.yOffset,
                                          fireLineRectStartX+w,
                                          startPos.LineHeightPixel + 0.5);
                }
                if (endPos.LineOffsetPixelY != startPos.LineOffsetPixelY + startPos.LineHeightPixel)
                {
                    auto fireLineRectStartX = pageOffsetX;
                    if(tl.xOffset < 0) {
                        fireLineRectStartX = ax;
                    }
                        canvas.drawRectFilled(fireLineRectStartX, ay+startPos.LineOffsetPixelY - this->tl.yOffset +
                                                             startPos.LineHeightPixel,
                                              (double) this->collider.w,
                                              (endPos.LineOffsetPixelY - this->tl.yOffset) -
                                              (startPos.LineOffsetPixelY - this->tl.yOffset) -
                                              startPos.LineHeightPixel + 0.5);

                }


                if (endPos.LineHeightPixel + endPos.LineOffsetPixelY - this->tl.yOffset > 0)
                    canvas.drawRectFilled(pageOffsetX+0, ay+endPos.LineOffsetPixelY - this->tl.yOffset,
                                                      endPos.LineOffsetPixelX,
                                                      endPos.LineHeightPixel);
            }

        } else {
            if (startPos != endPos)
                canvas.drawRectFilled(pageOffsetX+startPos.LineOffsetPixelX,
                                                  ay+startPos.LineOffsetPixelY - this->tl.yOffset,
                                                  endPos.LineOffsetPixelX - startPos.LineOffsetPixelX,
                                                  startPos.LineHeightPixel);
        }
    }
    if (this->focus && startBTo1 && end1To2) {
        if (!PosLinEQ(startPos, endPos) || startPos.LineOffsetPixelY != endPos.LineOffsetPixelY) {
            {
                auto fireLineRectStartX = pageOffsetX;
                if(tl.xOffset < 0) {
                    fireLineRectStartX = ax;
                }
                canvas.drawRectFilled(fireLineRectStartX, pageOffsetY+0,
                                                  (double) this->collider.w,
                                                  endPos.LineOffsetPixelY - this->tl.yOffset + 0.5);
                if (endPos.LineHeightPixel + endPos.LineOffsetPixelY - this->tl.yOffset > 0)
                    canvas.drawRectFilled(pageOffsetX+0, ay+endPos.LineOffsetPixelY - this->tl.yOffset,
                                                      endPos.LineOffsetPixelX,
                                                      endPos.LineHeightPixel);
            }

        } else {
            canvas.drawRectFilled(pageOffsetX+startPos.LineOffsetPixelX, ay+startPos.LineOffsetPixelY - this->tl.yOffset,
                                              endPos.LineOffsetPixelX - startPos.LineOffsetPixelX,
                                              startPos.LineHeightPixel);
        }
    }
    if (this->focus && start1To2 && end2ToE) {
        if (!PosLinEQ(startPos, endPos) || startPos.LineOffsetPixelY != endPos.LineOffsetPixelY) {
            {

                {
                    auto fireLineRectStartX = (pageOffsetX + startPos.LineOffsetPixelX);

                    auto w = (double) this->collider.w;

                    if (tl.xOffset<0 && -tl.xOffset > startPos.LineOffsetPixelX) {
                        fireLineRectStartX = ax;
                        w = this->collider.w;
                    }
                    canvas.drawRectFilled(fireLineRectStartX ,
                                          ay + startPos.LineOffsetPixelY - this->tl.yOffset,
                                          fireLineRectStartX+w,
                                          startPos.LineHeightPixel + 0.5);
                }
                auto fireLineRectStartX = pageOffsetX;
                if(tl.xOffset < 0) {
                    fireLineRectStartX = ax;
                }
                if (endPos.LineOffsetPixelY != startPos.LineOffsetPixelY + startPos.LineHeightPixel)
                    canvas.drawRectFilled(fireLineRectStartX, ay+startPos.LineOffsetPixelY - this->tl.yOffset +
                                                         startPos.LineHeightPixel,
                                                      (double) this->collider.w+std::max(fireLineRectStartX, 0.0),
                                                      (double) this->collider.h - startPos.LineHeightPixel +
                                                      this->tl.yOffset
                    );
            }

        } else {
            canvas.drawRectFilled(pageOffsetX+startPos.LineOffsetPixelX, ay+startPos.LineOffsetPixelY - this->tl.yOffset,
                                              endPos.LineOffsetPixelX - startPos.LineOffsetPixelX,
                                              startPos.LineHeightPixel);
        }
    }
    if (this->focus && startBTo1 && end2ToE) {

        canvas.drawRectFilled(pageOffsetX+0, pageOffsetY+0,
                                          (double) this->collider.w,
                                          (double) this->collider.h);
    }
    canvas.setFillStyle(0xFF000000);
    for (auto &E: strings) {
        std::string t = "\t";
        std::string bb = "    ";
        E = m_replace(E, t, bb, -1);
    }

    canvas.drawText(strings, this->tl.font, this->ax+this->tl.xOffset, this->ay+(-this->tl.yOffset), this->collider.w,
                       this->collider.h);


    int blinkE = tick / 500 % 2;
    static int64_t blinkTick = 0;
    if (this->tl.blink_time > 0) {
        blinkTick = tick + this->tl.blink_time * 1000;
        this->tl.blink_time = 0;
    }
    if (blinkTick > tick)
        blinkE = 1;

    if (blinkE == 1 && this->focus) {
        if (end1To2) {
            canvas.setStrokeWidth(2.0);

            canvas.setStrokeStyle(0xFFFF0000);
            TextLayout::PosEx Pos1;
            Pos1.pos.realLineNumber = this->tl.ViewEyeTextBeginLine;
            Pos1.pos.subLineNumber = this->tl.ViewEyeTextBeginSubLine;
            TextLayout::PosEx Pos2;
            Pos2.pos.realLineNumber = this->tl.ViewEyeTextEndLine;
            Pos2.pos.subLineNumber = this->tl.ViewEyeTextEndSubLine;
            if (PosLineGTE(this->tl.hitPos2, Pos1) && PosLineLTE(this->tl.hitPos2, Pos2))
            {
                canvas.drawLine(pageOffsetX+this->tl.hitPos2.LineOffsetPixelX,
                                pageOffsetY+this->tl.hitPos2.LineOffsetPixelY - this->tl.yOffset,
                                pageOffsetX+this->tl.hitPos2.LineOffsetPixelX,
                                pageOffsetY+this->tl.hitPos2.LineOffsetPixelY - this->tl.yOffset +
                                this->tl.hitPos2.LineHeightPixel);
            }

        }else if(end2ToE){
            canvas.setStrokeWidth(2.0);

            canvas.setStrokeStyle(0xFFFF0000);
            TextLayout::PosEx Pos1;
            Pos1.pos.realLineNumber = this->tl.ViewEyeTextBeginLine;
            Pos1.pos.subLineNumber = this->tl.ViewEyeTextBeginSubLine;
            TextLayout::PosEx Pos2;
            Pos2.pos.realLineNumber = this->tl.ViewEyeTextEndLine;
            Pos2.pos.subLineNumber = this->tl.ViewEyeTextEndSubLine;
            if (PosLineGTE(this->tl.hitPos2, Pos1) && PosLineLTE(this->tl.hitPos2, Pos2))
            {
                auto fireLineRectStartX = (pageOffsetX + startPos.LineOffsetPixelX);

                auto w = (double) this->collider.w;

                if (tl.xOffset<0 && -tl.xOffset > startPos.LineOffsetPixelX) {
                    fireLineRectStartX = ax;
                    w = this->collider.w;
                }
                canvas.drawLine(fireLineRectStartX,
                                pageOffsetY+this->tl.hitPos2.LineOffsetPixelY - this->tl.yOffset,
                                fireLineRectStartX,
                                pageOffsetY+this->tl.hitPos2.LineOffsetPixelY - this->tl.yOffset +
                                this->tl.hitPos2.LineHeightPixel);
            }
        }


    }
    this->drawBound();
    canvas.clipDrawEnd();
    return 0;
}

void EditBox::textboxOnClick(int x, int y) {
//    this->window->setCurControl(this);
    this->mouseLclickPosX = x;
    this->mouseLclickPosY = y;

    this->clickCollider = this->collider;
    this->mouseLState = true;
    int tinyLineNumber = 0;
    int tinyLineHeight = 0;
    int tinyLineYViewPos = 0;

}

void EditBox::onMouseMove(int x, int y) {

}

void EditBox::onMouseLUp(int x, int y) {
    this->mouseLState = false;
    this->tl.onLButtonUp(x - this->ax, y - this->ay);

}

void EditBox::onMouseLDown(int x, int y) {
//    SDL_GetWindowPosition(this->window->sdlWindow, &this->window->OldWindowPosX, &this->window->OldWindowPosY);
    SDL_StartTextInput();
    SDL_Rect rect;
    rect.x = this->collider.x;
    rect.y = this->collider.y;
    rect.w = this->collider.w;
    rect.h = this->collider.h;
    SDL_SetTextInputRect(&rect);
    this->tl.onLButtonDown(x - this->ax, y - this->ay);
}

void EditBox::onMouseRDown(int x, int y) {
    std::cout << "EditBox Mouse Up by Right EditBox" << std::endl;
}

void EditBox::onMouseRUp(int x, int y) {
    std::cout << "EditBox Mouse Up by Right EditBox" << std::endl;
}

void EditBox::onMouseMDown(int x, int y) {
    std::cout << "EditBox Mouse Down by middle EditBox" << std::endl;
}

void EditBox::onMouseMUp(int x, int y) {
    std::cout << "EditBox Mouse Up by middle EditBox" << std::endl;
}

void EditBox::onMouseWheel(int val) {
    int absVal = std::abs(val);
    for (int i = 0; i < absVal; i++) {
        this->tl.pageMovePixel(-val / absVal * 7);
    }
}

void EditBox::onMouseDrag(int x, int y, int gx, int gy) {
    this->tl.onDrag(x - this->ax, y - this->ay);
}

void EditBox::onKeyDown(int Key) {
    switch (Key) {
        case SDLK_UP: {
            this->tl.up_move();
            break;
        }
        case SDLK_LEFT: {
            this->tl.left_move();
            break;
        }
        case SDLK_DOWN: {
            this->tl.down_move();
            break;
        }
        case SDLK_RIGHT: {
            this->tl.right_move();
            break;
        }
        case SDLK_BACKSPACE: {
            this->tl.keyBackspace();
            break;
        }
        case SDLK_DELETE: {
            this->tl.keyDelete();
            break;
        }
        case SDLK_LCTRL:
        case SDLK_RCTRL: {
            this->ctrl = true;
            break;
        }
        case SDLK_c: {
            if (this->ctrl) {
                this->tl.copy();
            }
            else
            {

            }
            break;
        }
        case SDLK_v: {
            if (this->ctrl) {
                this->tl.paste();
            }
            else
            {

            }
            break;
        }
        case SDLK_a: {
            if (this->ctrl) {

            }
            else
            {

            }
            break;
        }
        case SDLK_x: {
            if (this->ctrl) {

            }
            else
            {

            }
            break;
        }
        case SDLK_y: {
            if (this->ctrl) {

            }
            else
            {

            }
            break;
        }
        case SDLK_z: {
            if (this->ctrl) {

            }
            else
            {

            }
            break;
        }
        default: {
            break;
        }
    }
}

void EditBox::onKeyUp(int Key) {
    switch (Key) {
        case SDLK_LCTRL:
        case SDLK_RCTRL: {
            this->ctrl = false;
            break;
        }
        default: {

        }
    }
}

void EditBox::onWindowResize(int newSizeW, int newSizeH) {
//	this->resize(newSizeW - 40, newSizeH - 40);
//	this->setSize(newSizeW - 40, newSizeH - 40);
//	this->tl.pageW = newSizeW - 40;
//	this->tl.pageH = newSizeH - 40;


//	this->tl.onResize();
}

void EditBox::onInputText(const std::string &str) {
    this->tl.input(str);
}

void EditBox::onFocus(bool focus_) {
    BaseControl::onFocus(focus_);
    this->setDirty(true);
}

void EditBox::setPos(int x, int y){
    GET_GUI();
    int oldax = this->ax;
    int olday = this->ay;
    int offsetX = x - this->collider.x;
    int offsetY = y - this->collider.y;
    this->collider.x = x;
    this->collider.y = y;
    this->ax = this->parent->ax + x;
    this->ay = this->parent->ay + y;
    this->calcClipRect();
    gui->wc.updateAABB((AABBKey)this, this->clipRect.l, this->clipRect.t, this->clipRect.r-this->clipRect.l, this->clipRect.b-this->clipRect.t);
}

void EditBox::setSize(int w, int h) {
    GET_GUI();
    this->collider.w = w;
    this->collider.h = h;
    this->calcClipRect();
    gui->wc.removeAABB((AABBKey) this);
    if (w <= 0 || h <= 0)
        gui->wc.insertAABB((AABBKey)this, -1, -1,1, 1);
    else
        gui->wc.insertAABB((AABBKey)this, this->ax, this->ay, w, h);
    gui->wc.updateAABB((AABBKey)this, this->clipRect.l, this->clipRect.t, this->clipRect.r-this->clipRect.l, this->clipRect.b-this->clipRect.t);
}
#endif

void UIEvent::evFinger(int action, int fingerID, float x, float y, float z){

}

void UIEvent::evQuit(){

}

void UIEvent::evKey(int action, int key){
    auto gui = ChainGui::get();
    if (!gui->curControl)
        return;
    if(action == ACTION_DOWN)
        gui->curControl->onKeyDown(key);
    else if(action == ACTION_UP)
        gui->curControl->onKeyUp(key);
}

void UIEvent::evMouse(int action, int button, int x, int y){
    auto gui = ChainGui::get();
    if(action == ACTION_MOVE){
        if(gui->curControl) {
            if(gui->curControl->isPress){
                int newX = 0;
                int newY = 0;
                SDL_GetGlobalMouseState(&newX, &newY);
                gui->curControl->onMouseDrag(x, y, newX, newY);
            }

        }

    }else if(action == ACTION_DOWN){
        SDL_CaptureMouse(SDL_TRUE);
        if(button == BUTTON_LEFT){

            auto control = (BaseControl *) gui->getWidgetByPoint(x, y);
            if(gui->curControl != control)
            {
                if(gui->curControl)
                    gui->curControl->onFocus(false);
                gui->curControl = control;
                if(control){
                    gui->curControl->onFocus(true);
                }
            }
            if(gui->curControl){
                gui->curControl->drawBound_ = true;
                gui->curControl->isPress = true;
                SDL_GetGlobalMouseState(&gui->curControl->pressPosX, &gui->curControl->pressPosY);
                gui->curControl->onMouseLDown(x, y);
            }

        }
    }else if(action == ACTION_UP){
        SDL_CaptureMouse(SDL_FALSE);
//        if(gui->curControl == NULL){
//            gui->curControl = (BaseControl *) gui->getWidgetByPoint(x, y);
//            if(gui->curControl)
//                gui->curControl->drawBound_ = true;
//        }
        if(button == BUTTON_LEFT){
            if(gui->curControl) {
                gui->curControl->isPress = false;
                gui->curControl->onMouseLUp(x, y);
            }
        }

    }

}

void UIEvent::evInput(const std::string &str){
    auto gui = ChainGui::get();
    if (!gui->curControl)
        return;

    gui->curControl->onInputText(str);

}

void UIEvent::evWindowResize(int w, int h){
    auto gui = ChainGui::get();
    gui->setCanvasSize(w, h);
}
