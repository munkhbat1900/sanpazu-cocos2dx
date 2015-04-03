#ifndef __COMA_SPRITE__
#define __COMA_SPRITE__

#include "cocos2d.h"
#include "enums.h"



enum class kComaZOrder {
    coma,
    selectedComa,     // 選択されているコマ
};

class ComaSprite : public cocos2d::Sprite {
private:
    int nextPosX;
    int nextPosY;
    
public:
    struct PositionIndex {
        PositionIndex() {}
        PositionIndex(int x1, int y1) {
            x = x1;
            y = y1;
        }
        int x;
        int y;
    };
    
    ComaSprite();
    virtual ~ComaSprite();
    
    static ComaSprite* createWithComaType(ComaType comaType);
    const std::string getComaFileName(ComaType comaType);
    bool initWithComaType(ComaType comaType);
    
    static ComaSprite* createAttakingComa(const std::string fileName);
    bool initWithFileName(const std::string fileName);
    
    // コマ位置。
    CC_SYNTHESIZE(PositionIndex, positionIndex, PositionIndex);
    // コマのタグ
    CC_SYNTHESIZE(int, tag, Tag);
    // コマの種類ゲッターセッター
    CC_SYNTHESIZE(ComaType, comaType, ComaType);
    // コマのzOrder
    CC_SYNTHESIZE(kComaZOrder, zOrder, ComaZOrder);
    
    void setNextPos(int nextPosX, int nextPosY);
    
    void initNextPos();
    
    int getNextPosY();
    int getNextPosX();
};

#endif