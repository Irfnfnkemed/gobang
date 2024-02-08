#include <climits>
#include "evaluate.h"

inline bool Evaluator::match(PIECE_TYPE piece_type, int target_piece, int target_free) const {
    if (piece_type == PLAYER_PIECE) {
        return ((player_piece & target_piece) == target_piece) && ((free_piece & target_free) == target_free);
    } else {
        return ((ai_piece & target_piece) == target_piece) && ((free_piece & target_free) == target_free);
    }
}

int Evaluator::get_score(std::unordered_map<int, PIECE_TYPE> &board) {
    player_five = player_four_active = player_four_sleep =
    player_three_active = player_three_sleep = player_two_active = player_two_sleep =
    ai_five = ai_four_active = ai_four_sleep =
    ai_three_active = ai_three_sleep = ai_two_active = ai_two_sleep = 0;
    now_dir = HORIZONTAL;
    for (auto &iter: board) {
        now_pos = iter.first;
        for (int i = 0; i < 4; ++i) {
            set_piece(board);
            evaluate();
        }
    }
    if (player_five) {
        return INT_MIN;
    }
    if (ai_five) {
        return INT_MAX;
    }
//    if (player_four_active) {
//        return INT_MIN + 1;
//    }
//    if (ai_four_active) {
//        return INT_MAX - 1;
//    }
//    if (player_four_sleep >= 2) {
//        return INT_MIN + 2;
//    }
//    if (ai_four_sleep >= 2) {
//        return INT_MAX - 2;
//    }
//    if (player_four_sleep && player_three_active) {
//        return INT_MIN + 3;
//    }
//    if (ai_four_sleep && ai_three_active) {
//        return INT_MAX - 3;
//    }
    return (-player_four_active + ai_four_active) * SCORE_FOUR_ACTIVE + (-player_four_sleep + ai_four_sleep) * SCORE_FOUR_SLEEP +
           (-player_three_active + ai_three_active) * SCORE_THREE_ACTIVE + (-player_three_sleep + ai_three_sleep) * SCORE_THREE_SLEEP +
           (-player_two_active + ai_two_active) * SCORE_TWO_ACTIVE + (-player_two_sleep + ai_two_sleep) * SCORE_TWO_SLEEP;
}

void Evaluator::set_piece(std::unordered_map<int, PIECE_TYPE> &board) {
    int tmp = now_pos;
    player_piece = ai_piece = free_piece = 0;
    for (int i = 0; i <= 4; ++i) {
        if (tmp >= MIN_POS_ID && tmp <= MAX_POS_ID && (tmp & 0b1111)) {
            auto iter = board.find(tmp);
            if (iter != board.end()) {
                if (iter->second == PLAYER_PIECE) {
                    player_piece |= (1 << (i + 4));
                } else {
                    ai_piece |= (1 << (i + 4));
                }
            } else {
                free_piece |= (1 << (i + 4));
            }
            tmp += DIR_INTERVAL[now_dir];
        } else { break; }
    }
    tmp = now_pos - DIR_INTERVAL[now_dir];
    for (int i = 1; i <= 4; ++i) {
        if (tmp >= MIN_POS_ID && tmp <= MAX_POS_ID && (tmp & 0b1111)) {
            auto iter = board.find(tmp);
            if (iter != board.end()) {
                if (iter->second == PLAYER_PIECE) {
                    player_piece |= (1 << (4 - i));
                } else {
                    ai_piece |= (1 << (4 - i));
                }
            } else {
                free_piece |= (1 << (4 - i));
            }
            tmp -= DIR_INTERVAL[now_dir];
        } else { break; }
    }
    now_dir = (now_dir + 1) % 4;
}


