#include <unordered_set>
#include <iostream>
#include <algorithm>
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


Game_tree::Game_tree(NODE_CATE init_node, int search_depth_) {
    root = new Node(init_node);
    collect.push_back(root);
    search_depth = search_depth_;
}

Game_tree::~Game_tree() {
    for (Node *tmp: collect) {
        delete tmp;
    }
}

inline void Game_tree::set_alpha_beta(Node *father, Node *son) {
    if (father->node_cate == MAX_NODE) {
        if (father->alpha < son->node_value) {
            father->alpha = son->node_value;
            if (father == root) {
                next_root = son;
            }
        }
    } else {
        if (father->beta > son->node_value) {
            father->beta = son->node_value;
            if (father == root) {
                next_root = son;
            }
        }
    }
}

Node *Game_tree::new_node(Node *father, int id) {
    Node *tmp = new Node(father, id);
    tmp->key = hash_map.get_key(father->key, id, !(father->node_cate));
    tmp->node_value = hash_map.find(tmp->key, search_depth);
    tmp->board_value = evaluator.get_score(tmp->board, tmp->node_cate);
    collect.push_back(tmp);
    return tmp;
}

void Game_tree::free() {
    for (Node *tmp: collect) {
        if (tmp != root) {
            delete tmp;
        }
    }
    collect.clear();
    collect.push_back(root);
}

bool cmp(Node *a, Node *b) {
    if (a->node_cate == MAX_NODE) {
        return a->board_value < b->board_value;
    } else {
        return a->board_value > b->board_value;
    }
}


void Game_tree::min_max_search(Node *cur_node, int depth) {
    if (cur_node->node_value == INT_MAX - 2 || cur_node->node_value == INT_MIN + 2 ||
        cur_node->node_value == INT_MAX - 3 || cur_node->node_value == INT_MIN + 3 ||
        cur_node->node_value == INT_MAX - 4 || cur_node->node_value == INT_MIN + 4) {
        cur_node->alpha = cur_node->beta = cur_node->node_value;
        hash_map.insert(cur_node->key, search_depth, cur_node->node_value);
        return;
    }
    if (depth < search_depth) {
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
            son_board_id.insert((8 << 4) | 8); // init status
        }
        Node *now_node;
        for (int pos: son_board_id) {
            now_node = new_node(cur_node, pos);
            if (evaluator.effective()) {
                cur_node->son.push_back(now_node);
            }
        }
        if (cur_node->son.empty()) {
            for (int pos: son_board_id) {
                now_node = new_node(cur_node, pos);
                cur_node->son.push_back(now_node);
            }
        }
        std::sort(cur_node->son.begin(), cur_node->son.end(), cmp);
        int size = son_board_id.size();
        for (Node *now_node: cur_node->son) {
            --size;
            if (now_node->node_value == INT_MIN) {
                now_node->alpha = cur_node->alpha;
                now_node->beta = cur_node->beta;
                min_max_search(now_node, depth + 1);
            } else {
                now_node->alpha = now_node->beta = now_node->node_value;
            }
            set_alpha_beta(cur_node, now_node);
            if (cur_node->alpha >= cur_node->beta && size > 0) {
                if (cur_node->node_cate == MAX_NODE) {
                    cur_node->node_value = cur_node->alpha = cur_node->beta = INT_MAX - 1;
                } else {
                    cur_node->node_value = cur_node->alpha = cur_node->beta = INT_MIN + 1;
                }
                return; // alpha-beta prune
            }
        }
        if (cur_node->node_cate == MAX_NODE) {
            cur_node->node_value = cur_node->beta = cur_node->alpha;
        } else {
            cur_node->node_value = cur_node->alpha = cur_node->beta;
        }
        if (cur_node->node_value != INT_MAX - 1 && cur_node->node_value != INT_MIN + 1) {
            hash_map.insert(cur_node->key, search_depth, cur_node->node_value);
        }
    } else {
        cur_node->node_value = cur_node->alpha = cur_node->beta = cur_node->board_value;
        hash_map.insert(cur_node->key, search_depth, cur_node->node_value);
    }
}

void Game_tree::AI_next_status() {
    int max_search_depth = search_depth;
    for (int i = 2; i <= max_search_depth; i += 2) {
        search_depth = i;
        root->alpha = INT_MIN;
        root->beta = INT_MAX;
        root->node_value = INT_MIN;
        min_max_search(root);
        if (root->node_value == INT_MAX - 2) {
            for (Node *tmp: root->son) {
                if (tmp->board_value == INT_MAX - 2) {
                    next_root = tmp;
                }
                break;
            }
        }
        if (root->node_value == INT_MAX - 2 || root->node_value == INT_MIN + 2 ||
            root->node_value == INT_MAX - 3 || root->node_value == INT_MIN + 3 ||
            root->node_value == INT_MAX - 4 || root->node_value == INT_MIN + 4) {
            break;
        }
    }
    search_depth = max_search_depth;
    for (auto &iter: next_root->board) {
        if (!root->board.count(iter.first)) {
            next_pos = iter.first;
            break;
        }
    }
    hash_map.next_step();
    root = next_root;
    next_root = nullptr;
    free();
}

void Game_tree::player_next_status(int pos) {
    root = new_node(root, pos);
    root->alpha = INT_MIN;
    root->beta = INT_MAX;
    root->node_value = INT_MIN;
    hash_map.next_step();
    free();
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

int Game_tree::get_pos() const {
    return next_pos;
}

bool Game_tree::win() const {
    return root->board_value == INT_MAX - 2;
}

bool Game_tree::lose() const {
    return root->board_value == INT_MIN + 2;
}
