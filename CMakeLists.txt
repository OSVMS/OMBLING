cmake_minimum_required(VERSION 3.20)
project(Ombling LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SFML_DIR "D:/libs/SFML-3.1.0/lib/cmake/SFML")
set(BUILD_SHARED_LIBS ON)

find_package(SFML CONFIG REQUIRED COMPONENTS Graphics Window System Network)

add_executable(app
    main.cpp
    game.cpp
    game_mode_test.cpp
    game_mode_solo.cpp
    game_mode_multi.cpp
    prediction_engine.cpp
    ai_easy.cpp
    ai_medium.cpp
    ai_hard.cpp
    ui_helpers.cpp
    carte.cpp
)

target_link_libraries(app
    PRIVATE
        SFML::Graphics
        SFML::Window
        SFML::System
        SFML::Network
)
