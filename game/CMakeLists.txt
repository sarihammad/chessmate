cmake_minimum_required(VERSION 3.14)
project(Chessmate)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)

FetchContent_Declare(
    sfml
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
)
FetchContent_MakeAvailable(sfml)

FetchContent_Declare(
    websocketpp
    GIT_REPOSITORY https://github.com/zaphoyd/websocketpp.git
    GIT_TAG 0.8.2
)
FetchContent_MakeAvailable(websocketpp)

FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG asio-1-28-0
)
FetchContent_MakeAvailable(asio)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/include/gui/gui.cpp
    ${CMAKE_SOURCE_DIR}/main.cpp
)
list(FILTER SOURCES EXCLUDE REGEX ".*/_deps/.*")

add_executable(chessmate ${SOURCES})

target_include_directories(chessmate PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${websocketpp_SOURCE_DIR}
    ${asio_SOURCE_DIR}/asio/include
)

target_compile_definitions(chessmate PRIVATE ASIO_STANDALONE)

target_link_libraries(chessmate PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
    # websocketpp
    nlohmann_json::nlohmann_json
)

add_custom_command(TARGET chessmate POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:chessmate>/assets)