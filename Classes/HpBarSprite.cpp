//
//  HpBarSprite.cpp
//  puzzle
//
//  Created by Avirmed Munkhbat on 9/4/14.
//
//

#include "HpBarSprite.h"
#include "enums.h"

USING_NS_CC;

HpBarSprite::HpBarSprite(){
    
}

HpBarSprite::~HpBarSprite(){}

HpBarSprite* HpBarSprite::createHpBar(std::string imageFileName) {
    HpBarSprite* sprite = new HpBarSprite();
    if (sprite && sprite->initWithFileName(imageFileName)) {
        sprite->autorelease();
        auto drawNode1 = DrawNode::create();
        sprite->addChild(drawNode1, 1, 1);
        sprite->initTimeBarSize();
        return sprite;
    } else {
        CC_SAFE_DELETE(sprite);
        return nullptr;
    }
}

bool HpBarSprite::initWithFileName(std::string imageFileName) {
    if (!Sprite::initWithFile(imageFileName)) {
        return false;
    }
    return true;
}

void HpBarSprite::initTimeBarSize() {
    float squareSizeX = this->getContentSize().width;
    float squareSizeY = this->getContentSize().height;
    
    _timeBarStartPoint = 50;
    _timeBarWidth = squareSizeX - _timeBarStartPoint - 10;
    _timeBarHeight = squareSizeY - 5;
    
    _timeBarShrinkSpeed = (_timeBarWidth /  PUZZLE_TIME) / 2;
}

float HpBarSprite::getTimeBarLength(float seconds) {
    return PUZZLE_TIME * _timeBarWidth / seconds;
}

void HpBarSprite::animateTimeBar(float delta) {
    if ((int)delta >= PUZZLE_TIME) {
        return;
    }
    float width = _timeBarWidth - _timeBarShrinkSpeed * delta;
    float startPoint = _timeBarStartPoint + _timeBarShrinkSpeed * delta;
    Vec2 timerAnimationRect[4] = {
        Vec2(width + _timeBarStartPoint, _timeBarHeight),
        Vec2(startPoint, _timeBarHeight),
        Vec2(startPoint, 0 + 5),
        Vec2(width + _timeBarStartPoint, 0 + 5)
    };
    auto drawNode1 = dynamic_cast<DrawNode*>(getChildByTag(1));
    drawNode1->clear();
    drawNode1->drawPolygon(timerAnimationRect,       // 座標データ
                           4,        // 角数
                           Color4F(0, 255, 255, 1.0f),
                           1,                 // 線の太さ
                           Color4F(0, 255, 255, 1.0f));  //枠線の色。
}

void HpBarSprite::stopTimeAnimation() {
    CCLOG("stopTimeAnimation");
    auto node = dynamic_cast<DrawNode*>(getChildByTag(1));
    node->getActionManager()->removeAllActions();
    node->clear();
    //node->removeFromParentAndCleanup(true);
    _isAnimating = false;
}