#include <unordered_set>
#include <iostream>
#include <algorithm>
#include "Game_tree.h"


Game_tree::Game_tree(NODE_CATE init_node, int search_depth_) : evaluator(&hash_map) {
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

Node *Game_tree::new_node(Node *father, int pos) {
    Node *tmp = new Node(father, pos);
    tmp->key = hash_map.get_key(father->key, pos, !(father->node_cate));
    tmp->node_value = hash_map.find_node(tmp->key, search_depth);
    tmp->board_value = evaluator.evaluate_board(tmp);
    tmp->pos_value = evaluator.evaluate_pos(pos);
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

int Game_tree::get_bound(NODE_CATE node_cate, int pos_value) {
    int bound;
    if (node_cate == MAX_NODE) {
        if (pos_value >= SCORE_FIVE) {
            bound = SCORE_FIVE;
        } else if (pos_value >= SCORE_THREE_SLEEP) {
            bound = SCORE_THREE_SLEEP;
        } else if (pos_value >= SCORE_TWO_ACTIVE) {
            bound = SCORE_TWO_ACTIVE;
        } else if (pos_value >= 0) {
            bound = 0;
        } else if (pos_value >= -SCORE_TWO_ACTIVE) {
            bound = -SCORE_TWO_ACTIVE;
        } else if (pos_value >= -SCORE_THREE_SLEEP) {
            bound = -SCORE_THREE_SLEEP;
        } else if (pos_value >= -SCORE_FIVE) {
            bound = -SCORE_FIVE;
        } else {
            bound = INT_MIN;
        }
    } else {
        if (pos_value <= -SCORE_FIVE) {
            bound = -SCORE_FIVE;
        } else if (pos_value <= -SCORE_THREE_SLEEP) {
            bound = -SCORE_THREE_SLEEP;
        } else if (pos_value <= -SCORE_TWO_ACTIVE) {
            bound = -SCORE_TWO_ACTIVE;
        } else if (pos_value <= 0) {
            bound = 0;
        } else if (pos_value <= SCORE_TWO_ACTIVE) {
            bound = SCORE_TWO_ACTIVE;
        } else if (pos_value <= SCORE_THREE_SLEEP) {
            bound = SCORE_THREE_SLEEP;
        } else if (pos_value <= SCORE_FIVE) {
            bound = SCORE_FIVE;
        } else {
            bound = INT_MAX;
        }
    }
    return bound;
}

bool Game_tree::kill(int score) {
    return score == SCORE_KILL_ONE || score == SCORE_KILL_TWO || score == SCORE_KILL_THREE;
}

bool cmp(Node *a, Node *b) {
    if (a->node_cate == MAX_NODE) {
        return a->pos_value < b->pos_value;
    } else {
        return a->pos_value > b->pos_value;
    }
}


void Game_tree::min_max_search(Node *cur_node, int depth) {
    if (depth == 0 && kill(-cur_node->board_value)) {
        std::unordered_set<int> son_board_id;
        int danger = INT_MAX;
        for (auto &it: cur_node->board) {
            int tmp_x = (it.first) >> 4, tmp_y = (it.first) & 0b1111;
            for (int i = -1; i <= 1; ++i) {
                if (tmp_x + i < 1) { continue; }
                if (tmp_x + i > BOARD_WIDTH) { break; }
                for (int j = -1; j <= 1; ++j) {
                    if (tmp_y + j < 1) { continue; }
                    if (tmp_y + j > BOARD_WIDTH) { break; }
                    int id = ((tmp_x + i) << 4) | (tmp_y + j);
                    if (!cur_node->board.count(id)) {
                        son_board_id.insert(id);
                    }
                }
            }
        }
        Node *son_node;
        for (int pos: son_board_id) {
            son_node = new_node(cur_node, pos);
            cur_node->son.push_back(son_node);
        }
        for (auto & iter : cur_node->son) {
            int tmp = evaluator.evaluate_kill_danger(iter);
            if (tmp < danger) {
                danger = tmp;
                next_root = iter;
            }
        }
        return;
    }
    if (depth > 0 && (kill(cur_node->board_value) || kill(-cur_node->board_value))) {
        cur_node->alpha = cur_node->beta = cur_node->node_value = cur_node->board_value;
        hash_map.insert_node_score(cur_node->key, search_depth, cur_node->node_value);
        return; // kill situation
    }
    if (kill(cur_node->node_value) || kill(-cur_node->node_value)) {
        cur_node->alpha = cur_node->beta = cur_node->node_value;
        hash_map.insert_node_score(cur_node->key, search_depth, cur_node->node_value);
        return; // kill situation
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
        Node *son_node;
        for (int pos: son_board_id) {
            son_node = new_node(cur_node, pos);
            cur_node->son.push_back(son_node);
        }
        std::sort(cur_node->son.begin(), cur_node->son.end(), cmp);
        int bound = get_bound(cur_node->node_cate, cur_node->son[0]->pos_value);
        int valid = 0;
        for (int i = 0; i < cur_node->son.size(); ++i) {
            if (cur_node->node_cate == MAX_NODE) {
                if (cur_node->son[i]->pos_value < bound) {
                    if (!valid) {
                        bound = get_bound(cur_node->node_cate, cur_node->son[i]->pos_value);
                    } else {
                        break;
                    }
                }
            } else {
                if (cur_node->son[i]->pos_value > bound) {
                    if (!valid) {
                        bound = get_bound(cur_node->node_cate, cur_node->son[i]->pos_value);
                    } else {
                        break;
                    }
                }
            }
            if (cur_node->son[i]->node_value == INT_MIN) { // cannot be multiplexed
                cur_node->son[i]->alpha = cur_node->alpha;
                cur_node->son[i]->beta = cur_node->beta;
                min_max_search(cur_node->son[i], depth + 1);
            } else {
                cur_node->son[i]->alpha = cur_node->son[i]->beta = cur_node->son[i]->node_value;
            }
            set_alpha_beta(cur_node, cur_node->son[i]);
            if (cur_node->node_cate == MAX_NODE) {
                if (!kill(-cur_node->son[i]->node_value) && (i == 0 || cur_node->son[i]->node_value != PRUNE_MIN)) {
                    ++valid;
                }
            } else {
                if (!kill(cur_node->son[i]->node_value) && (i == 0 || cur_node->son[i]->node_value != PRUNE_MAX)) {
                    ++valid;
                }
            }
            if (cur_node->alpha >= cur_node->beta && i < cur_node->son.size() - 1) {
                if (cur_node->node_cate == MAX_NODE) {
                    cur_node->node_value = cur_node->alpha = cur_node->beta = PRUNE_MAX;
                } else {
                    cur_node->node_value = cur_node->alpha = cur_node->beta = PRUNE_MIN;
                }
                return; // alpha-beta prune
            }
            if (cur_node->node_cate == MAX_NODE) {
                if (kill(cur_node->node_value)) {
                    cur_node->alpha = cur_node->beta = cur_node->node_value;
                    hash_map.insert_node_score(cur_node->key, search_depth, cur_node->node_value);
                    return; // kill situation
                }
            } else {
                if (kill(-cur_node->node_value)) {
                    cur_node->alpha = cur_node->beta = cur_node->node_value;
                    hash_map.insert_node_score(cur_node->key, search_depth, cur_node->node_value);
                    return; // kill situation
                }
            }
        }
        if (cur_node->node_cate == MAX_NODE) {
            cur_node->node_value = cur_node->beta = cur_node->alpha;
        } else {
            cur_node->node_value = cur_node->alpha = cur_node->beta;
        }
        if (cur_node->node_value != PRUNE_MIN && cur_node->node_value != PRUNE_MAX) {
            hash_map.insert_node_score(cur_node->key, search_depth, cur_node->node_value);
        }
        return;
    } else {
        cur_node->node_value = cur_node->alpha = cur_node->beta = cur_node->board_value;
        hash_map.insert_node_score(cur_node->key, search_depth, cur_node->node_value);
        return;
    }
}

void Game_tree::AI_next_status() {
    next_root_list.clear();
    int max_search_depth = search_depth;
    for (int i = 2; i <= max_search_depth; i += 2) {
        search_depth = i;
        root->alpha = INT_MIN;
        root->beta = INT_MAX;
        root->node_value = INT_MIN;
        root->son.clear();
        min_max_search(root);
        if (root->node_value == SCORE_KILL_ONE) {
            for (Node *tmp: root->son) {
                if (tmp->board_value == SCORE_KILL_ONE) {
                    next_root = tmp;
                }
                break;
            }
        }
        if (kill(-root->node_value)) {
            std::cout << "give up" << root->node_value << std::endl;
        }
        if (kill(root->node_value)) { // absolutely win
            break;
        }
        if (!kill(-root->node_value) && root->board.size() >= 4 && min_max_kill(root)) { // calculate kill
            std::cout << i << std::endl;
            break;
        }
        if (!kill(-root->node_value) || (i == 2)) {
            next_root_list.push_back(next_root);
        }
    }
    search_depth = max_search_depth;
    if (kill(-root->node_value)) { // avoid suicide
        next_root = *next_root_list.rbegin();
    }
    next_pos = next_root->pos;
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
    free();
}

void Game_tree::print_board() const {
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
    return root->board_value == SCORE_KILL_ONE;
}

bool Game_tree::lose() const {
    return root->board_value == -SCORE_KILL_ONE;
}

int Game_tree::get_five_pos() const {
    bool flag;
    for (auto &iter: root->board) {
        for (int i = 0; i < 4; ++i) {
            flag = true;
            for (int j = 1; j < 5; ++j) {
                auto tmp_iter = root->board.find(iter.first + DIR_INTERVAL[i] * j);
                if (tmp_iter == root->board.end() || tmp_iter->second != iter.second) {
                    flag = false;
                    break;
                }
            }
            if (flag) {
                return (iter.first << 10) | (i << 8);
            }
        }
    }
}

bool Game_tree::min_max_kill(Node *cur_node, int depth) {
    if (kill(-cur_node->board_value)) {
        return false; // kill situation
    }
    if (kill(cur_node->board_value)) {
        return true; // kill situation
    }
    if (depth < kill_search_depth - 1) {
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
        Node *son_node;
        for (int pos: son_board_id) {
            son_node = new_node(cur_node, pos);
            if (cur_node->node_cate == MAX_NODE) {
                if (cur_node->player_four_sleep) {
                    if (!son_node->player_four_sleep) {
                        cur_node->son.push_back(son_node);
                    }
                } else {
                    if (kill(son_node->board_value)) {
                        return true; // kill situation
                    } else if (son_node->ai_four_sleep || son_node->ai_three_active) {
                        cur_node->son.push_back(son_node);
                    }
                }
            } else {
                if (cur_node->ai_four_sleep) {
                    if (!son_node->ai_four_sleep) {
                        cur_node->son.push_back(son_node);
                    }
                } else {
                    if (son_node->player_four_sleep || !son_node->ai_three_active) {
                        cur_node->son.push_back(son_node);
                    }
                }
            }
        }
        if (cur_node->son.empty()) {
            return false;
        }
        std::sort(cur_node->son.begin(), cur_node->son.end(), cmp);
        if (cur_node->node_cate == MAX_NODE) {
            for (auto &iter: cur_node->son) {
                if (min_max_kill(iter, depth + 1)) {
                    return true;
                }
            }
            return false;
        } else {
            for (auto &iter: cur_node->son) {
                if (!min_max_kill(iter, depth + 1)) {
                    return false;
                }
            }
            return true;
        }
    } else {
        return false;
    }
}

bool Game_tree::min_max_kill(Node *cur_node) {
    if (cur_node->son.empty()) {
        if (!kill(-cur_node->node_value) && cur_node->node_value != PRUNE_MAX &&
            cur_node->node_value != PRUNE_MIN && cur_node->node_cate == MAX_NODE) {
            return min_max_kill(cur_node, 0);
        } else {
            return false;
        }
    } else {
        if (cur_node->node_cate == MAX_NODE) {
            for (auto &iter: cur_node->son) {
                if (min_max_kill(iter)) {
                    if (cur_node == root) {
                        next_root = iter;
                    }
                    return true;
                }
            }
            return false;
        } else {
            for (auto &iter: cur_node->son) {
                if (!min_max_kill(iter)) {
                    return false;
                }
            }
            return true;
        }
    }
}

