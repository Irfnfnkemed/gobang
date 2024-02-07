#include "constant.h"

const NODE_CATE MAX_NODE = true;
const NODE_CATE MIN_NODE = false;

const PIECE_TYPE AI_PIECE = true;
const PIECE_TYPE PLAYER_PIECE = false;

const LINE_DIR HORIZONTAL = 0;
const LINE_DIR VERTICAL = 1;
const LINE_DIR DIAGONAL = 2;
const LINE_DIR ANTI_DIAGONAL = 3;
const int DIR_INTERVAL[4] = {1, 16, 17, 15};


const int BOARD_WIDTH = 15;
const int MIN_POS_ID = 0b00010001;
const int MAX_POS_ID = 0b11111111;

const int SEARCH_DEPTH = 2;
const int PLACE_RANGE = 3;

const int SCORE_FIVE = 10000000;
const int SCORE_FOUR_ACTIVE = 500000;
const int SCORE_FOUR_SLEEP = 2500;
const int SCORE_THREE_ACTIVE = 5000;
const int SCORE_THREE_SLEEP = 500;
const int SCORE_TWO_ACTIVE = 50;
const int SCORE_TWO_SLEEP = 10;
