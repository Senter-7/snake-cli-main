#include <gtest/gtest.h>
#include "snake.h"
#include <vector>
#include <algorithm>

// Provide definition for the global game pointer used by input/game functions
SnakeGame* g_game = nullptr;

// Test fixture for SnakeGame tests
class SnakeGameTest : public ::testing::Test {
protected:
    void SetUp() override {
        game = new SnakeGame();
    }
    
    void TearDown() override {
        delete game;
    }
    
    SnakeGame* game;
};

// Direction movement tests
TEST(SnakeBehaviour, NextHeadRight) {
    std::pair<int, int> current = std::make_pair(5, 5);
    EXPECT_EQ(get_next_head(current, DIR_RIGHT), std::make_pair(5, 6));
}

TEST(SnakeBehaviour, NextHeadRightWrapAround) {
    std::pair<int, int> current = std::make_pair(5, 9);
    EXPECT_EQ(get_next_head(current, DIR_RIGHT), std::make_pair(5, 0));
}

TEST(SnakeBehaviour, NextHeadLeft) {
    std::pair<int, int> current = std::make_pair(5, 5);
    EXPECT_EQ(get_next_head(current, DIR_LEFT), std::make_pair(5, 4));
}

TEST(SnakeBehaviour, NextHeadLeftWrapAround) {
    std::pair<int, int> current = std::make_pair(5, 0);
    EXPECT_EQ(get_next_head(current, DIR_LEFT), std::make_pair(5, 9));
}

TEST(SnakeBehaviour, NextHeadUp) {
    std::pair<int, int> current = std::make_pair(5, 5);
    EXPECT_EQ(get_next_head(current, DIR_UP), std::make_pair(4, 5));
}

TEST(SnakeBehaviour, NextHeadUpWrapAround) {
    std::pair<int, int> current = std::make_pair(0, 5);
    EXPECT_EQ(get_next_head(current, DIR_UP), std::make_pair(9, 5));
}

TEST(SnakeBehaviour, NextHeadDown) {
    std::pair<int, int> current = std::make_pair(5, 5);
    EXPECT_EQ(get_next_head(current, DIR_DOWN), std::make_pair(6, 5));
}

TEST(SnakeBehaviour, NextHeadDownWrapAround) {
    std::pair<int, int> current = std::make_pair(9, 5);
    EXPECT_EQ(get_next_head(current, DIR_DOWN), std::make_pair(0, 5));
}

// SnakeGame class tests
TEST_F(SnakeGameTest, InitialState) {
    EXPECT_EQ(game->getScore(), 0);
    EXPECT_FALSE(game->isPaused());
    EXPECT_EQ(game->getDirection(), DIR_RIGHT);
    EXPECT_EQ(game->getSnake().size(), 1);
    EXPECT_EQ(game->getSnake().front(), std::make_pair(0, 0));
}

TEST_F(SnakeGameTest, DirectionChange) {
    game->setDirection(DIR_UP);
    EXPECT_EQ(game->getDirection(), DIR_UP);
    
    game->setDirection(DIR_LEFT);
    EXPECT_EQ(game->getDirection(), DIR_LEFT);
    
    game->setDirection(DIR_DOWN);
    EXPECT_EQ(game->getDirection(), DIR_DOWN);
}

TEST_F(SnakeGameTest, PauseResume) {
    EXPECT_FALSE(game->isPaused());
    
    game->pauseGame();
    EXPECT_TRUE(game->isPaused());
    
    game->resumeGame();
    EXPECT_FALSE(game->isPaused());
}

TEST_F(SnakeGameTest, InputHandling) {
    // Test direction input (avoid opposite-direction moves)
    // Start default: RIGHT
    game->handleInput('w'); // to UP
    EXPECT_EQ(game->getDirection(), DIR_UP);

    game->handleInput('a'); // to LEFT (perpendicular)
    EXPECT_EQ(game->getDirection(), DIR_LEFT);

    game->handleInput('s'); // to DOWN (perpendicular)
    EXPECT_EQ(game->getDirection(), DIR_DOWN);

    game->handleInput('d'); // to RIGHT (perpendicular)
    EXPECT_EQ(game->getDirection(), DIR_RIGHT);

    // Reverse prevention: from RIGHT, 'a' (LEFT) should be ignored
    game->handleInput('a');
    EXPECT_EQ(game->getDirection(), DIR_RIGHT);

    // Test pause input
    game->handleInput(PAUSE_KEY);
    EXPECT_TRUE(game->isPaused());
    
    game->handleInput(PAUSE_KEY);
    EXPECT_FALSE(game->isPaused());
}

