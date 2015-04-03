#include "GameLayer.h"
#include <chrono>
#include <thread>

USING_NS_CC;

Scene* GameLayer::createScene() {
    auto scene = Scene::create();
    
    auto layer = GameLayer::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

bool GameLayer::init() {
    if (!Layer::init()) {
        return false;
    }
    
    // 乱数初期化
    srand((unsigned)time(nullptr));
    
    // 画面構築。
    constructScreen();
    // コマを配置
    initComa();
    
    // タッチイベント
    auto eventListener = EventListenerTouchOneByOne::create();
    eventListener->onTouchBegan = onTouchBegan();
    eventListener->onTouchEnded = onTouchEnded();
    eventListener->onTouchMoved = onTouchMoved();
    _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
    
    //毎フレームupdateメソッドの呼び出し。
    scheduleUpdate();
    
    return true;
}

void GameLayer::update(float delta) {
    if (_isComaMoving && (_puzzleTime < PUZZLE_TIME)) {
        _puzzleTime += delta;
        _hpBarSprite->animateTimeBar(_puzzleTime);
    }
    if (_isComaMoving && (_puzzleTime >= PUZZLE_TIME)) {
        CCLOG("seconds = %d", (int)_puzzleTime);
        _isComaMoving = false;
        puzzleEnd();
    }
}

void GameLayer::constructScreen() {
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 visibleOrigin = Director::getInstance()->getVisibleOrigin();
    
    
    // エネミー領域と味方キャラクタをテクスチャとして配置。画面の上半分を
    Sprite* enemyAreaSprite = Sprite::create(enemyImageFileName);
    enemyAreaSprite->cocos2d::Node::setPosition(Vec2(visibleSize.width / 2, 3 * visibleSize.height / 4 - 7));
    enemyAreaSprite->setScaleY(1.08f);
    this->addChild(enemyAreaSprite, static_cast<int>(kZOrder::background), static_cast<int>(kTag::enemyArea));
    
    // HP バーを表示
    HpBarSprite* hpBarSprite = HpBarSprite::createHpBar(hpBarImageFileName);
    hpBarSprite->cocos2d::Node::setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 - 35));
    this->addChild(hpBarSprite, static_cast<int>(kZOrder::background), static_cast<int>(kTag::HpBarArea));
    this->_hpBarSprite = hpBarSprite;
    
    BoardSprite* boardSprite = BoardSprite::createBoardSprite(puzzleBoardFileName);
    boardSprite->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 4));
    this->addChild(boardSprite, static_cast<int>(kZOrder::background), static_cast<int>(kTag::Board));
    
    this->puzzleBoardSprite = boardSprite;
}

void GameLayer::initComa() {
    puzzleBoardSprite->initComa();
}

std::function<void(cocos2d::Touch*, cocos2d::Event*)> GameLayer::onTouchMoved() {
    return [&] (cocos2d::Touch* touch, cocos2d::Event* event) {
        
        if (selectedComa && (_puzzleTime < PUZZLE_TIME)) {
            selectedComa->setPosition(selectedComa->getPosition() + touch->getDelta());
            
            Size size = puzzleBoardSprite->getContentSize();
            Vec2 touchPoint = puzzleBoardSprite->convertTouchToNodeSpace(touch);
            
            ComaSprite* passedComa = puzzleBoardSprite->getPassedComa(touchPoint);
            if (passedComa != selectedComa) {
                _isComaMoving = true;
                puzzleBoardSprite->changePosition(passedComa, selectedComa);
            }
        } else if (selectedComa && !_isComaMoving) {
            puzzleEnd();
        }
    };
}

std::function<bool(cocos2d::Touch*, cocos2d::Event*)> GameLayer::onTouchBegan() {
    return [&](cocos2d::Touch* touch, cocos2d::Event* event) {
        if (!touchable) {
            return false;
        }
        Vec2 touchPoint = puzzleBoardSprite->convertTouchToNodeSpace(touch);
        selectedComa = puzzleBoardSprite->getSelectedComaAndMoveLittle(touchPoint);
        
        if (selectedComa == nullptr) {
            return false;
        }
        touchable = false;
        selectedComa->setZOrder(static_cast<int>(kZOrder::selectedComa));
        return true;
    };
}

std::function<void(cocos2d::Touch*, cocos2d::Event*)> GameLayer::onTouchEnded() {
    return [&] (cocos2d::Touch* touch, cocos2d::Event* event) {
        puzzleEnd();
    };
}

void GameLayer::puzzleEnd() {
    if (selectedComa == nullptr) {
        return;
    }
    puzzleBoardSprite->setComaPosition();
    
    selectedComa = nullptr;
    _puzzleTime = 0.0f;
    _isComaMoving = false;
    _hpBarSprite->stopTimeAnimation();
    _hpBarSprite->initTimeBarSize();
    
    findSuccessAndRemove();
    successCount = 0;
}

