# Tests CMakeLists.txt
cmake_minimum_required(VERSION 3.24)

# Simple test executable using existing test files
add_executable(chessmate_tests
  perft_test.cpp
  movegen_test.cpp
)

# Link with Catch2 only (tests are standalone for now)
target_link_libraries(chessmate_tests
  PRIVATE
    Catch2::Catch2WithMain
)

# Set include directories
target_include_directories(chessmate_tests
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

# Add test to CTest
add_test(NAME chessmate_tests COMMAND chessmate_tests)
