//
//  enums.h
//  puzzle
//
//  Created by Avirmed Munkhbat on 9/5/14.
//
//

#ifndef puzzle_enums_h
#define puzzle_enums_h

// タグ
enum class kTag {
    enemyArea,
    HpBarArea,
    Board,
};

// zオーダー
enum class kZOrder {
    background,
    coma,
    selectedComa,     // 選択されているコマ
};

// コマの種類
enum class ComaType {
    Red,
    Blue,
    Gray,
    Purple,
    Pink,
    Green,
    Size,
};

enum class Direction {
    x,
    y
};

// ボードサイズ
const int BOARD_X = 7;
const int BOARD_Y = 5;

const int SUCCESS_COUNT = 3;

// コマが消えるアニメーションの時間
const float COMA_REMOVE_TIME = 0.3f;
//コマ移動時間
const float COMA_MOVING_TIME = 0.3f;
const float DELTA = 0.2f;

//ボードのサイズについてありえない大きな数値
const int BOARD_INF = 10000;

const float PUZZLE_TIME = 4.0f;

const float COMA_EXCHANGE_TIME = 0.08f;

const float COMA_SHRINKING_TIME = 0.1f;

#endif