void GameLayer::checkPuzzleByAxis(Direction direction, int x, int y) {
    int tag = puzzleBoardSprite->computeComaTag(ComaSprite::PositionIndex(x, y));
    //既にチェックしているコマなら省く。
    if (Direction::x == direction && tagComaCheckMap.at(tag).first) {
        return;
    } else if (Direction::y == direction && tagComaCheckMap.at(tag).second) {
        return;
    }
    
    // 同類の連続コマの個数
    int continousComaCounter {1};
    
    // 同類のコマが3個以上並んでいるか
    while (true) {
        ComaSprite::PositionIndex nextComaPositionIndex;
        if (Direction::x == direction) {
            nextComaPositionIndex = ComaSprite::PositionIndex(x + continousComaCounter, y);
            
        } else if (Direction::y == direction) {
            nextComaPositionIndex = ComaSprite::PositionIndex(x, y + continousComaCounter);
        }
        
        // 同類かどうかをチェック。
        if (nextComaPositionIndex.x <= BOARD_X && nextComaPositionIndex.y <= BOARD_Y && tagComaTypeMap.at(tag) == tagComaTypeMap.at(puzzleBoardSprite->computeComaTag(nextComaPositionIndex))) {
            // 次の玉をチェックしたことにする。
            if (Direction::x == direction) {
                // x軸に沿ってチェックしたことにする。
                tagComaCheckMap.insert(std::make_pair(tag, std::make_pair(true, tagComaCheckMap.at(tag).second)));
            } else if (Direction::y == direction) {
                // y軸に沿ってチェックしたことにする。
                tagComaCheckMap.insert(std::make_pair(tag, std::make_pair(tagComaCheckMap.at(tag).first, true)));
            }
            continousComaCounter++;
        } else {
            if (continousComaCounter >= SUCCESS_COUNT) {
                // パズル成功数カウンタインクリメント
                successCount++;
                
                // 何番目のサクセスかを表すカウンタ。最初はあり得ない大きい数値を入れておく
                int successNumber = successCount;
                
                for (int i = 0; i < continousComaCounter; i++) {
                    int xx = x;
                    int yy = y;
                    if (Direction::x == direction) {
                        xx += i;
                    } else {
                        yy += i;
                    }
                    
                    try {
                        int tmp = successComaMap.at(puzzleBoardSprite->computeComaTag(ComaSprite::PositionIndex(xx, yy)));
                        if (successNumber > tmp) {
                            successNumber = tmp;
                        }
                    } catch (std::out_of_range) {
                    }
                }
                
                int previosSuccessNumber = successNumber;
                for (int i = 0; i < continousComaCounter; i++) {
                    ComaSprite::PositionIndex positionIndex;
                    if (Direction::x == direction) {
                        positionIndex = ComaSprite::PositionIndex(x + i, y);
                    } else {
                        positionIndex = ComaSprite::PositionIndex(x, y + i);
                    }
                    int tmpTag = puzzleBoardSprite->computeComaTag(positionIndex);
                    try {
                        if (successNumber < successComaMap.at(tmpTag)) {
                            previosSuccessNumber = successComaMap.at(tmpTag);
                        }
                    } catch (std::out_of_range) {
                    }
                    
                    successComaMap[tmpTag] = successNumber;
                }
                
                // 既存のsuccessNumberがあれば、successNumberより大きければ、全コマに対してそれをおきかえる。
                if (previosSuccessNumber > successNumber) {
                    for (auto pair : successComaMap) {
                        if (pair.second == previosSuccessNumber) {
                            successComaMap[pair.first] = successNumber;
                        }
                    }
                }
            }
            break;
        }
    }
}

