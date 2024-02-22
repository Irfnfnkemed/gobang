#ifndef GOBANG_GAME_TREE_H
#define GOBANG_GAME_TREE_H

#include <unordered_map>
#include <unordered_set>
#include <cstdint>
#include <climits>
#include "evaluate.h"
#include "node.h"
#include "constant.h"
#include "zobrist_hash.h"


class Game_tree {
public:
    Node *root, *next_root = nullptr;
    int next_pos;
    Hash_map hash_map;
    Evaluator evaluator;
    std::vector<Node *> collect;
    int search_depth, kill_search_depth = 100;

    inline void set_alpha_beta(Node *father, Node *son);

    inline Node *new_node(Node *father, int pos);

    void free();

    int get_bound(NODE_CATE node_cate, int pos_value);

    bool kill(int score);

public:
    explicit Game_tree(NODE_CATE init_node, int search_depth = SEARCH_DEPTH);

    ~Game_tree();

    void min_max_search(Node *cur_node, int depth = 0);

    void AI_next_status(); // AI selection

    void player_next_status(int pos); // player selection

    void print_board();

    int get_pos() const;

    bool win() const;

    bool lose() const;

    int get_five_pos() const;
};

#endif //GOBANG_GAME_TREE_H
