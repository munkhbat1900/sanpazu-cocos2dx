#ifndef __HPBARSPRITE__
#define __HPBARSPRITE__

#include "cocos2d.h"

class HpBarSprite : public cocos2d::Sprite {
private:
    // アニメーション実行中かどうか
    bool _isAnimating;
    float _timeBarWidth;
    float _timeBarHeight;
    float _timeBarStartPoint = 50;
    float _timeBarShrinkSpeed;
    
    // 過ぎた秒数を渡して、バーの長さを取得する。
    float getTimeBarLength(float seconds);
    
public:
    HpBarSprite();
    ~HpBarSprite();
    
    static HpBarSprite* createHpBar(std::string imageFileName);
    bool initWithFileName(std::string imageFileName);
    
    void animateTimeBar(float delta);
    void stopTimeAnimation();
    void clearDrawNode();
    void initTimeBarSize();
};

#endif

