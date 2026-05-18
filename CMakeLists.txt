cmake_minimum_required(VERSION 3.20)
project(Ombling LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Par defaut on garde le chemin Windows, mais il peut etre surcharge
# (ex: via -DSFML_DIR=... dans build_macos.sh).
if(WIN32)
    set(SFML_DIR "D:/libs/SFML-3.1.0/lib/cmake/SFML" CACHE PATH "Path to SFMLConfig.cmake")
endif()
set(BUILD_SHARED_LIBS ON)

find_package(SFML CONFIG REQUIRED COMPONENTS Graphics Window System Network)

add_executable(app
    main.cpp
    game.cpp
    game_mode_test.cpp
    game_mode_solo.cpp
    game_mode_multi.cpp
    turn_resolver.cpp
    input_controller.cpp
    render_controller.cpp
    prediction_engine.cpp
    ai_strategy.cpp
    ai_easy.cpp
    ai_medium.cpp
    ai_hard.cpp
    joker_effect.cpp
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
