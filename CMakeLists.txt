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
    src/main.cpp
    src/core/game.cpp
    src/core/game_mode_test.cpp
    src/core/game_mode_solo.cpp
    src/core/game_mode_multi.cpp
    src/core/turn_resolver.cpp
    src/core/prediction_engine.cpp
    src/ui/input_controller.cpp
    src/ui/render_controller.cpp
    src/ui/ui_helpers.cpp
    src/ai/ai_strategy.cpp
    src/ai/ai_easy.cpp
    src/ai/ai_medium.cpp
    src/ai/ai_hard.cpp
    src/jokers/joker_effect.cpp
    src/model/carte.cpp
)

target_include_directories(app
    PRIVATE
        src
        src/core
        src/ui
        src/ai
        src/jokers
        src/model
)

target_link_libraries(app
    PRIVATE
        SFML::Graphics
        SFML::Window
        SFML::System
        SFML::Network
)
