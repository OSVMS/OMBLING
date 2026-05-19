#include "render_controller.h"

#include "game.h"

RenderController::RenderController(Game& game) : game(game) {}

void RenderController::renderFrame() {
    if (game.ecran == Game::Ecran::Menu) {
        game.renderMenu();
    } else if (game.ecran == Game::Ecran::MenuSolo) {
        game.renderMenuSolo();
    } else if (game.ecran == Game::Ecran::MenuMulti) {
        game.renderMenuMulti();
    } else if (game.ecran == Game::Ecran::MenuDeck) {
        game.renderMenuDeck();
    } else if (game.ecran == Game::Ecran::MenuMultiOnline) {
        game.renderMenuMultiOnline();
    } else if (game.ecran == Game::Ecran::OnlineWaiting) {
        game.renderOnlineWaiting();
    } else if (game.ecran == Game::Ecran::FinPartie) {
        game.renderFinPartie();
    } else {
        game.renderJeu();
    }
    game.window.display();
}
