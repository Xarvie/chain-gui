//
// Created by Administrator on 2022/5/18.
//

#include "UIControl.h"
#include "ChainGui.h"

int idGen() {
    static uint64_t id = 0;
    id++;
    return id;
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
    this->color = 0x00000000;
    this->BaseControl::init(parent);
    return 0;
}

Box *Box::create(Box *parent, int x, int y, int w, int h) {

    return nullptr;
}

int Box::setColor(UIColor color) {
    this->color = color;
    this->setDirty(true);
    return 0;
}

int Box::draw(int64_t tick) {
    return 0;
}

int Box::resize(int w, int h) {
    auto gui = ChainGui::get();
    this->collider.w = w;
    this->collider.h = h;
    gui->canvas.data.w = w;
    gui->canvas.data.h = h;
    return 0;
}

Window *Window::create(Box *parent, const std::string &title, int x, int y, int w, int h) {
    auto *self = new Window;
    self->init(parent, x, y, w, h);
    return self;
}

int Window::init(Box *parent, int x, int y, int w, int h) {
    auto gui = ChainGui::get();
    if (parent == NULL) {
        gui->rootWindows.insert(this);
    }
    return Box::init(parent, x, y, w, h);
}

int Window::draw(int64_t tick) {
    auto gui = ChainGui::get();

    gui->canvas.setFillStyle(0xFF0000FF);
    gui->canvas.drawRectFilled(this->collider.x, this->collider.y, this->collider.w, this->collider.h);

    gui->canvas.setFillStyle(0xFF00FF00);
    gui->canvas.drawRectFilled(this->collider.x, this->collider.y, this->collider.w, 20);

    gui->canvas.setFillStyle(0xFFF20202);
    gui->canvas.drawCircleFilled(this->collider.x + this->collider.w - 10.0, this->collider.y + 10.0, 5);


    Box::draw(tick);
    drawBound();
    return 0;
}

int BaseControl::init(Box *parent) {
    auto gui = ChainGui::get();
    this->layer = 0;
    this->id = idGen();
    this->parent = parent;
    if (parent != nullptr) {
        parent->childControlMap.insert(this);
    }

    this->ax = (parent ? parent->ax : 0) + this->collider.x;
    this->ay = (parent ? parent->ay : 0) + this->collider.y;

    gui->wc.insertAABB((AABBKey) this, this->ax, this->ay, this->collider.w, this->collider.h);
    return 0;
}

int BaseControl::setDirty(bool dirty_) {
    auto gui = ChainGui::get();
    gui->dirty = dirty_;
    return 0;
}

int BaseControl::draw(int64_t tick) {
    return 0;
}

void BaseControl::drawBound() {
    auto gui = ChainGui::get();
    if (!gui->drawBound)
        return;

    gui->canvas.setStrokeStyle(0xFF00FF00);
    gui->canvas.drawRect(this->collider.x, this->collider.y, this->collider.w, this->collider.h);
}

void BaseControl::setPos(int x, int y) {

}

void BaseControl::setSize(int w, int h) {

}

