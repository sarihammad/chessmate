cmake_minimum_required(VERSION 3.24)

# Set policy version for compatibility with older dependencies BEFORE project()
set(CMAKE_POLICY_DEFAULT_CMP0000 NEW)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

project(chessmate LANGUAGES CXX)

include(FetchContent)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(CHESSMATE_BUILD_TESTS "Build tests" ON)
option(CHESSMATE_ENABLE_ASAN "Enable AddressSanitizer in Debug" ON)
option(CHESSMATE_ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer in Debug" ON)
option(CHESSMATE_ENABLE_LTO "Enable link-time optimization" OFF)

# Dependencies (pin by commit SHA)
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        v1.12.0
)

FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG        v3.11.3
)

FetchContent_Declare(
  websocketpp
  GIT_REPOSITORY https://github.com/zaphoyd/websocketpp.git
  GIT_TAG        0.8.2
)

FetchContent_Declare(
  asio
  GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
  GIT_TAG        asio-1-28-0
)

FetchContent_Declare(
  sfml
  GIT_REPOSITORY https://github.com/SFML/SFML.git
  GIT_TAG        3.0.1
)

if (CHESSMATE_BUILD_TESTS)
  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.4.0
  )
  FetchContent_MakeAvailable(Catch2)
endif()

FetchContent_MakeAvailable(spdlog nlohmann_json websocketpp asio sfml)

# Project sources (list explicitly; avoid GLOB for reproducibility)
set(CHESSMATE_SOURCES
  src/main.cpp
  src/utils/utils.cpp
  src/gui/board_view.cpp
  src/gui/texture_manager.cpp
  src/gui/gui.cpp
  src/application/game_controller.cpp
  src/infrastructure/players/human_player.cpp
  src/infrastructure/players/ai_player.cpp
  src/infrastructure/players/network_player.cpp
  src/infrastructure/network/websocket_client.cpp
  src/infrastructure/config.cpp
  src/domain/board.cpp
  src/domain/game.cpp
  src/domain/pieces/bishop.cpp
  src/domain/pieces/king.cpp
  src/domain/pieces/knight.cpp
  src/domain/pieces/pawn.cpp
  src/domain/pieces/queen.cpp
  src/domain/pieces/rook.cpp
  src/infrastructure/ai/ai_evaluator.cpp
  src/gui/widgets/Button.cpp
  src/gui/widgets/sprite_piece.cpp
)

add_executable(chessmate ${CHESSMATE_SOURCES})
target_include_directories(chessmate PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(chessmate PRIVATE 
  sfml-graphics
  sfml-window
  sfml-system
  sfml-audio
  nlohmann_json::nlohmann_json
  spdlog::spdlog
)

target_compile_definitions(chessmate PRIVATE ASIO_STANDALONE)

target_compile_features(chessmate PRIVATE cxx_std_23)

# Warnings-as-errors
if (MSVC)
  target_compile_options(chessmate PRIVATE /W4 /permissive- /utf-8 /WX)
else()
  target_compile_options(chessmate PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wshadow -Wformat=2 -Werror)
endif()

# Sanitizers (Linux/macOS only, not Windows)
if (CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT MSVC AND NOT WIN32)
  if (CHESSMATE_ENABLE_ASAN)
    target_compile_options(chessmate PRIVATE -fsanitize=address -fno-omit-frame-pointer)
    target_link_options(chessmate PRIVATE -fsanitize=address -fno-omit-frame-pointer)
  endif()
  if (CHESSMATE_ENABLE_UBSAN)
    target_compile_options(chessmate PRIVATE -fsanitize=undefined)
    target_link_options(chessmate PRIVATE -fsanitize=undefined)
  endif()
endif()

# LTO
if (CHESSMATE_ENABLE_LTO)
  include(CheckIPOSupported)
  check_ipo_supported(RESULT ipo_supported OUTPUT ipo_msg)
  if (ipo_supported)
    set_property(TARGET chessmate PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
  endif()
endif()

# Copy assets to build directory
add_custom_command(TARGET chessmate POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:chessmate>/assets)

# Tests
if (CHESSMATE_BUILD_TESTS)
  add_subdirectory(tests)
endif()

# Installation
install(TARGETS chessmate DESTINATION bin)
install(DIRECTORY assets DESTINATION share/chessmate)