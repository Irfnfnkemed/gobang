#ifndef GOBANG_EVALUATE_H
#define GOBANG_EVALUATE_H

#include <unordered_map>
#include <cstdint>
#include "constant.h"
#include "node.h"
#include "zobrist_hash.h"

class Evaluator {
private:
    Hash_map *hash_map;
    Node *node{};
    int player_piece = 0;
    int ai_piece = 0;
    int free_piece = 0;
    int now_pos = 0;
    int player_five{}, player_four_active{}, player_four_sleep{},
            player_three_active{}, player_three_sleep{}, player_two_active{}, player_two_sleep{};
    int ai_five{}, ai_four_active{}, ai_four_sleep{},
            ai_three_active{}, ai_three_sleep{}, ai_two_active{}, ai_two_sleep{};
    int score{};

    inline bool match(PIECE_TYPE piece_type, int target_piece) const;

    inline bool match(PIECE_TYPE piece_type, int target_piece, int target_free) const;

    inline bool match(PIECE_TYPE piece_type, int target_piece, int target_free, int target_occupy) const;

    void set_piece(int now_pos_, LINE_DIR now_dir);

    void evaluate_line();

    int get_score();

public:
    explicit Evaluator(Hash_map *hash_map);

    ~Evaluator() = default;

    int evaluate_board(Node *node);

    int evaluate_kill_danger(Node *node);

    int evaluate_pos(int pos);

};

#endif //GOBANG_EVALUATE_H
