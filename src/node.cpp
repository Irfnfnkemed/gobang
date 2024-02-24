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

Evaluate_node::Evaluate_node(int score_, int player_four_sleep_, int ai_four_sleep_, int ai_three_active_) {
    score = score_;
    player_four_sleep = player_four_sleep_;
    ai_four_sleep = ai_four_sleep_;
    ai_three_active = ai_three_active_;
}

