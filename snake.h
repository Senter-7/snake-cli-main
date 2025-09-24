#ifndef SNAKE_H
#define SNAKE_H

#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <cstdlib>
#if defined(__unix__) || defined(__APPLE__)
#include <termios.h>
#include <unistd.h>
#endif
#include <map>
#include <deque>
#include <algorithm>
#include <fstream>
#include <set>
#include <string>
#include <utility>

const int BOARD_SIZE = 10;
const int MAX_TOP_SCORES = 10;
const int BASE_DELAY_MS = 500;
const int MIN_DELAY_MS = 100;
const int DELAY_REDUCTION_MS = 50;
const int POISON_CHANCE = 3; // 1 in 3 chance for poison food

const char DIR_RIGHT = 'r';
const char DIR_LEFT = 'l';
const char DIR_UP = 'u';
const char DIR_DOWN = 'd';

const char PAUSE_KEY = 'x';
const char QUIT_KEY = 'q';

class SnakeGame {
private:
    char direction;
    bool paused;
    std::multiset<int, std::greater<int>> topScores;
    std::deque<std::pair<int, int>> snake;
    std::pair<int, int> food;
    std::pair<int, int> poisonFood;
    int score;
    
    
    void loadScores();
    void saveScores();
    void showTopScores();
    void renderGame();
    std::pair<int, int> getNextHead(const std::pair<int, int>& current, char dir);
    void generateFood();
    void generatePoisonFood();
    bool isValidPosition(const std::pair<int, int>& pos);
    void gameOver(const std::string& reason);
    int calculateDelay();
    
public:
    SnakeGame();
    ~SnakeGame();
    
    // Game control methods
    void startGame();
    void handleInput(char input);
    void updateGame();
    void pauseGame();
    void resumeGame();
    bool isGameOver();
    
    // Getters
    int getScore() const { return score; }
    bool isPaused() const { return paused; }
    char getDirection() const { return direction; }
    const std::deque<std::pair<int, int>>& getSnake() const { return snake; }
    std::pair<int, int> getFood() const { return food; }
    std::pair<int, int> getPoisonFood() const { return poisonFood; }
    
    // Setters
    void setDirection(char dir) { direction = dir; }
};

// Global game instance pointer for input -> game communication
extern SnakeGame* g_game;

// Utility functions
std::pair<int, int> get_next_head(const std::pair<int, int>& current, char direction);
void input_handler();
void game_play();


// SnakeGame class implementation
SnakeGame::SnakeGame() : direction(DIR_RIGHT), paused(false), score(0) {
    loadScores();
    snake.push_back(std::make_pair(0, 0));
    generateFood();
    poisonFood = std::make_pair(-1, -1);
}

SnakeGame::~SnakeGame() {
    saveScores();
}

void SnakeGame::loadScores() {
    std::ifstream infile("scores.txt");
    if (!infile.is_open()) {
        // File doesn't exist or can't be opened, start with empty scores
        return;
    }
    
    int score;
    while (infile >> score) {
        if (score >= 0) { // Only accept non-negative scores
            topScores.insert(score);
        }
    }
    infile.close();
}

void SnakeGame::saveScores() {
    std::ofstream outfile("scores.txt");
    if (!outfile.is_open()) {
        std::cerr << "Warning: Could not save scores to file." << std::endl;
        return;
    }
    
    int count = 0;
    for (int score : topScores) {
        if (count++ == MAX_TOP_SCORES) break; 
        outfile << score << "\n";
    }
    outfile.close();
}

void SnakeGame::showTopScores() {
    std::cout << "\n=== Top Scores ===\n";
    int count = 0;
    for (int s : topScores) {
        std::cout << ++count << ". " << s << std::endl;
        if (count == MAX_TOP_SCORES) break;
    }
    std::cout << "==================\n";
}

void SnakeGame::renderGame() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (i == food.first && j == food.second) {
                std::cout << "🍎";
            } else if (std::find(snake.begin(), snake.end(), std::make_pair(i, j)) != snake.end()) {
                std::cout << "🐍";
            } else if (i == poisonFood.first && j == poisonFood.second) {
                std::cout << "💀";
            } else {
                std::cout << "⬜";
            }
        }
        std::cout << std::endl;
    }
}

std::pair<int, int> SnakeGame::getNextHead(const std::pair<int, int>& current, char dir) {
    std::pair<int, int> next;
    if (dir == DIR_RIGHT) {
        next = std::make_pair(current.first, (current.second + 1) % BOARD_SIZE);
    } else if (dir == DIR_LEFT) {
        next = std::make_pair(current.first, current.second == 0 ? BOARD_SIZE - 1 : current.second - 1);
    } else if (dir == DIR_DOWN) {
        next = std::make_pair((current.first + 1) % BOARD_SIZE, current.second);
    } else if (dir == DIR_UP) {
        next = std::make_pair(current.first == 0 ? BOARD_SIZE - 1 : current.first - 1, current.second);
    }
    return next;
}

void SnakeGame::generateFood() {
    do {
        food = std::make_pair(rand() % BOARD_SIZE, rand() % BOARD_SIZE);
    } while (std::find(snake.begin(), snake.end(), food) != snake.end());
}

