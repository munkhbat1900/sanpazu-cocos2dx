//
//  BoardSprite.cpp
//  puzzle
//
//  Created by Avirmed Munkhbat on 9/4/14.
//
//

#include "BoardSprite.h"

USING_NS_CC;

BoardSprite::BoardSprite(){
}

BoardSprite::~BoardSprite(){}

BoardSprite* BoardSprite::createBoardSprite(std::string imageFileName) {
    BoardSprite* sprite = new BoardSprite();
    if (sprite && sprite->initWithFileName(imageFileName)) {
        sprite->autorelease();
        return sprite;
    } else {
        CC_SAFE_DELETE(sprite);
        return nullptr;
    }
}

bool BoardSprite::initWithFileName(std::string imageFileName) {
    if (!Sprite::initWithFile(imageFileName)) {
        return false;
    }
    return true;
}

void BoardSprite::initComa() {
    for (int x = 1; x <= BOARD_X; x++) {
        for (int y = 1; y <= BOARD_Y; y++) {
            auto comaSprite = createNewComaByComaType(getComaType(ComaSprite::PositionIndex(x, y)), ComaSprite::PositionIndex(x, y));
            this->addChild(comaSprite, static_cast<int>(kZOrder::coma), computeComaTag(ComaSprite::PositionIndex(x, y)));
            allComaSpriteTagMap.insert(computeComaTag(ComaSprite::PositionIndex(x, y)), comaSprite);
        }
    }
}

ComaType BoardSprite::getComaType(ComaSprite::PositionIndex positionIndex) {
     //最初からパズルが成功していないように配置する。
    int comaType;
    bool b = true;
    while (b) {
        comaType = rand() % static_cast<int>(ComaType::Size);
        // 左端ではなければ、左側に同類のコマが配置されていないようにする。左側が既に配置されている。
        if (positionIndex.x > 1) {
            ComaSprite* leftComaSprite = nullptr;
            for (Node* node : this->getChildren()) {
                if (computeComaTag(ComaSprite::PositionIndex(positionIndex.x - 1, positionIndex.y)) == node->getTag()) {
                    leftComaSprite = (ComaSprite*)node;
                }
            }
            if (leftComaSprite->getComaType() != (ComaType)comaType) {
                b = false;
            } else {
                b = true;
            }
        } else {
            b = false;
        }
        // 下端ではなければ、下のコマと同類のコマが配置されないようにする
        if (positionIndex.y > 1 && !b) {
            ComaSprite* belowComaSprite = nullptr;
            for (Node* node : this->getChildren()) {
                if (computeComaTag(ComaSprite::PositionIndex(positionIndex.x, positionIndex.y - 1)) == node->getTag()) {
                    belowComaSprite = (ComaSprite*)node;
                }
            }
            if (belowComaSprite->getComaType() == (ComaType)comaType) {
                b = true;
            } else {
                b = false;
            }
        }
    }
    return (ComaType)comaType;
}

ComaSprite* BoardSprite::createNewComaByComaType(ComaType comaTyp, ComaSprite::PositionIndex positionIndex) {
    ComaSprite* comaSprite = ComaSprite::createWithComaType((ComaType)comaTyp);
    comaSprite->setPosition(getComaPosition(positionIndex));
    comaSprite->setComaType(comaTyp);
    comaSprite->setTag(computeComaTag(positionIndex));
    comaSprite->setPositionIndex(positionIndex);
    return comaSprite;
}

ComaSprite* BoardSprite::createNewComa(ComaSprite::PositionIndex positionIndex) {

    int comaType = rand() % static_cast<int>(ComaType::Size);
    ComaSprite* comaSprite = ComaSprite::createWithComaType((ComaType)comaType);
    comaSprite->setPosition(getComaPosition(positionIndex));
    comaSprite->setComaType((ComaType)comaType);
    comaSprite->setTag(computeComaTag(positionIndex));
    return comaSprite;
}

int BoardSprite::computeComaTag(ComaSprite::PositionIndex positionIndex) {
    int y = positionIndex.y;
    if (positionIndex.y < 1) {
        y = BOARD_Y + positionIndex.y * (-1) + 1;
    }
    return COMA_BASE_TAG + positionIndex.x * 1000 + y;
}

