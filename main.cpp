#include <iostream>
#include "game_tree.h"


int main() {
    Game_tree gameTree(MIN_NODE, 6);
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

    Game_tree A(MIN_NODE, 4);
    Game_tree P(MAX_NODE, 2);
    int i = 0;
    while (true) {
        std::cout << i++ << std::endl;
        P.AI_next_status();
        A.player_next_status(P.get_pos());
        A.print_board();
        if (A.win() || A.lose()) {
            break;
        }
        std::cout << i++ << std::endl;
        A.AI_next_status();
        P.player_next_status(A.get_pos());
        A.print_board();
        if (A.win() || A.lose()) {
            break;
        }
    }
    if (A.win()) {
        std::cout << "A WIN.";
    } else {
        std::cout << "A LOSE.";
    }

    return 0;
}