void Evaluator::evaluate() {
    if (match(PLAYER_PIECE, 0b111110000)) { // player--五子
        ++player_five;
        return;
    } else if (match(PLAYER_PIECE, 0b011110000, 0b100001000)) { // player--活四
        ++player_four_active;
    } else if (match(PLAYER_PIECE, 0b111010000, 0b000100000) || match(PLAYER_PIECE, 0b101110000, 0b010000000) || match(PLAYER_PIECE, 0b110110000, 0b001000000) ||
               (match(PLAYER_PIECE, 0b011110000) && (match(AI_PIECE, 0b100000000, 0b000001000) || match(AI_PIECE, 0b000001000, 0b100000000)))) { // player--冲四
        ++player_four_sleep;
    } else if (match(PLAYER_PIECE, 0b011010000, 0b100101000) || match(PLAYER_PIECE, 0b010110000, 0b101001000) ||
               match(PLAYER_PIECE, 0b001110000, 0b010001100) || match(PLAYER_PIECE, 0b001110000, 0b110001000)) { // player--活三
        ++player_three_active;
    } else if ((match(PLAYER_PIECE, 0b001110000) && (match(AI_PIECE, 0b010000000, 0b000001100) || match(AI_PIECE, 0b000001000, 0b110000000))) ||
               (match(PLAYER_PIECE, 0b011010000) && (match(AI_PIECE, 0b100000000, 0b000101000) || match(AI_PIECE, 0b000001000, 0b100100000))) ||
               (match(PLAYER_PIECE, 0b010110000) && (match(AI_PIECE, 0b100000000, 0b001001000) || match(AI_PIECE, 0b000001000, 0b101000000))) ||
               match(PLAYER_PIECE, 0b110010000, 0b001100000) || match(PLAYER_PIECE, 0b100110000, 0b011000000) ||
               match(PLAYER_PIECE, 0b101010000, 0b010100000) || (match(PLAYER_PIECE, 0b001110000, 0b010001000) && match(AI_PIECE, 0b100000100))) { // player--眠三
        ++player_three_sleep;
    } else if (match(PLAYER_PIECE, 0b001010000, 0b110101000) || match(PLAYER_PIECE, 0b001010000, 0b010101100) ||
               match(PLAYER_PIECE, 0b010010000, 0b101101000) || match(PLAYER_PIECE, 0b000110000, 0b111001000) ||
               match(PLAYER_PIECE, 0b000110000, 0b011001100) || match(PLAYER_PIECE, 0b000110000, 0b001001110)) { // player--活二
        ++player_two_active;
    }

    if (match(AI_PIECE, 0b111110000)) { // AI--五子
        ++ai_five;
        return;
    } else if (match(AI_PIECE, 0b011110000, 0b100001000)) { // AI--活四
        ++ai_four_active;
    } else if (match(AI_PIECE, 0b111010000, 0b000100000) || match(AI_PIECE, 0b101110000, 0b010000000) || match(AI_PIECE, 0b110110000, 0b001000000) ||
               (match(AI_PIECE, 0b011110000) && (match(PLAYER_PIECE, 0b100000000, 0b000001000) || match(PLAYER_PIECE, 0b000001000, 0b100000000)))) { // AI--冲四
        ++ai_four_sleep;
    } else if (match(AI_PIECE, 0b011010000, 0b100101000) || match(AI_PIECE, 0b010110000, 0b101001000) ||
               match(AI_PIECE, 0b001110000, 0b010001100) || match(AI_PIECE, 0b001110000, 0b110001000)) { // AI--活三
        ++ai_three_active;
    } else if ((match(AI_PIECE, 0b001110000) && (match(PLAYER_PIECE, 0b010000000, 0b000001100) || match(PLAYER_PIECE, 0b000001000, 0b110000000))) ||
               (match(AI_PIECE, 0b011010000) && (match(PLAYER_PIECE, 0b100000000, 0b000101000) || match(PLAYER_PIECE, 0b000001000, 0b100100000))) ||
               (match(AI_PIECE, 0b010110000) && (match(PLAYER_PIECE, 0b100000000, 0b001001000) || match(PLAYER_PIECE, 0b000001000, 0b101000000))) ||
               match(AI_PIECE, 0b110010000, 0b001100000) || match(AI_PIECE, 0b100110000, 0b011000000) ||
               match(AI_PIECE, 0b101010000, 0b010100000) || (match(AI_PIECE, 0b001110000, 0b010001000) && match(PLAYER_PIECE, 0b100000100))) { // AI--眠三
        ++ai_three_sleep;
    } else if (match(AI_PIECE, 0b001010000, 0b110101000) || match(AI_PIECE, 0b001010000, 0b010101100) ||
               match(AI_PIECE, 0b010010000, 0b101101000) || match(AI_PIECE, 0b000110000, 0b111001000) ||
               match(AI_PIECE, 0b000110000, 0b011001100) || match(AI_PIECE, 0b000110000, 0b001001110)) { // AI--活二
        ++ai_two_active;
    }
}
