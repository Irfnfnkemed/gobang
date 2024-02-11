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
    std::vector<Node *> son;

    explicit Node(NODE_CATE init_cate);

    Node(Node *father, int pos);

    ~Node() = default;

};

#endif //GOBANG_NODE_H
