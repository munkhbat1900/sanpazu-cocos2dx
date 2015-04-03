#ifndef __GAMELAYER_H__
#define __GAMELAYER_H__

#include "cocos2d.h"
#include "ComaSprite.h"
#include "BoardSprite.h"
#include "enums.h"
#include "HpBarSprite.h"

class GameLayer : public cocos2d::Layer {
private:
    const std::string enemyImageFileName = "enemy.png";
    const std::string puzzleBoardFileName = "board.png";
    const std::string hpBarImageFileName = "Hpbar.png";
    
    // バズル領域のスプライト
    BoardSprite* puzzleBoardSprite;
    //選択されたコマ。
    ComaSprite* selectedComa;
    // HPバーsprite
    HpBarSprite* _hpBarSprite;
    
    // 画面構築
    void constructScreen();
    
    void initComa();
    
    // タッチイベント
    std::function<bool(cocos2d::Touch*, cocos2d::Event*)> onTouchBegan();
    std::function<void(cocos2d::Touch*, cocos2d::Event*)> onTouchEnded();
    std::function<void(cocos2d::Touch*, cocos2d::Event*)> onTouchMoved();
    
    bool touchable = true;
    
    void findSuccessAndRemove();
    // コマの削除とアニメーション
    void removeComaAndAnimation();
    // コマを削除
    void removeComa(Node * comaSprite);
    // コマの移動アニメーション
    void moveComaAnimation();
    
    using Element = std::pair<int, int>;
    struct second_order {
        bool operator()(const Element& x, const Element& y) {
            return x.second > y.second;
        }
    };
    //削除すべきコマを順番にセットする。
    void getRemoveComas();
    
    // パズルが成功しているかどうかを軸ごとにチェックする。
    void checkPuzzleByAxis(Direction direction, int x, int y);
    // 優先順位付きキューを初期化
    void initializeQueue();
    
    void removeNext();
    // コマが消去された後に新しいコマを初期化する。
    void initNewComa();
    //パズル終了時の処理
    void puzzleEnd();
    
    // タグとコマの種類のマップ
    std::map<int, ComaType> tagComaTypeMap;
    // タグとコマスプライトのマップ
    //cocos2d::Map<int, ComaSprite*> tagComaSpriteMap;
    // コマのタグとパズルが成功しているかどうかでチェックされたかを表すペアのマップ。ペアは横方向と縦方向でチェックしたかの値を持つ。
    std::map<int, std::pair<bool, bool>> tagComaCheckMap;
    // 成功したコマのタグをマップとして保存。
    // キー：コマのタグ、値：パズル成功番号。
    std::map<int, int> successComaMap;
    // mapはデフォルトでは、キーが上昇順にソートされている。そのため、successComaMapはコマが上昇順にソートされている。
    // successComaMapを値が上昇順にソートするために priorityキューを利用。
    std::priority_queue<Element, std::vector<Element>, second_order> successComaBySuccessNumber;
    // 削除するコマのタグのリスト。
    std::vector<int> removeComaTags;
    // 削除されたコマのリスト
    std::vector<int> removedComas;
    // パズル成功数
    int successCount {0};
    int successCounter {0};
    
    // パズル操作時間
    float _puzzleTime = 0.0f;
    // コマの入れ替え行っているか？
    bool _isComaMoving = false;
    
    void update(float delta);
    
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(GameLayer);
};

#endif