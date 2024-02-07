#ifndef GOBANG_EVALUATE_H
#define GOBANG_EVALUATE_H

#include <unordered_map>
#include <cstdint>
#include "constant.h"


class Evaluator {
private:
    int player_piece = 0;
    int ai_piece = 0;
    int free_piece = 0;
    int now_pos = 0;
    LINE_DIR now_dir = HORIZONTAL;
    int player_five, player_four_active, player_four_sleep,
            player_three_active, player_three_sleep, player_two_active, player_two_sleep;
    int ai_five, ai_four_active, ai_four_sleep,
            ai_three_active, ai_three_sleep, ai_two_active, ai_two_sleep;

    inline bool match(PIECE_TYPE piece_type, int target_piece, int target_free = 0) const;

public:
    Evaluator() = default;

    ~Evaluator() = default;

    int get_score(std::unordered_map<int, PIECE_TYPE> &board);

    void set_piece(std::unordered_map<int, PIECE_TYPE> &board);

    void evaluate();

};

#endif //GOBANG_EVALUATE_H
