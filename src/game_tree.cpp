#include <unordered_set>
#include <iostream>
#include "Game_tree.h"

Node::Node(NODE_CATE init_cate) {
    node_cate = init_cate;
}

Node::Node(Node *father, int id) {
    node_cate = !(father->node_cate);
    board = father->board;
    board[id] = node_cate; // add new piece
    alpha = father->alpha;
    beta = father->beta;
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
    if (father->node_cate == MAX_NODE) {
        if (father->alpha < son->beta) {
            father->alpha = son->beta;
        }
    } else {
        if (father->beta > son->alpha) {
            father->beta = son->alpha;
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
        tmp->ignore = false;
        tmp->alpha = father->alpha;
        tmp->beta = father->beta;
    }
    return tmp;
}


void Game_tree::min_max_search(Node *cur_node, int depth) {
    if (cur_node->value == INT_MAX || cur_node->value == INT_MIN) {
        cur_node->alpha = cur_node->beta = cur_node->value;
        return;
    }
    if (depth < SEARCH_DEPTH) {
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
            min_max_search(now_node, depth + 1);
            set_alpha_beta(cur_node, now_node);
            if (cur_node->alpha >= cur_node->beta) {
                std::cout << "!";
                cur_node->ignore = true;
                return; // alpha-beta prune
            }
        }
    } else {
        if (cur_node->node_cate == MAX_NODE) {
            cur_node->alpha = cur_node->value;
        } else {
            cur_node->beta = cur_node->value;
        }
    }
}

void Game_tree::AI_next_status() {
    std::cout.flush();
    root->alpha = INT_MIN;
    root->beta = INT_MAX;
    min_max_search(root);
    Node *select = nullptr;
    int max = INT_MIN;
    for (Node *tmp: root->son) {
        if (!(tmp->ignore) && tmp->beta >= max) {
            select = tmp;
            max = tmp->beta;
        }
    }
    root = select;
}

void Game_tree::player_next_status(int pos) {
    long long key = hash_map.get_key(root->key, pos, PLAYER_PIECE);
    Node *node = hash_map.find(key);
    if (node != nullptr) {
        root = node;
        root->son.clear();
    } else {
        root = new_node(root, pos);
    }
    root->alpha = INT_MIN;
    root->beta = INT_MAX;
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
    std::cout << std::dec << evaluator.get_score(root->board) << std::endl;
}