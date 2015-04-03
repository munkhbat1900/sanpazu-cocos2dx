#ifndef __BOARD_SPRITE__
#define __BOARD_SPRITE__

#include "cocos2d.h"
#include "ComaSprite.h"
#include "enums.h"

class BoardSprite : public cocos2d::Sprite {
private:
    // ボードサイズ
    const int BOARD_X = 7;
    const int BOARD_Y = 5;
    const int COMA_BASE_TAG = 10000;
    const int LABEL_BASE_TAG = 10;
    
    cocos2d::Map<int, ComaSprite*> allComaSpriteTagMap;
    
    void comaExchangeAnimation(ComaSprite* passedComa, ComaSprite* selectedComa);
    
    cocos2d::Vector<cocos2d::Label*> successNumberLabelVector;
    
    int _attackCounter {0};
    
public:
    
    BoardSprite();
    ~BoardSprite();
    
    static BoardSprite* createBoardSprite(std::string imageFileName);
    bool initWithFileName(std::string imageFileName);
    // コマの初期配置
    void initComa();
    // 新規コマを生成
    ComaSprite* createNewComa(ComaSprite::PositionIndex positionIndex);
    // コマの位置から座標を取得する
    cocos2d::Vec2 getComaPosition(ComaSprite::PositionIndex positionIndex);
    // コマの位置からタグを生成
    int computeComaTag(ComaSprite::PositionIndex positionIndex);
    // 選択されているコマを取得。つまり指で触れているコマ。touchPointはパズルボードを基準にした座標
    // 取得の際にタッチしたポイントにコマがするように移動。
    ComaSprite* getSelectedComaAndMoveLittle(cocos2d::Vec2 touchPoint);
    // タッチポイントから、コマのインデックスを取得
    ComaSprite::PositionIndex getPositionIndex(cocos2d::Vec2 touchPoint);
    
    ComaSprite* getPassedComa(cocos2d::Vec2 passedPoint);
    
    void changePosition(ComaSprite* passedComa, ComaSprite* selectedComa);
    
    //コマの座標をセットする。
    void setComaPosition();
    
    // コマの種類を指定してコマを作成。
    ComaSprite* createNewComaByComaType(ComaType comaTyp, ComaSprite::PositionIndex positionIndex);
    // 初期配置の時に、パズルが成功しないように配置するためのコマの種類を選択。
    ComaType getComaType(ComaSprite::PositionIndex positionIndex);
    // コマの新しい位置を検索。引数は消えるコマのタグのリスト。
    void searchNewPosition(const std::vector<int> &removedComas);
    // コマの新しい位置をセット。
    void setNewPosition(ComaSprite::PositionIndex positionIndex);
    // コマの移動アニメーション
    void moveComaAnimation(const std::vector<int> &removedComas);
    // コマが消去された後に、新規コマを生成。
    void initNewComa(const std::vector<int> &removedComas);
    // タグからpositionIndexを取得
    ComaSprite::PositionIndex getPositionIndexFromTag(int tag);
    
    void removeComa(int tag);
    
    // 「○連撃」を表示
    void showLabel(const std::vector<int> &removeComaTags);
    // 「○連撃」を消去。
    void deleteAllLabel();
};

#endif