Vec2 BoardSprite::getComaPosition(ComaSprite::PositionIndex positionIndex) {
    // マスのサイズを計算。
    float squareSizeX = this->getContentSize().width / BOARD_X;
    float squareSizeY = this->getContentSize().height / BOARD_Y;
    
    int x = squareSizeX * positionIndex.x - squareSizeX / 2;
    int y = squareSizeY * positionIndex.y - squareSizeY / 2;
    return Vec2(x, y);
}

ComaSprite* BoardSprite::getPassedComa(cocos2d::Vec2 passedPoint) {
    ComaSprite::PositionIndex positionIndex = getPositionIndex(passedPoint);
    
    //Node* passedComaSprite = this->getChildByTag(computeComaTag(positionIndex));
    ComaSprite* passedComaSprite = nullptr;
    for (Node* node : this->getChildren()) {
        if (computeComaTag(positionIndex) == node->getTag()) {
            passedComaSprite = (ComaSprite*)node;
        }
    }
    
    return (ComaSprite*)passedComaSprite;
}

ComaSprite* BoardSprite::getSelectedComaAndMoveLittle(cocos2d::Vec2 touchPoint) {
    // バズル領域以外の所がタップされていれば、
    if (touchPoint.x < 0 || touchPoint.y < 0) {
        CCLOG("0以下");
        return nullptr;
    }
    
    if (touchPoint.x > this->getContentSize().width || touchPoint.y > this->getContentSize().height) {
        CCLOG("最大以上");
        return nullptr;
    }
    
    ComaSprite::PositionIndex positionIndex = getPositionIndex(touchPoint);
    
    ComaSprite* selectedComaSprite;
    for (Node* node : this->getChildren()) {
        if (computeComaTag(positionIndex) == node->getTag()) {
            selectedComaSprite = (ComaSprite*)node;
        }
    }
    // TODO 下記がNULLになるので調査必要。
    //ComaSprite* selectedComaSprite = (ComaSprite*)this->getChildByTag(computeComaTag(positionIndex));
    
    if (selectedComaSprite == nullptr) {
        CCLOG("cannot get selected sprite");
        return nullptr;
    }
    
    selectedComaSprite->setPosition(Vec2(touchPoint.x, touchPoint.y + selectedComaSprite->getContentSize().height / 3));
    return selectedComaSprite;
}

void BoardSprite::searchNewPosition(const std::vector<int> &removedComas) {
    for (int tag : removedComas) {
        ComaSprite::PositionIndex posIndex = getPositionIndexFromTag(tag);
        int xIndex = posIndex.x;
        int yIndex = posIndex.y;
        for (auto pair : allComaSpriteTagMap) {
            ComaSprite::PositionIndex positionIndex = getPositionIndexFromTag(pair.first);
            
            if (xIndex == positionIndex.x && yIndex > positionIndex.y) {
                setNewPosition(positionIndex);
            }
        }
    }
}

void BoardSprite::setNewPosition(ComaSprite::PositionIndex positionIndex) {
    ComaSprite* comaSprite = allComaSpriteTagMap.at(computeComaTag(positionIndex));
    
    int nextPosY = comaSprite->getNextPosY();
    if (nextPosY == BOARD_INF) {
        nextPosY = positionIndex.y;
    }
    
    nextPosY++;
    
    comaSprite->setNextPos(positionIndex.x, nextPosY);
}

void BoardSprite::moveComaAnimation(const std::vector<int> &removedComas) {
    searchNewPosition(removedComas);
    for (auto pair: allComaSpriteTagMap) {
        ComaSprite* comaSprite = pair.second;
        int nextPosY = comaSprite->getNextPosY();
        int nextPosX = comaSprite->getNextPosX();
        
        // 移動対象のコマか
        if (nextPosX != BOARD_INF && nextPosY != BOARD_INF) {
            int newTag = computeComaTag(ComaSprite::PositionIndex(nextPosX, nextPosY));
            comaSprite->setTag(newTag);
            comaSprite->setPositionIndex(ComaSprite::PositionIndex(nextPosX, nextPosY));
            
            // アニメーションをセットする。
            Vec2 pos = getComaPosition(ComaSprite::PositionIndex(nextPosX, nextPosY));
            MoveTo* move = MoveTo::create(COMA_MOVING_TIME, getComaPosition(ComaSprite::PositionIndex(nextPosX, nextPosY)));
            
            comaSprite->setNextPos(BOARD_INF, BOARD_INF);
            comaSprite->runAction(move);
        }
    }
    
    allComaSpriteTagMap.clear();

    for (int x = 1; x <= BOARD_X; x++) {
        for (int y = 1; y <= BOARD_Y; y++) {
            ComaSprite* sprite = nullptr;
            int tag = computeComaTag(ComaSprite::PositionIndex(x, y));
            for (Node* node : this->getChildren()) {
                if (tag == node->getTag()) {
                    sprite = (ComaSprite*)node;
                }
            }
            allComaSpriteTagMap.insert(tag, sprite);
        }
    }
}

