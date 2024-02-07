#ifndef GOBANG_GAME_TREE_H
#define GOBANG_GAME_TREE_H

#include <unordered_map>
#include <cstdint>
#include <climits>
#include "evaluate.h"
#include "constant.h"
#include "zobrist_hash.h"

struct Node {
    NODE_CATE node_cate;
    std::unordered_map<int, PIECE_TYPE> board;
    long long key = 0;
    int value = 0;
    int alpha = INT_MIN, beta = INT_MAX;
    std::unordered_set<Node *> son;
    Node *select = nullptr;
    int depth;

    explicit Node(NODE_CATE init_cate);

    Node(Node *father, int id);

    ~Node() = default;

};

class Game_tree {
private:
    Node *root;
    Evaluator evaluator;
    Hash_map<Node> hash_map;
    std::vector<Node *> collect;

    static inline void set_alpha_beta(Node *father, Node *son);

    inline Node *new_node(Node *father, int id);

    void reduce_depth(Node *cur_root,int depth);

public:
    explicit Game_tree(NODE_CATE init_node);

    ~Game_tree();

    void min_max_search(Node *cur_node);

    void AI_next_status(); // AI selection

    void player_next_status(int pos); // player selection

    void print_board();


};

#endif //GOBANG_GAME_TREE_H