void GameLayer::findSuccessAndRemove() {
    successComaMap.clear();
    removedComas.clear();
    removeComaTags.clear();
    tagComaTypeMap.clear();
    tagComaCheckMap.clear();
    touchable = false;
    successCounter = 0;
    for (int x = 1; x <= BOARD_X; x++) {
        for (int y = 1; y <= BOARD_Y; y++) {
            
            // スプライトを取得
            ComaSprite* sprite = nullptr;
            int tag = puzzleBoardSprite->computeComaTag(ComaSprite::PositionIndex(x, y));
            for (Node* node : puzzleBoardSprite->getChildren()) {
                if (tag == node->getTag()) {
                    sprite = (ComaSprite*)node;
                }
            }
            tagComaTypeMap.insert(std::make_pair(tag, sprite->getComaType()));
            //tagComaSpriteMap.insert(tag, sprite);
            tagComaCheckMap.insert(std::make_pair(tag, std::make_pair(false, false)));
        }
    }
    
    for (int x = 1; x <= BOARD_X; x++) {
        for (int y = 1; y <= BOARD_Y; y++) {
            checkPuzzleByAxis(Direction::x, x, y);
            checkPuzzleByAxis(Direction::y, x, y);
        }
    }
    
    if (successComaMap.empty()) {
        touchable = true;
        puzzleBoardSprite->deleteAllLabel();
        return;
    }
    
    initializeQueue();
    for (auto pair : successComaMap) {
        removedComas.push_back(pair.first);
    }
    
    auto func1 = CallFuncN::create(CC_CALLBACK_0(GameLayer::removeNext, this));
    auto func2 = CallFuncN::create(CC_CALLBACK_0(GameLayer::findSuccessAndRemove, this));
    auto delay = DelayTime::create((COMA_REMOVE_TIME + DELTA + 0.05f) * successCounter + COMA_MOVING_TIME + DELTA + COMA_SHRINKING_TIME);
    FiniteTimeAction* sequence = Sequence::create(func1, delay, func2, NULL);
    this->runAction(sequence);
}

void GameLayer::initializeQueue() {
    int temp = 0;
    std::vector<int> successNumberVector;
    for (std::pair<int, int> pair : successComaMap) {
        successComaBySuccessNumber.push(pair);
        successNumberVector.push_back(pair.second);
    }
    std::sort(successNumberVector.begin(), successNumberVector.end());
    if (successNumberVector.size() <= 0) {
        return;
    }
    temp = successNumberVector[0];
    successCounter++;
    for (int successNo : successNumberVector) {
        if (successNo != temp) {
            successCounter++;
            temp = successNo;
        }
    }
}

void GameLayer::getRemoveComas() {
    int previousSuccessNumber = successComaBySuccessNumber.top().second;
    int successNumber = successComaBySuccessNumber.top().second;
    removeComaTags.clear();
    while (!successComaBySuccessNumber.empty() && successNumber == previousSuccessNumber) {
        Element element = successComaBySuccessNumber.top();
        successNumber = element.second;
        if (previousSuccessNumber == successNumber) {
            removeComaTags.push_back(element.first);
            successComaBySuccessNumber.pop();
        }
    }
}

void GameLayer::removeNext() {
    if (!successComaBySuccessNumber.empty()) {
        auto delay = DelayTime::create(COMA_REMOVE_TIME + DELTA);
        auto func1 = CallFuncN::create(CC_CALLBACK_0(GameLayer::removeComaAndAnimation, this));
        auto func2 = CallFuncN::create(CC_CALLBACK_0(GameLayer::getRemoveComas, this));
        auto func3 = CallFuncN::create(CC_CALLBACK_0(GameLayer::removeNext, this));
        auto initNewComaFunc = CallFuncN::create(CC_CALLBACK_0(GameLayer::initNewComa, this));
        auto moveComaAnimationFunc = CallFuncN::create(CC_CALLBACK_0(GameLayer::moveComaAnimation, this));
        auto delay2 = DelayTime::create(0.05f);
        FiniteTimeAction* sequence = Sequence::create(delay2, func2, func1, delay, initNewComaFunc, moveComaAnimationFunc, func3, NULL);
        this->runAction(sequence);
    }
}

// コマをコマの成功番号毎に分類して順番消す。
void GameLayer::removeComaAndAnimation() {
    bool labelFlag = false;
    for (auto tag : removeComaTags) {
        if (!labelFlag) {
            puzzleBoardSprite->showLabel(removeComaTags);
            labelFlag = true;
        }
        auto func1 = CallFuncN::create(CC_CALLBACK_1(GameLayer::removeComa, this));
        auto action = ScaleTo::create(COMA_SHRINKING_TIME, 0);
        // アクションをつなげる
        FiniteTimeAction* sequence = Sequence::create(action, func1, NULL);
        
        // アクションをセットする。
        ComaSprite* sprite = nullptr;
        for (Node* node : puzzleBoardSprite->getChildren()) {
            if (tag == node->getTag()) {
                sprite = (ComaSprite*)node;
            }
        }
        sprite->runAction(sequence);
    }
}

void GameLayer::removeComa(Node* comaSprite) {
    puzzleBoardSprite->removeComa(comaSprite->getTag());
    comaSprite->removeFromParentAndCleanup(true);
}

void GameLayer::initNewComa() {
    if (successComaBySuccessNumber.empty()) {
        puzzleBoardSprite->initNewComa(removedComas);
    }
}

void GameLayer::moveComaAnimation() {
    if (successComaBySuccessNumber.empty()) {
        // コマの移動
        puzzleBoardSprite->moveComaAnimation(removedComas);
    }
}

