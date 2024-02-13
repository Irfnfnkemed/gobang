cmake_minimum_required(VERSION 3.22)
project(gobang)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)

add_executable(include/game_tree.h src/game_tree.cpp src/constant.cpp include/constant.h src/evaluate.cpp include/evaluate.h include/zobrist_hash.h src/node.cpp include/node.h socket.cpp)
