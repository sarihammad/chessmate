cmake_minimum_required(VERSION 3.20)
project(Chessmate VERSION 1.0.0 LANGUAGES CXX)

# Options
option(CHESSMATE_BUILD_TESTS "Build tests" ON)
option(CHESSMATE_ENABLE_SANITIZERS "Enable ASAN/UBSAN in Debug" ON)
option(CHESSMATE_ENABLE_LTO "Enable link time optimization" OFF)
option(CHESSMATE_WARNINGS_AS_ERRORS "Treat warnings as errors" ON)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler warnings
if(CHESSMATE_WARNINGS_AS_ERRORS)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(chessmate PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wshadow -Wformat=2 -Werror
        )
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(chessmate PRIVATE /W4 /permissive- /utf-8 /WX)
    endif()
endif()

# Sanitizers
if(CHESSMATE_ENABLE_SANITIZERS AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(chessmate PRIVATE -fsanitize=address,undefined)
        target_link_options(chessmate PRIVATE -fsanitize=address,undefined)
    endif()
endif()

# Link Time Optimization
if(CHESSMATE_ENABLE_LTO)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)
    if(ipo_supported)
        set_property(TARGET chessmate PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
        message(SEND_ERROR "IPO is not supported: ${ipo_error}")
    endif()
endif()

# Use CPM.cmake for dependency management
include(cmake/CPM.cmake)

# SFML
CPMAddPackage(
    NAME sfml
    GITHUB_REPOSITORY SFML/SFML
    GIT_TAG 3.0.1
)

# WebSocket++
CPMAddPackage(
    NAME websocketpp
    GITHUB_REPOSITORY zaphoyd/websocketpp
    GIT_TAG 0.8.2
)

# ASIO
CPMAddPackage(
    NAME asio
    GITHUB_REPOSITORY chriskohlhoff/asio
    GIT_TAG asio-1-28-0
)

# nlohmann/json
CPMAddPackage(
    NAME nlohmann_json
    GITHUB_REPOSITORY nlohmann/json
    GIT_TAG v3.11.3
)

# Catch2 for testing
if(CHESSMATE_BUILD_TESTS)
    CPMAddPackage(
        NAME Catch2
        GITHUB_REPOSITORY catchorg/Catch2
        GIT_TAG v3.4.0
    )
endif()

# Source files (explicit list instead of GLOB)
set(SOURCES
    src/main.cpp
    src/application/game_controller.cpp
    src/domain/board.cpp
    src/domain/game.cpp
    src/domain/pieces/bishop.cpp
    src/domain/pieces/king.cpp
    src/domain/pieces/knight.cpp
    src/domain/pieces/pawn.cpp
    src/domain/pieces/queen.cpp
    src/domain/pieces/rook.cpp
    src/gui/board_view.cpp
    src/gui/gui.cpp
    src/gui/texture_manager.cpp
    src/gui/widgets/Button.cpp
    src/gui/widgets/sprite_piece.cpp
    src/infrastructure/ai/ai_evaluator.cpp
    src/infrastructure/network/websocket_client.cpp
    src/infrastructure/players/ai_player.cpp
    src/infrastructure/players/human_player.cpp
    src/infrastructure/players/network_player.cpp
    src/utils/utils.cpp
)

add_executable(chessmate ${SOURCES})

target_include_directories(chessmate PRIVATE
    include
    ${websocketpp_SOURCE_DIR}
    ${asio_SOURCE_DIR}/asio/include
)

target_compile_definitions(chessmate PRIVATE ASIO_STANDALONE)

target_link_libraries(chessmate PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
    nlohmann_json::nlohmann_json
)

# Copy assets to build directory
add_custom_command(TARGET chessmate POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:chessmate>/assets)

# Testing
if(CHESSMATE_BUILD_TESTS)
    enable_testing()
    
    # Add test executable
    add_executable(chessmate_tests
        tests/test_domain.cpp
        tests/test_application.cpp
    )
    
    target_link_libraries(chessmate_tests PRIVATE
        chessmate
        Catch2::Catch2WithMain
    )
    
    include(CTest)
    include(Catch)
    catch_discover_tests(chessmate_tests)
endif()

# Installation
install(TARGETS chessmate DESTINATION bin)
install(DIRECTORY assets DESTINATION share/chessmate)