void BoardSprite::initNewComa(const std::vector<int> &removedComas) {
    // 新しいコマの
    int ycounter[BOARD_X + 1];
    
    for (int i = 1; i <= BOARD_X; i++) {
        ycounter[i] = 0;
    }
    
    for (auto tag : removedComas) {
        ComaSprite::PositionIndex positionIndex = getPositionIndexFromTag(tag);
        
        // 新規コマの作成。
        ComaSprite::PositionIndex newPositionIndex = ComaSprite::PositionIndex(positionIndex.x, ycounter[positionIndex.x]);
        ComaSprite* newComaSprite = createNewComa(newPositionIndex);
        this->addChild(newComaSprite, static_cast<int>(kZOrder::coma), computeComaTag(newPositionIndex));
        ycounter[positionIndex.x] = ycounter[positionIndex.x] - 1;
        allComaSpriteTagMap.insert(computeComaTag(newPositionIndex), newComaSprite);
    }
}

void BoardSprite::removeComa(int tag) {
    allComaSpriteTagMap.erase(tag);
}

ComaSprite::PositionIndex BoardSprite::getPositionIndexFromTag(int tag) {
    int xIndex = (tag - COMA_BASE_TAG) / 1000;
    int yIndex = (tag - COMA_BASE_TAG) % 100;
    int y = yIndex;
    if (yIndex > BOARD_Y) {
        y = BOARD_Y - yIndex + 1;
    }
    return ComaSprite::PositionIndex(xIndex, y);
}

ComaSprite::PositionIndex BoardSprite::getPositionIndex(cocos2d::Vec2 touchPoint) {
    
    // マスのサイズを計算。
    float squareSizeX = this->getContentSize().width / BOARD_X;
    float squareSizeY = this->getContentSize().height / BOARD_Y;
    
    int xIndex = touchPoint.x / squareSizeX + 1;
    int yIndex = touchPoint.y / squareSizeY + 1;
    
    
    
    // 境界を超えていれば、境界にする。
    xIndex = xIndex > BOARD_X ? BOARD_X : xIndex;
    xIndex = xIndex < 1 ? 1 : xIndex;
    yIndex = yIndex > BOARD_Y ? BOARD_Y : yIndex;
    yIndex = yIndex < 1 ? 1 : yIndex;
        
    return ComaSprite::PositionIndex(xIndex, yIndex);
}

void BoardSprite::changePosition(ComaSprite *passedComa, ComaSprite *selectedComa) {
    int passedComaTag = computeComaTag(passedComa->getPositionIndex());
    ComaSprite::PositionIndex passedComaPositionIndex = passedComa->getPositionIndex();
    
    passedComa->setTag(computeComaTag(selectedComa->getPositionIndex()));
    passedComa->setPositionIndex(selectedComa->getPositionIndex());
    
    selectedComa->setTag(passedComaTag);
    selectedComa->setPositionIndex(passedComaPositionIndex);
    
    allComaSpriteTagMap.insert(selectedComa->getTag(), selectedComa);
    allComaSpriteTagMap.insert(passedComa->getTag(), passedComa);
    
    comaExchangeAnimation(passedComa, selectedComa);
}

