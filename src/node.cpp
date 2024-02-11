#include "node.h"


Node::Node(NODE_CATE init_cate) {
    node_cate = init_cate;
}

Node::Node(Node *father, int pos_) {
    node_cate = !(father->node_cate);
    board = father->board;
    pos = pos_;
    board[pos] = node_cate; // add new piece
    alpha = father->alpha;
    beta = father->beta;
}