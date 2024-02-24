#ifndef GOBANG_NODE_H
#define GOBANG_NODE_H

#include <unordered_map>
#include <climits>
#include <vector>
#include "constant.h"

struct Node {
    NODE_CATE node_cate;
    std::unordered_map<int, PIECE_TYPE> board;
    long long key = 0;
    int pos;
    int node_value = INT_MIN, pos_value = INT_MIN, board_value = INT_MIN;
    int alpha = INT_MIN, beta = INT_MAX;
    bool player_four_sleep = false, ai_four_sleep = false, ai_three_active = false;
    std::vector<Node *> son;

    explicit Node(NODE_CATE init_cate);

    Node(Node *father, int pos);

    ~Node() = default;
};

struct Evaluate_node {
    int score = INT_MIN;
    bool player_four_sleep = false;
    bool ai_four_sleep = false;
    bool ai_three_active = false;

    Evaluate_node() = default;

    explicit Evaluate_node(int score_, int player_four_sleep_, int ai_four_sleep_, int ai_three_active_);
};

#endif //GOBANG_NODE_H
