#ifndef GOBANG_ZOBRIST_HASH_H
#define GOBANG_ZOBRIST_HASH_H

#include <unordered_map>
#include <queue>
#include <random>
#include <climits>
#include "constant.h"

class Hash_map {
private:
    int now_depth = 0;
    long long zobrist[256][2];
    std::unordered_map<long long, std::queue<std::pair<int, int>>> node_score;
    std::unordered_map<long long, int> board_score;

public:
    Hash_map() {
        std::random_device rd;
        std::mt19937_64 generator(rd());
        std::uniform_int_distribution<long long> distribution(LLONG_MIN, LLONG_MAX);
        for (int i = 0b00010001; i <= 0b11111111; ++i) {
            zobrist[i][0] = distribution(generator);
            zobrist[i][1] = distribution(generator);
        }
    }

    long long get_key(long long pre_key, int pos, PIECE_TYPE type) {
        return pre_key ^ zobrist[pos][type];
    }

    int find_node(long long key, int min_depth) {
        min_depth += now_depth;
        auto iter = node_score.find(key);
        if (iter == node_score.end()) {
            return INT_MIN;
        } else {
            while (!iter->second.empty()) {
                if (iter->second.front().first < min_depth) {
                    iter->second.pop();
                } else {
                    return iter->second.front().second;
                }
            }
            return INT_MIN;
        }
    }

    int find_board(long long key) {
        return board_score[key];
    }

    void insert_node_score(long long key, int depth, int value) {
        node_score[key].push(std::make_pair(depth + now_depth, value));
    }

    void insert_board_score(long long key, int value) {
        board_score[key] = value;
    }

    void next_step() {
        ++now_depth;
    }

//    bool erase(long long key) {
//        auto iter = map.find(key);
//        if (iter != map.end()) {
//            return map.erase(iter->first);
//        } else {
//            return false;
//        }
//    }

};


#endif //GOBANG_ZOBRIST_HASH_H