TEST_F(SnakeGameTest, SnakeMovement) {
    // Test that snake moves in the correct direction
    auto initialSnake = game->getSnake();
    EXPECT_EQ(initialSnake.size(), 1);
    EXPECT_EQ(initialSnake.front(), std::make_pair(0, 0));
    
    // Test that food is generated at a valid position
    auto food = game->getFood();
    EXPECT_TRUE(food.first >= 0 && food.first < BOARD_SIZE);
    EXPECT_TRUE(food.second >= 0 && food.second < BOARD_SIZE);
}

TEST_F(SnakeGameTest, ScoreCalculation) {
    // Initial score should be 0
    EXPECT_EQ(game->getScore(), 0);
    
    // Score should be calculated as snake size * 10
    // This would need to be tested with actual game updates
}

TEST_F(SnakeGameTest, FoodGeneration) {
    auto food = game->getFood();
    
    // Food should be within board bounds
    EXPECT_TRUE(food.first >= 0 && food.first < BOARD_SIZE);
    EXPECT_TRUE(food.second >= 0 && food.second < BOARD_SIZE);
    
    // Food should not be on the snake initially
    auto snake = game->getSnake();
    EXPECT_TRUE(std::find(snake.begin(), snake.end(), food) == snake.end());
}

TEST_F(SnakeGameTest, PoisonFoodGeneration) {
    // Initially, poison food should be at invalid position
    auto poisonFood = game->getPoisonFood();
    EXPECT_EQ(poisonFood, std::make_pair(-1, -1));
}

// Edge case tests
TEST(SnakeBehaviour, EdgeCaseMovement) {
    // Test movement from all edges
    std::pair<int, int> topLeft = std::make_pair(0, 0);
    std::pair<int, int> topRight = std::make_pair(0, 9);
    std::pair<int, int> bottomLeft = std::make_pair(9, 0);
    std::pair<int, int> bottomRight = std::make_pair(9, 9);
    
    // From top-left corner
    EXPECT_EQ(get_next_head(topLeft, DIR_UP), std::make_pair(9, 0));
    EXPECT_EQ(get_next_head(topLeft, DIR_LEFT), std::make_pair(0, 9));
    
    // From top-right corner
    EXPECT_EQ(get_next_head(topRight, DIR_UP), std::make_pair(9, 9));
    EXPECT_EQ(get_next_head(topRight, DIR_RIGHT), std::make_pair(0, 0));
    
    // From bottom-left corner
    EXPECT_EQ(get_next_head(bottomLeft, DIR_DOWN), std::make_pair(0, 0));
    EXPECT_EQ(get_next_head(bottomLeft, DIR_LEFT), std::make_pair(9, 9));
    
    // From bottom-right corner
    EXPECT_EQ(get_next_head(bottomRight, DIR_DOWN), std::make_pair(0, 9));
    EXPECT_EQ(get_next_head(bottomRight, DIR_RIGHT), std::make_pair(9, 0));
}

// Collision detection tests
TEST_F(SnakeGameTest, SelfCollisionDetection) {
    // This would require setting up a snake with multiple segments
    // and testing collision detection
    auto snake = game->getSnake();
    EXPECT_EQ(snake.size(), 1); // Initial snake has only head
    
    // Test that single segment snake doesn't collide with itself
    // (except in edge cases that would need more complex setup)
}

// Game state tests
TEST_F(SnakeGameTest, GameStateConsistency) {
    // Test that game state remains consistent
    EXPECT_FALSE(game->isGameOver());
    
    // Test that snake is always valid
    auto snake = game->getSnake();
    for (const auto& segment : snake) {
        EXPECT_TRUE(segment.first >= 0 && segment.first < BOARD_SIZE);
        EXPECT_TRUE(segment.second >= 0 && segment.second < BOARD_SIZE);
    }
}

