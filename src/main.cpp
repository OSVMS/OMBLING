#include "game.h"

int main(int /*argc*/, char* argv[]) {
    Game game;
    if (!game.initialize(argv)) {
        return -1;
    }

    game.run();
    return 0;
}
