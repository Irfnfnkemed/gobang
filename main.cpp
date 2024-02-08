#include <iostream>
#include "game_tree.h"


int main() {
    Game_tree gameTree(MIN_NODE);
    gameTree.player_next_status((8 << 4) + 8);
    gameTree.print_board();
    while (true) {
        gameTree.AI_next_status();
        gameTree.print_board();
        int x, y;
        std::cin >> x >> y;
        gameTree.player_next_status((x << 4) + y);
        gameTree.print_board();
    }
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