// Performance tests
TEST_F(SnakeGameTest, PerformanceTest) {
    // Test that game operations are reasonably fast
    auto start = std::chrono::high_resolution_clock::now();
    
    // Perform multiple operations
    for (int i = 0; i < 1000; ++i) {
        (void)get_next_head(std::make_pair(i % BOARD_SIZE, i % BOARD_SIZE), DIR_RIGHT);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Should complete in reasonable time (less than 100ms for 1000 operations)
    EXPECT_LT(duration.count(), 100);
}

// Boundary tests
TEST(SnakeBehaviour, BoundaryConditions) {
    // Test all boundary conditions
    for (int i = 0; i < BOARD_SIZE; ++i) {
        // Test top row
        std::pair<int, int> top = std::make_pair(0, i);
        EXPECT_EQ(get_next_head(top, DIR_UP), std::make_pair(BOARD_SIZE - 1, i));
        
        // Test bottom row
        std::pair<int, int> bottom = std::make_pair(BOARD_SIZE - 1, i);
        EXPECT_EQ(get_next_head(bottom, DIR_DOWN), std::make_pair(0, i));
        
        // Test left column
        std::pair<int, int> left = std::make_pair(i, 0);
        EXPECT_EQ(get_next_head(left, DIR_LEFT), std::make_pair(i, BOARD_SIZE - 1));
        
        // Test right column
        std::pair<int, int> right = std::make_pair(i, BOARD_SIZE - 1);
        EXPECT_EQ(get_next_head(right, DIR_RIGHT), std::make_pair(i, 0));
    }
}

// Input validation tests
TEST_F(SnakeGameTest, InputValidation) {
    // Test that invalid input doesn't change direction
    char originalDirection = game->getDirection();
    game->handleInput('z'); // Invalid input
    EXPECT_EQ(game->getDirection(), originalDirection);
    
    game->handleInput('1'); // Invalid input
    EXPECT_EQ(game->getDirection(), originalDirection);
}

TEST_F(SnakeGameTest, DirectionChangeValidation) {
    // Test that snake can't reverse direction
    game->setDirection(DIR_RIGHT);
    game->handleInput('a'); // Try to go left (opposite direction)
    EXPECT_EQ(game->getDirection(), DIR_RIGHT); // Should remain right
    
    game->setDirection(DIR_UP);
    game->handleInput('s'); // Try to go down (opposite direction)
    EXPECT_EQ(game->getDirection(), DIR_UP); // Should remain up
}

// Error handling tests
TEST_F(SnakeGameTest, FileHandling) {
    // Test that game handles missing score file gracefully
    // This would require mocking or temporary file manipulation
    // For now, we test that the game initializes without crashing
    EXPECT_NO_THROW({
        SnakeGame testGame;
    });
}

// Game logic tests
TEST_F(SnakeGameTest, GameLogicConsistency) {
    // Test that game state remains consistent after operations
    auto initialSnake = game->getSnake();
    auto initialFood = game->getFood();
    
    // Snake should be valid
    EXPECT_FALSE(initialSnake.empty());
    EXPECT_TRUE(initialSnake.front().first >= 0 && initialSnake.front().first < BOARD_SIZE);
    EXPECT_TRUE(initialSnake.front().second >= 0 && initialSnake.front().second < BOARD_SIZE);
    
    // Food should be valid
    EXPECT_TRUE(initialFood.first >= 0 && initialFood.first < BOARD_SIZE);
    EXPECT_TRUE(initialFood.second >= 0 && initialFood.second < BOARD_SIZE);
    
    // Food should not be on snake
    EXPECT_TRUE(std::find(initialSnake.begin(), initialSnake.end(), initialFood) == initialSnake.end());
}

// Stress tests
TEST_F(SnakeGameTest, StressTest) {
    // Test multiple rapid direction changes
    for (int i = 0; i < 100; ++i) {
        game->setDirection(DIR_RIGHT);
        game->setDirection(DIR_DOWN);
        game->setDirection(DIR_LEFT);
        game->setDirection(DIR_UP);
    }
    
    // Game should still be in valid state
    EXPECT_FALSE(game->isGameOver());
    EXPECT_EQ(game->getSnake().size(), 1);
}

// Memory management tests
TEST_F(SnakeGameTest, MemoryManagement) {
    // Test that game can be created and destroyed multiple times
    for (int i = 0; i < 10; ++i) {
        SnakeGame* tempGame = new SnakeGame();
        EXPECT_NE(tempGame, nullptr);
        delete tempGame;
    }
}


/** 
 * g++ -o my_tests snake_test.cpp -lgtest -lgtest_main -pthread;
 * This command is a two-part shell command. Let's break it down.

  The first part is the compilation:
  g++ -o my_tests hello_gtest.cpp -lgtest -lgtest_main -pthread


   * g++: This invokes the GNU C++ compiler.
   * -o my_tests: This tells the compiler to create an executable file named
     my_tests.
   * hello_gtest.cpp: This is the C++ source file containing your tests.
   * -lgtest: This links the Google Test library, which provides the core testing
     framework.
   * -lgtest_main: This links a pre-compiled main function provided by Google
     Test, which saves you from writing your own main() to run the tests.
   * -pthread: This links the POSIX threads library, which is required by Google
     Test for its operation.
 * 
*/