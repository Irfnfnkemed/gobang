#ifndef GOBANG_GAME_TREE_H
#define GOBANG_GAME_TREE_H

#include <unordered_map>
#include <unordered_set>
#include <cstdint>
#include <climits>
#include "evaluate.h"
#include "constant.h"
#include "zobrist_hash.h"

struct Node {
    NODE_CATE node_cate;
    std::unordered_map<int, PIECE_TYPE> board;
    long long key = 0;
    int node_value = INT_MIN, board_value = INT_MIN;
    int alpha = INT_MIN, beta = INT_MAX;
    std::vector<Node *> son;

    explicit Node(NODE_CATE init_cate);

    Node(Node *father, int id);

    ~Node() = default;

};

class Game_tree {
public:
    Node *root, *next_root = nullptr;
    int next_pos;
    Evaluator evaluator;
    Hash_map hash_map;
    std::vector<Node *> collect;
    int search_depth;

    inline void set_alpha_beta(Node *father, Node *son);

    inline Node *new_node(Node *father, int id);

    void free();

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


};

#endif //GOBANG_GAME_TREE_H
