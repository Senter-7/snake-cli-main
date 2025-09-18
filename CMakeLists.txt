cmake_minimum_required(VERSION 3.14)
project(SnakeGame)

set(CMAKE_CXX_STANDARD 17)

# Game executable (just main.cpp + snake.h)
add_executable(snake_game main.cpp)

# GoogleTest
add_subdirectory(extern/googletest)

enable_testing()

# Test executable (snake.h included directly)
add_executable(tests snake_test.cpp)

# Link GoogleTest
target_link_libraries(tests gtest_main)

include(GoogleTest)
gtest_discover_tests(tests)
