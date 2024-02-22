#ifndef GOBANG_CONSTANT_H
#define GOBANG_CONSTANT_H

#include <cstdint>

using NODE_CATE = bool;
extern const NODE_CATE MAX_NODE;
extern const NODE_CATE MIN_NODE;

using PIECE_TYPE = bool;
extern const PIECE_TYPE AI_PIECE;
extern const PIECE_TYPE PLAYER_PIECE;

using LINE_DIR = int;
extern const LINE_DIR HORIZONTAL;
extern const LINE_DIR VERTICAL;
extern const LINE_DIR DIAGONAL;
extern const LINE_DIR ANTI_DIAGONAL;
extern const int DIR_INTERVAL[4];


extern const int BOARD_WIDTH;
extern const int MIN_POS_ID;
extern const int MAX_POS_ID;

extern const int SEARCH_DEPTH;

extern const int PLACE_RANGE;

extern const int SCORE_KILL_ONE;
extern const int SCORE_KILL_TWO;
extern const int SCORE_KILL_THREE;
extern const int SCORE_FIVE;
extern const int SCORE_FOUR_ACTIVE;
extern const int SCORE_FOUR_SLEEP;
extern const int SCORE_THREE_ACTIVE;
extern const int SCORE_THREE_SLEEP;
extern const int SCORE_TWO_ACTIVE;
extern const int SCORE_TWO_SLEEP;

extern const int PRUNE_MIN;
extern const int PRUNE_MAX;



#endif //GOBANG_CONSTANT_H
