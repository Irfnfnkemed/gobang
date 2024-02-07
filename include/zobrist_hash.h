#ifndef GOBANG_ZOBRIST_HASH_H
#define GOBANG_ZOBRIST_HASH_H

#include <unordered_map>
#include <random>
#include <climits>
#include "constant.h"

template<typename T>
class Hash_map {
private:
    long long zobrist[256][2];
    std::unordered_map<long long, T *> map;

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

    T *find(long long key) {
        auto iter = map.find(key);
        if (iter != map.end()) {
            return iter->second;
        } else {
            return nullptr;
        }
    }

    void insert(long long key, T *value) {
        map[key] = value;
    }

    bool erase(long long key) {
        auto iter = map.find(key);
        if (iter != map.end()) {
            return map.erase(iter->first);
        } else {
            return false;
        }
    }

};


#endif //GOBANG_ZOBRIST_HASH_H
