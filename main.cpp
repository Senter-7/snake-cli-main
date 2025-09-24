#include "snake.h"
#include <thread>

SnakeGame* g_game = nullptr;

int main(int argc, char* argv[]) {
    std::thread input_thread(input_handler);
    std::thread game_thread(game_play);
    input_thread.join();
    game_thread.join();
    return 0;
}