void SnakeGame::generatePoisonFood() {
    do {
        poisonFood = std::make_pair(rand() % BOARD_SIZE, rand() % BOARD_SIZE);
    } while (std::find(snake.begin(), snake.end(), poisonFood) != snake.end() && 
             poisonFood == food);
}

bool SnakeGame::isValidPosition(const std::pair<int, int>& pos) {
    return pos.first >= 0 && pos.first < BOARD_SIZE && 
           pos.second >= 0 && pos.second < BOARD_SIZE;
}

static inline void clear_screen() {
#if defined(_WIN32)
    system("cls");
#else
    system("clear");
#endif
}

void SnakeGame::gameOver(const std::string& reason) {
    clear_screen();
    std::cout << "Game Over! " << reason << std::endl;
    std::cout << "Final Score: " << score << " points\n";
    
    topScores.insert(score);
    saveScores();
    showTopScores();
    exit(0);
}

int SnakeGame::calculateDelay() {
    int reduction = (snake.size() / 10) * DELAY_REDUCTION_MS;
    return std::max(MIN_DELAY_MS, BASE_DELAY_MS - reduction);
}

void SnakeGame::startGame() {
    clear_screen();
    showTopScores();
}

void SnakeGame::handleInput(char input) {
    std::map<char, char> keymap = {{'d', DIR_RIGHT}, {'a', DIR_LEFT}, {'w', DIR_UP}, {'s', DIR_DOWN}};
    
    if (keymap.find(input) != keymap.end()) {
        char newDirection = keymap[input];
        // Prevent snake from moving backwards into itself
        if (!(direction == DIR_RIGHT && newDirection == DIR_LEFT) &&
            !(direction == DIR_LEFT && newDirection == DIR_RIGHT) &&
            !(direction == DIR_UP && newDirection == DIR_DOWN) &&
            !(direction == DIR_DOWN && newDirection == DIR_UP)) {
            direction = newDirection;
        }
    } else if (input == PAUSE_KEY) {
        paused = !paused;
    } else if (input == QUIT_KEY) {
        saveScores();
        exit(0);
    }
}

void SnakeGame::updateGame() {
    if (paused) {
        renderGame();
        std::cout << "Game paused. Press x to continue" << std::endl;
        std::cout << "Score: " << score << " points" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        return;
    }
    
    std::pair<int, int> head = getNextHead(snake.back(), direction);
    score = snake.size() * 10;
    
    if (std::find(snake.begin(), snake.end(), head) != snake.end()) {
        gameOver("You hit yourself!");
        return;
    }
    
    if (head == food) {
        generateFood();
        
        if (rand() % POISON_CHANCE == 0) {
            generatePoisonFood();
        } else {
            poisonFood = std::make_pair(-1, -1);
        }
        snake.push_back(head);
    } else if (head == poisonFood) {
        gameOver("You ate poisonous food!");
        return;
    } else {
        snake.push_back(head);
        snake.pop_front();
    }
    
    renderGame();
    std::cout << "length of snake: " << snake.size() << std::endl;
    std::cout << "Score: " << score << " points" << std::endl;
    
    std::this_thread::sleep_for(std::chrono::milliseconds(calculateDelay()));
}

void SnakeGame::pauseGame() {
    paused = true;
}

void SnakeGame::resumeGame() {
    paused = false;
}

bool SnakeGame::isGameOver() {
    return false; // This would be set to true when game ends
}

// Utility functions (keeping original interface for compatibility)
std::pair<int, int> get_next_head(const std::pair<int, int>& current, char direction) {
    std::pair<int, int> next;
    if (direction == DIR_RIGHT) {
        next = std::make_pair(current.first, (current.second + 1) % BOARD_SIZE);
    } else if (direction == DIR_LEFT) {
        next = std::make_pair(current.first, current.second == 0 ? BOARD_SIZE - 1 : current.second - 1);
    } else if (direction == DIR_DOWN) {
        next = std::make_pair((current.first + 1) % BOARD_SIZE, current.second);
    } else if (direction == DIR_UP) {
        next = std::make_pair(current.first == 0 ? BOARD_SIZE - 1 : current.first - 1, current.second);
    }
    return next;
}

void input_handler() {
#if defined(__unix__) || defined(__APPLE__)
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    std::map<char, char> keymap = {{'d', DIR_RIGHT}, {'a', DIR_LEFT}, {'w', DIR_UP}, {'s', DIR_DOWN}};
    while (true) {
        char input = getchar();
        if (g_game && keymap.find(input) != keymap.end()) {
            g_game->handleInput(input);
        } else if (g_game && input == PAUSE_KEY) {
            g_game->handleInput(input);
        } else if (input == QUIT_KEY) {
            exit(0);
        }
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#else
    std::map<char, char> keymap = {{'d', DIR_RIGHT}, {'a', DIR_LEFT}, {'w', DIR_UP}, {'s', DIR_DOWN}};
    while (true) {
        int c = std::cin.get();
        if (c == EOF) break;
        char input = static_cast<char>(c);
        if (g_game) {
            g_game->handleInput(input);
        }
        if (input == QUIT_KEY) exit(0);
    }
#endif
}

void game_play() {
    if (!g_game) {
        g_game = new SnakeGame();
    }
    g_game->startGame();
    while (true) {
        std::cout << "\033[H";
        g_game->updateGame();
    }
}

#endif 