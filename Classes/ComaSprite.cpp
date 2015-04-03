//
//  ComaSprite.cpp
//  puzzle
//
//  Created by Avirmed Munkhbat on 9/3/14.
//
//

#include "ComaSprite.h"

USING_NS_CC;

ComaSprite::ComaSprite()
: positionIndex(PositionIndex(0, 0)){
    initNextPos();
}

void ComaSprite::initNextPos() {
    this->nextPosY = BOARD_INF;
    this->nextPosX = BOARD_INF;
}

ComaSprite::~ComaSprite(){}

ComaSprite* ComaSprite::createWithComaType(ComaType comaType) {
    ComaSprite* sprite = new ComaSprite();
    if (sprite && sprite->initWithComaType(comaType)) {
        sprite->autorelease();
        return sprite;
    } else {
        CC_SAFE_DELETE(sprite);
        return nullptr;
    }
}

bool ComaSprite::initWithComaType(ComaType comaType) {
    if (!Sprite::initWithFile(getComaFileName(comaType))) {
        return false;
    }
    
    this->comaType = comaType;
    return true;
}

const std::string ComaSprite::getComaFileName(ComaType comaType) {
    switch (comaType) {
        case ComaType::Blue:
            return "gi.png";
            break;
        case ComaType::Gray:
            return "gun.png";
            break;
        case ComaType::Green:
            return "shoku.png";
            break;
        case ComaType::Pink:
            return "peach.png";
            break;
        case ComaType::Purple:
            return "kan.png";
            break;
        case ComaType::Red:
            return "go.png";
            break;
        default:
            CCLOG("invalid coma type");
            assert(false);
            return "";
    }
}

void ComaSprite::setNextPos(int nextPosX, int nextPosY) {
    this->nextPosX = nextPosX;
    this->nextPosY = nextPosY;
}

int ComaSprite::getNextPosY() {
    return this->nextPosY;
}

int ComaSprite::getNextPosX() {
    return this->nextPosX;
}

ComaSprite* ComaSprite::createAttakingComa(const std::string fileName) {
    ComaSprite* sprite = new ComaSprite();
    if (sprite && sprite->initWithFileName(fileName)) {
        sprite->autorelease();
        return sprite;
    } else {
        CC_SAFE_DELETE(sprite);
        return nullptr;
    }
}

bool ComaSprite::initWithFileName(const std::string fileName) {
    if (!Sprite::initWithFile(fileName)) {
        return false;
    }
    
    this->comaType = comaType;
    return true;
}
