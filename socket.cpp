#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ctime>
#include "include/game_tree.h"

template<typename ...Args>
void log(std::ofstream &log_file, const Args &...log_infos) {
    std::time_t now = std::time(nullptr);
    std::tm *localTime = std::localtime(&now);
    char time[80];
    std::strftime(time, sizeof(time), "[%Y-%m-%d %H:%M:%S]: ", localTime);
    log_file << time;
    ((log_file << log_infos << ' '), ...);
    log_file << std::endl;
}


int main(int argc, char *argv[]) {
    int server_fd, new_socket;
    struct sockaddr_in address{};
    int addrlen = sizeof(address);
    char player_pos[32] = {0};
    char ai_pos[32] = {0};

    std::ofstream info_log(argv[2], std::ios::app); //信息日志
    std::ofstream error_log(argv[3], std::ios::app); //错误日志


    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        log(error_log, "socket failed");
        return 1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(6666);
    if (bind(server_fd, (struct sockaddr *) &address, sizeof(address)) == -1) {
        log(error_log, "bind failed");
        return 1;
    }

    if (listen(server_fd, 3) == -1) {
        log(error_log, "listen failed");
        return 1;
    }
    Game_tree gameTree(MIN_NODE, 6);
    while (true) {
        std::memset(player_pos, 0, sizeof(player_pos));
        std::memset(ai_pos, 0, sizeof(ai_pos));
        if ((new_socket = accept(server_fd, (struct sockaddr *) &address, (socklen_t *) &addrlen)) == -1) {
            log(error_log, "accept failed");
            return 1;
        }
        size_t bytesRead = recv(new_socket, player_pos, sizeof(player_pos), 0);
        if (bytesRead == -1) {
            log(error_log, "recv failed");
        } else if (bytesRead == 0) {
            log(error_log, "connect closed");
        }
        gameTree.player_next_status(std::stoi(player_pos));
        if (gameTree.lose()) {
            sprintf(ai_pos, "%d", (1 << 18) | gameTree.get_five_pos());
            send(new_socket, ai_pos, strlen(ai_pos), 0);
            break;
        }
        gameTree.AI_next_status();
        if (gameTree.win()) {
            sprintf(ai_pos, "%d", gameTree.get_five_pos() | gameTree.get_pos());
            send(new_socket, ai_pos, strlen(ai_pos), 0);
            break;
        }
        sprintf(ai_pos, "%d", gameTree.get_pos());
        send(new_socket, ai_pos, strlen(ai_pos), 0);
    }


//    Game_tree gameTree(MIN_NODE, 6);
//    gameTree.player_next_status((8 << 4) + 8);
//    gameTree.print_board();
//    while (true) {
//        gameTree.AI_next_status();
//        gameTree.print_board();
//        int x, y;
//        std::cin >> x >> y;
//        gameTree.player_next_status((x << 4) + y);
//        gameTree.print_board();
//    }

    Game_tree A(MIN_NODE, 6);
    Game_tree P(MAX_NODE, 2);
    int i = 0;
    while (true) {
        std::cout << std::dec << i++ << std::endl;
        P.AI_next_status();
        A.player_next_status(P.get_pos());
        A.print_board();
        if (A.win() || A.lose()) {
            break;
        }
        std::cout << std::dec << i++ << std::endl;
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
