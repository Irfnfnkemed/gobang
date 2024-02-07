#include <unordered_set>
#include <iostream>
#include "Game_tree.h"

Node::Node(NODE_CATE init_cate) {
    node_cate = init_cate;
    depth = 0;
}

Node::Node(Node *father, int id) {
    node_cate = !(father->node_cate);
    board = father->board;
    board[id] = node_cate; // add new piece
    alpha = father->alpha;
    beta = father->beta;
    depth = father->depth + 1;
}


Game_tree::Game_tree(NODE_CATE init_node) {
    root = new Node(init_node);
    collect.push_back(root);
}

Game_tree::~Game_tree() {
    for (Node *tmp: collect) {
        delete tmp;
    }
}

inline void Game_tree::set_alpha_beta(Node *father, Node *son) {
    if (father->node_cate == MIN_NODE) {
        if (father->alpha < son->alpha) {
            father->alpha = son->alpha;
            father->select = son;
        }
    } else {
        if (father->beta > son->beta) {
            father->beta = son->beta;
            father->select = son;
        }
    }
}

Node *Game_tree::new_node(Node *father, int id) {
    long long key = hash_map.get_key(father->key, id, !(father->node_cate));
    Node *tmp = hash_map.find(key);
    if (tmp == nullptr) {
        tmp = new Node(father, id);
        tmp->key = key;
        tmp->value = evaluator.get_score(tmp->board);
        hash_map.insert(key, tmp);
        collect.push_back(tmp);
    } else {
        tmp->depth = father->depth + 1;
    }
    return tmp;
}

void Game_tree::reduce_depth(Node *cur_root, int depth) {
    cur_root->depth = depth;
    for (Node *tmp: cur_root->son) {
        if (depth + 1 != tmp->depth) {
            reduce_depth(tmp, depth + 1);
        }
    }
}


void Game_tree::min_max_search(Node *cur_node) {
    if (cur_node->value == INT_MAX || cur_node->value == INT_MIN) {
        cur_node->alpha = cur_node->beta = cur_node->value;
        return;
    }
    if (cur_node->depth < SEARCH_DEPTH) {
        if (cur_node->son.empty()) { // need to build game tree node
            std::unordered_set<int> son_board_id;
            for (auto &it: cur_node->board) {
                int tmp_x = (it.first) >> 4, tmp_y = (it.first) & 0b1111;
                for (int i = -PLACE_RANGE; i <= PLACE_RANGE; ++i) {
                    if (tmp_x + i < 1) { continue; }
                    if (tmp_x + i > BOARD_WIDTH) { break; }
                    for (int j = -PLACE_RANGE; j <= PLACE_RANGE; ++j) {
                        if (tmp_y + j < 1) { continue; }
                        if (tmp_y + j > BOARD_WIDTH) { break; }
                        int id = ((tmp_x + i) << 4) | (tmp_y + j);
                        if (!cur_node->board.count(id)) {
                            son_board_id.insert(id);
                        }
                    }
                }
            }
            if (son_board_id.empty()) {
                /////////////////////
            }
            Node *now_node;
            for (int pos: son_board_id) {
                now_node = new_node(cur_node, pos);
                cur_node->son.insert(now_node);
                min_max_search(now_node);
                set_alpha_beta(cur_node, now_node);
                if (cur_node->alpha >= cur_node->beta) {
                    if (cur_node->node_cate == MIN_NODE) {
                        cur_node->beta = cur_node->alpha;
                    } else {
                        cur_node->alpha = cur_node->beta;
                    }
                    return; // alpha-beta prune
                }
            }
            if (cur_node->node_cate == MIN_NODE) {
                cur_node->beta = cur_node->alpha;
            } else {
                cur_node->alpha = cur_node->beta;
            }
            return;
        } else { // already built in last round
            for (Node *tmp: cur_node->son) {
                tmp->alpha = cur_node->alpha;
                tmp->beta = cur_node->beta;
                tmp->depth = cur_node->depth + 1;
                min_max_search(tmp);
                set_alpha_beta(cur_node, tmp);
                if (cur_node->alpha >= cur_node->beta) {
                    if (cur_node->node_cate == MIN_NODE) {
                        cur_node->beta = cur_node->alpha;
                    } else {
                        cur_node->alpha = cur_node->beta;
                    }
                    return; // alpha-beta prune
                }
            }
        }
    } else {
        cur_node->alpha = cur_node->beta = cur_node->value;
    }
}

void Game_tree::AI_next_status() {
    root->alpha = INT_MIN;
    root->beta = INT_MAX;
    min_max_search(root);
//    if (root->select == nullptr) {
//        root->select = root->son[0];
//    }
    reduce_depth(root->select, 0);
    root = root->select;
}

void Game_tree::player_next_status(int pos) {
    long long key = hash_map.get_key(root->key, pos, PLAYER_PIECE);
    Node *node = hash_map.find(key);
    if (node != nullptr) {
        reduce_depth(node, 0);
        root = node;
    } else {
        root = new_node(root, pos);
        root->depth = 0;
        root->alpha = INT_MIN;
        root->beta = INT_MAX;
    }
}

void Game_tree::print_board() {
    int display[16][16] = {0};
    for (auto &iter: root->board) {
        display[(iter.first) >> 4][(iter.first) & 0b1111] = int(iter.second) + 1;
    }
    std::cout << "\n\n\n\n\n\n\n\n\n\n    1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n";
    for (int i = 1; i <= 15; ++i) {
        std::cout << ' ' << std::hex << i << ' ';
        for (int j = 1; j <= 15; ++j) {
            switch (display[i][j]) {
                case 0:
                    std::cout << " . ";
                    break;
                case 1:
                    std::cout << " P ";
                    break;
                default:
                    std::cout << " A ";
                    break;
            }
        }
        std::cout << '\n';
    }
    std::cout.flush();
}