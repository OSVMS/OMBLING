#pragma once

class Game;

class RenderController {
public:
    explicit RenderController(Game& game);

    void renderFrame();

private:
    Game& game;
};