void BoardSprite::comaExchangeAnimation(ComaSprite *passedComa, ComaSprite *selectedComa) {
    /* コマの入れ替えアニメーション */
    if (passedComa->getNumberOfRunningActions() > 0) {
        passedComa->stopAllActions();
        passedComa->setPosition(getComaPosition(passedComa->getPositionIndex()));
    }
    
    // マスのサイズを計算。
    float squareSizeX = this->getContentSize().width / BOARD_X;
    float squareSizeY = this->getContentSize().height / BOARD_Y;
    
    ComaSprite::PositionIndex passedComaPositionIndex = passedComa->getPositionIndex();
    ComaSprite::PositionIndex selectedComaPositionIndex = selectedComa->getPositionIndex();
    
    Direction exchangeDirection;
    if (passedComaPositionIndex.x != selectedComaPositionIndex.x) {
        exchangeDirection = Direction::x;
    } else if(passedComaPositionIndex.y != selectedComaPositionIndex.y) {
        exchangeDirection = Direction::y;
    }
    
    Vec2 controlPoint1 = getComaPosition(selectedComa->getPositionIndex());
    Vec2 controlPoint2 = getComaPosition(passedComa->getPositionIndex());
    if (exchangeDirection == Direction::x) {
        controlPoint1 = Vec2(controlPoint1.x, controlPoint1.y - squareSizeY / 2);
        controlPoint2 = Vec2(controlPoint2.x, controlPoint2.y - squareSizeY / 2);
    } else {
        controlPoint1 = Vec2(controlPoint1.x + squareSizeX / 2, controlPoint1.y);
        controlPoint2 = Vec2(controlPoint2.x + squareSizeX / 2, controlPoint2.y);
    }
    
    ccBezierConfig bezier;
    bezier.endPosition = getComaPosition(passedComa->getPositionIndex());
    bezier.controlPoint_1 = controlPoint1;
    bezier.controlPoint_2 = controlPoint2;
    auto delay = DelayTime::create(COMA_EXCHANGE_TIME);
    auto action = BezierTo::create(COMA_EXCHANGE_TIME, bezier);
    FiniteTimeAction* sequence = Sequence::create(delay, action, NULL);
    passedComa->runAction(sequence);
}

void BoardSprite::setComaPosition() {
    for (int x = 1; x <= BOARD_X; x++) {
        for (int y = 1; y <= BOARD_Y; y++) {
            ComaSprite* sprite = nullptr;
            int tag = computeComaTag(ComaSprite::PositionIndex(x, y));
            for (Node* node : this->getChildren()) {
                if (tag == node->getTag()) {
                    sprite = (ComaSprite*)node;
                }
            }
            sprite->setPosition(getComaPosition(sprite->getPositionIndex()));
            sprite->setZOrder(static_cast<int>(kZOrder::coma));
        }
    }
}

void BoardSprite::showLabel(const std::vector<int> &removeComaTags) {
    _attackCounter++;
    bool flag = true;
    for (auto tag : removeComaTags) {
        ComaSprite::PositionIndex positionIndex = getPositionIndexFromTag(tag);
        int labelTag = LABEL_BASE_TAG + 10 * positionIndex.x + positionIndex.y;
        auto label = (Label*)this->getChildByTag(labelTag);
        
        if (label != nullptr) {
            continue;
        }
        
        if (_attackCounter > 1 && flag) {
            // マスのサイズを計算。
            float squareSizeY = this->getContentSize().width / BOARD_Y;
            Vec2 comaPosition = getComaPosition(positionIndex);
            
            auto labelAttackCountText = StringUtils::format("%d\n", _attackCounter);
            Vec2 attackCountLabelPos = Vec2(comaPosition.x, comaPosition.y + squareSizeY / 6);
            auto attackCountLabel = Label::createWithSystemFont(labelAttackCountText, "arial", 40);  //システムフォントは重いそうです。
            attackCountLabel->setPosition(attackCountLabelPos);
            this->addChild(attackCountLabel, 3, labelTag);
            successNumberLabelVector.pushBack(attackCountLabel);
            
            Vec2 attackTextLabelPos = Vec2(comaPosition.x, comaPosition.y - squareSizeY / 6);
            auto attackTextLabel = Label::createWithSystemFont("連撃", "arial", 40);  //システムフォントは重いそうです。
            attackTextLabel->setPosition(attackTextLabelPos);
            this->addChild(attackTextLabel);
            attackTextLabel->setZOrder(3);
            successNumberLabelVector.pushBack(attackTextLabel);
            
            flag = false;
        }
    }
}

void BoardSprite::deleteAllLabel() {
    for (auto attackLabel: successNumberLabelVector) {
        attackLabel->removeFromParentAndCleanup(true);
    }
    successNumberLabelVector.clear();
    _attackCounter = 0;
}



