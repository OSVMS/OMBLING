#include "input_controller.h"

#include "game.h"
#include "ui_helpers.h"

#include <algorithm>
#include <optional>

InputController::InputController(Game& game) : game(game) {}

void InputController::handleEvent(const sf::Event& event) {
    if (event.is<sf::Event::Closed>()) {
        game.window.close();
        return;
    }

    if (const auto* resized = event.getIf<sf::Event::Resized>()) {
        handleResized(*resized);
    }

    if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
        handleKeyPressed(*key);
    }

    if (const auto* text = event.getIf<sf::Event::TextEntered>()) {
        handleTextEntered(*text);
    }

    if (const auto* click = event.getIf<sf::Event::MouseButtonPressed>()) {
        handleMousePressed(*click);
    }
}

void InputController::handleResized(const sf::Event::Resized& resized) {
    float winW = static_cast<float>(resized.size.x);
    float winH = static_cast<float>(resized.size.y);
    float ratio = std::min(winW / 800.f, winH / 560.f);
    float vpW = (800.f * ratio) / winW;
    float vpH = (560.f * ratio) / winH;
    game.vue.setViewport(sf::FloatRect(
        {(1.f - vpW) / 2.f, (1.f - vpH) / 2.f},
        {vpW, vpH}
    ));
    game.window.setView(game.vue);
}

void InputController::handleKeyPressed(const sf::Event::KeyPressed& key) {
    if (game.ecran == Game::Ecran::FinPartie) {
        if (key.code == sf::Keyboard::Key::Enter || key.code == sf::Keyboard::Key::Escape) {
            game.ecran = Game::Ecran::Menu;
        }
        return;
    }

    if (game.ecran == Game::Ecran::Menu) {
        if (key.code == sf::Keyboard::Key::Up) {
            game.selectionMenu = (game.selectionMenu + 3) % 4;
        } else if (key.code == sf::Keyboard::Key::Down) {
            game.selectionMenu = (game.selectionMenu + 1) % 4;
        } else if (key.code == sf::Keyboard::Key::Enter) {
            switch (game.selectionMenu) {
                case 0:
                    game.demarrerPartie(Game::ModeJeu::PartieTest);
                    break;
                case 1:
                    game.selectionMenuSolo = 0;
                    game.ecran = Game::Ecran::MenuSolo;
                    break;
                case 2:
                    game.selectionMenuMulti = 0;
                    game.ecran = Game::Ecran::MenuMulti;
                    break;
                case 3:
                    game.window.close();
                    break;
                default:
                    break;
            }
        }
        return;
    }

    if (game.ecran == Game::Ecran::MenuSolo) {
        if (key.code == sf::Keyboard::Key::Up) {
            game.selectionMenuSolo = (game.selectionMenuSolo + 3) % 4;
        } else if (key.code == sf::Keyboard::Key::Down) {
            game.selectionMenuSolo = (game.selectionMenuSolo + 1) % 4;
        } else if (key.code == sf::Keyboard::Key::Enter) {
            if (game.selectionMenuSolo == 0) {
                game.prochaineMode = Game::ModeJeu::SoloFacile;
                game.ecranApresDeck = Game::Ecran::Jeu;
                game.ecran = Game::Ecran::MenuDeck;
            } else if (game.selectionMenuSolo == 1) {
                game.prochaineMode = Game::ModeJeu::SoloMoyen;
                game.ecranApresDeck = Game::Ecran::Jeu;
                game.ecran = Game::Ecran::MenuDeck;
            } else if (game.selectionMenuSolo == 2) {
                game.prochaineMode = Game::ModeJeu::SoloDifficile;
                game.ecranApresDeck = Game::Ecran::Jeu;
                game.ecran = Game::Ecran::MenuDeck;
            } else if (game.selectionMenuSolo == 3) {
                game.ecran = Game::Ecran::Menu;
            }
        }
        return;
    }

    if (game.ecran == Game::Ecran::MenuMulti) {
        if (key.code == sf::Keyboard::Key::Up) {
            game.selectionMenuMulti = (game.selectionMenuMulti + 2) % 3;
        } else if (key.code == sf::Keyboard::Key::Down) {
            game.selectionMenuMulti = (game.selectionMenuMulti + 1) % 3;
        } else if (key.code == sf::Keyboard::Key::Enter) {
            if (game.selectionMenuMulti == 0) {
                game.prochaineMode = Game::ModeJeu::MultiMaison;
                game.ecranApresDeck = Game::Ecran::Jeu;
                game.ecran = Game::Ecran::MenuDeck;
            } else if (game.selectionMenuMulti == 1) {
                game.selectionOnline = 0;
                game.ecranApresDeck = Game::Ecran::MenuMultiOnline;
                game.ecran = Game::Ecran::MenuDeck;
            } else if (game.selectionMenuMulti == 2) {
                game.ecran = Game::Ecran::Menu;
            }
        }
        return;
    }

    if (game.ecran == Game::Ecran::MenuDeck) {
        if (key.code == sf::Keyboard::Key::Up) {
            game.selectionDeck = (game.selectionDeck + 5) % 5;
            game.updateDeckInfoText();
        } else if (key.code == sf::Keyboard::Key::Down) {
            game.selectionDeck = (game.selectionDeck + 1) % 5;
            game.updateDeckInfoText();
        } else if (key.code == sf::Keyboard::Key::Enter) {
            game.deckSelection = static_cast<Game::DeckType>(game.selectionDeck);
            game.assignPlayerDeck();
            if (game.ecranApresDeck == Game::Ecran::Jeu) {
                game.demarrerPartie(game.prochaineMode);
            } else {
                game.ecran = game.ecranApresDeck;
            }
        } else if (key.code == sf::Keyboard::Key::Escape) {
            game.ecran = game.ecranApresDeck;
        }
        return;
    }

    if (game.ecran == Game::Ecran::Jeu) {
        if (key.code == sf::Keyboard::Key::P) {
            if (game.estModeMultijoueurOnline()) {
                if (game.joueurCourant != game.myPlayerNumber || !game.peutPasserMainMaintenant()) {
                    return;
                }
                if (game.onlineHost && game.myPlayerNumber == 1) {
                    const int joueurQuiPasse = game.joueurCourant;
                    game.passerMainVolontairement();
                    game.sendPassExecution(joueurQuiPasse);
                } else if (!game.onlineHost && game.myPlayerNumber == 2) {
                    game.sendPassRequest();
                }
            } else {
                game.passerMainVolontairement();
            }
            return;
        }
        if (key.code == sf::Keyboard::Key::Num1 || key.code == sf::Keyboard::Key::Numpad1) {
            game.useJoker(JokerAction::X2);
            return;
        }
        if (key.code == sf::Keyboard::Key::Num2 || key.code == sf::Keyboard::Key::Numpad2) {
            game.useJoker(JokerAction::Mix);
            return;
        }
        if (key.code == sf::Keyboard::Key::Num3 || key.code == sf::Keyboard::Key::Numpad3) {
            game.useJoker(JokerAction::Vision);
            return;
        }
        if (key.code == sf::Keyboard::Key::Num4 || key.code == sf::Keyboard::Key::Numpad4) {
            game.useJoker(JokerAction::Plus5);
            return;
        }
        if (key.code == sf::Keyboard::Key::Num5 || key.code == sf::Keyboard::Key::Numpad5) {
            game.useJoker(JokerAction::Minus5);
            return;
        }
        if (key.code == sf::Keyboard::Key::Num6 || key.code == sf::Keyboard::Key::Numpad6) {
            game.useJoker(JokerAction::Tirage);
            return;
        }
        if (key.code == sf::Keyboard::Key::Num7 || key.code == sf::Keyboard::Key::Numpad7) {
            game.useJoker(JokerAction::Retry);
            return;
        }
        if (key.code == sf::Keyboard::Key::Num8 || key.code == sf::Keyboard::Key::Numpad8) {
            game.useJoker(JokerAction::Swap);
            return;
        }
        if (key.code == sf::Keyboard::Key::N) {
            game.useJoker(JokerAction::Next);
            return;
        }
    }

    if (game.ecran == Game::Ecran::MenuMultiOnline) {
        if (key.code == sf::Keyboard::Key::Up) {
            game.selectionOnline = (game.selectionOnline + 5) % 5;
        } else if (key.code == sf::Keyboard::Key::Down) {
            game.selectionOnline = (game.selectionOnline + 1) % 5;
        } else if (key.code == sf::Keyboard::Key::Backspace) {
            if (game.selectionOnline == 0 && !game.onlineIp.empty()) {
                game.onlineIp.pop_back();
            } else if (game.selectionOnline == 1 && !game.onlinePort.empty()) {
                game.onlinePort.pop_back();
            }
        } else if (key.code == sf::Keyboard::Key::Enter) {
            if (game.selectionOnline == 2) {
                if (game.startOnlineHost(game.onlinePort)) {
                    game.ecran = Game::Ecran::OnlineWaiting;
                }
            } else if (game.selectionOnline == 3) {
                if (game.startOnlineClient(game.onlineIp, game.onlinePort)) {
                    game.ecran = Game::Ecran::OnlineWaiting;
                }
            } else if (game.selectionOnline == 4) {
                game.resetOnlineSession();
                game.ecran = Game::Ecran::MenuMulti;
            }
        }
        return;
    }

    if (game.ecran == Game::Ecran::OnlineWaiting) {
        if (key.code == sf::Keyboard::Key::Escape) {
            game.resetOnlineSession();
            game.ecran = Game::Ecran::MenuMulti;
        }
        return;
    }

    if (key.code == sf::Keyboard::Key::Left || key.code == sf::Keyboard::Key::Right) {
        game.selectionJeu = 1 - game.selectionJeu;
    } else if (key.code == sf::Keyboard::Key::Enter) {
        if (game.selectionJeu == 0) {
            if (game.estModeMultijoueurOnline()) {
                if (game.joueurCourant != game.myPlayerNumber) {
                    return;
                }
                int predictionIndex = 0;
                const auto& types = PredictionEngine::predictionTypes();
                for (size_t i = 0; i < types.size(); ++i) {
                    if (types[i] == game.predictionActive) {
                        predictionIndex = static_cast<int>(i);
                        break;
                    }
                }
                if (game.onlineHost && game.myPlayerNumber == 1) {
                    int prevId = game.cartes[0] ? game.cartes[0]->getId() : 0;
                    game.tirerDeuxCartes();
                    game.sendExecuteMove(1, predictionIndex, prevId, game.derniereCarteId);
                } else if (!game.onlineHost && game.myPlayerNumber == 2) {
                    game.sendMoveRequest(predictionIndex);
                }
            } else {
                game.tirerDeuxCartes();
            }
        } else {
            game.ecran = Game::Ecran::Menu;
        }
    }
}

void InputController::handleMousePressed(const sf::Event::MouseButtonPressed& click) {
    sf::Vector2f pos = game.window.mapPixelToCoords(sf::Vector2i(click.position));

    if (game.ecran == Game::Ecran::Menu) {
        if (game.btnMenuPartieTest.getGlobalBounds().contains(pos)) {
            game.demarrerPartie(Game::ModeJeu::PartieTest);
        } else if (game.btnMenuSolo.getGlobalBounds().contains(pos)) {
            game.selectionMenuSolo = 0;
            game.ecran = Game::Ecran::MenuSolo;
        } else if (game.btnMenuMulti.getGlobalBounds().contains(pos)) {
            game.selectionMenuMulti = 0;
            game.ecran = Game::Ecran::MenuMulti;
        } else if (game.btnMenuQuitter.getGlobalBounds().contains(pos)) {
            game.window.close();
        }
        return;
    }

    if (game.ecran == Game::Ecran::MenuSolo) {
        if (game.btnSoloFacile.getGlobalBounds().contains(pos)) {
            game.prochaineMode = Game::ModeJeu::SoloFacile;
            game.ecranApresDeck = Game::Ecran::Jeu;
            game.ecran = Game::Ecran::MenuDeck;
        } else if (game.btnSoloMoyen.getGlobalBounds().contains(pos)) {
            game.prochaineMode = Game::ModeJeu::SoloMoyen;
            game.ecranApresDeck = Game::Ecran::Jeu;
            game.ecran = Game::Ecran::MenuDeck;
        } else if (game.btnSoloDifficile.getGlobalBounds().contains(pos)) {
            game.prochaineMode = Game::ModeJeu::SoloDifficile;
            game.ecranApresDeck = Game::Ecran::Jeu;
            game.ecran = Game::Ecran::MenuDeck;
        } else if (game.btnSoloRetour.getGlobalBounds().contains(pos)) {
            game.ecran = Game::Ecran::Menu;
        }
        return;
    }

    if (game.ecran == Game::Ecran::MenuMulti) {
        if (game.btnMultiMaison.getGlobalBounds().contains(pos)) {
            game.prochaineMode = Game::ModeJeu::MultiMaison;
            game.ecranApresDeck = Game::Ecran::Jeu;
            game.ecran = Game::Ecran::MenuDeck;
        } else if (game.btnMultiInternet.getGlobalBounds().contains(pos)) {
            game.selectionOnline = 0;
            game.ecranApresDeck = Game::Ecran::MenuMultiOnline;
            game.ecran = Game::Ecran::MenuDeck;
        } else if (game.btnMultiRetour.getGlobalBounds().contains(pos)) {
            game.ecran = Game::Ecran::Menu;
        }
        return;
    }

    if (game.ecran == Game::Ecran::MenuMultiOnline) {
        if (game.btnOnlineHost.getGlobalBounds().contains(pos)) {
            if (game.startOnlineHost(game.onlinePort)) {
                game.ecran = Game::Ecran::OnlineWaiting;
            }
        } else if (game.btnOnlineJoin.getGlobalBounds().contains(pos)) {
            if (game.startOnlineClient(game.onlineIp, game.onlinePort)) {
                game.ecran = Game::Ecran::OnlineWaiting;
            }
        } else if (game.btnOnlineBack.getGlobalBounds().contains(pos)) {
            game.resetOnlineSession();
            game.ecran = Game::Ecran::MenuMulti;
        } else if (game.btnOnlineIp.getGlobalBounds().contains(pos)) {
            game.selectionOnline = 0;
        } else if (game.btnOnlinePort.getGlobalBounds().contains(pos)) {
            game.selectionOnline = 1;
        }
        return;
    }

    if (game.ecran == Game::Ecran::MenuDeck) {
        for (int i = 0; i < 6; ++i) {
            if (game.btnDeckOptions[i].getGlobalBounds().contains(pos)) {
                if (i == 5) {
                    game.ecran = game.ecranApresDeck;
                    return;
                }
                game.selectionDeck = i;
                game.updateDeckInfoText();
                game.deckSelection = static_cast<Game::DeckType>(i);
                game.assignPlayerDeck();
                if (game.ecranApresDeck == Game::Ecran::Jeu) {
                    game.demarrerPartie(game.prochaineMode);
                } else {
                    game.ecran = game.ecranApresDeck;
                }
                return;
            }
        }
        return;
    }

    if (game.ecran == Game::Ecran::OnlineWaiting) {
        return;
    }

    if (game.ecran == Game::Ecran::FinPartie) {
        if (game.btnFinMenu.getGlobalBounds().contains(pos)) {
            game.ecran = Game::Ecran::Menu;
        }
        return;
    }

    if (game.joueurCourant >= 1 && game.joueurCourant <= 2) {
        auto& jokers = game.players[game.joueurCourant - 1].jokers;
        auto estJokerImmediat = [](JokerAction action) {
            switch (action) {
                case JokerAction::Next:
                case JokerAction::Mix:
                case JokerAction::Vision:
                case JokerAction::Tirage:
                case JokerAction::Retry:
                case JokerAction::Swap:
                    return true;
                default:
                    return false;
            }
        };
        for (int i = 0; i < static_cast<int>(jokers.size()); ++i) {
            if (game.jokerButtonBounds(i).contains(pos)) {
                if (estJokerImmediat(jokers[i])) {
                    game.pendingJoker = JokerAction::None;
                    game.useJoker(jokers[i]);
                } else {
                    game.pendingJoker = (game.pendingJoker == jokers[i]) ? JokerAction::None : jokers[i];
                }
                return;
            }
        }
    }

    for (int i = 0; i < game.kPredictionCount; ++i) {
        if (game.btnPredictions[i].getGlobalBounds().contains(pos)) {
            game.togglePredictionSelection(i);
            game.predictionActive = PredictionEngine::predictionTypes()[i];
            if (game.joueurCourant >= 1 && game.joueurCourant <= 2) {
                game.players[game.joueurCourant - 1].chosenPredictionIndex = i;
            }
            return;
        }
    }

    if (game.peutPasserMainMaintenant() && game.btnPasserMain.getGlobalBounds().contains(pos)) {
        if (game.estModeMultijoueurOnline()) {
            if (game.joueurCourant != game.myPlayerNumber) {
                return;
            }
            if (game.onlineHost && game.myPlayerNumber == 1) {
                const int joueurQuiPasse = game.joueurCourant;
                game.passerMainVolontairement();
                game.sendPassExecution(joueurQuiPasse);
            } else if (!game.onlineHost && game.myPlayerNumber == 2) {
                game.sendPassRequest();
            }
        } else {
            game.passerMainVolontairement();
        }
        return;
    }

    if (!game.fin && game.btnTirer.getGlobalBounds().contains(pos)) {
        if (game.estModeMultijoueurOnline()) {
            if (game.joueurCourant != game.myPlayerNumber) {
                return;
            }
            int predictionIndex = 0;
            const auto& types = PredictionEngine::predictionTypes();
            for (size_t i = 0; i < types.size(); ++i) {
                if (types[i] == game.predictionActive) {
                    predictionIndex = static_cast<int>(i);
                    break;
                }
            }
            if (game.onlineHost && game.myPlayerNumber == 1) {
                int prevId = game.cartes[0] ? game.cartes[0]->getId() : 0;
                game.tirerDeuxCartes();
                game.sendExecuteMove(1, predictionIndex, prevId, game.derniereCarteId);
            } else if (!game.onlineHost && game.myPlayerNumber == 2) {
                game.sendMoveRequest(predictionIndex);
            }
        } else {
            game.tirerDeuxCartes();
        }
    }

    if (game.btnQuitter.getGlobalBounds().contains(pos)) {
        game.ecran = Game::Ecran::Menu;
    }
}

void InputController::handleTextEntered(const sf::Event::TextEntered& text) {
    if (game.ecran != Game::Ecran::MenuMultiOnline) {
        return;
    }

    if (game.selectionOnline == 0) {
        if ((text.unicode >= '0' && text.unicode <= '9') || text.unicode == '.') {
            game.onlineIp.push_back(static_cast<char>(text.unicode));
        }
    } else if (game.selectionOnline == 1) {
        if (text.unicode >= '0' && text.unicode <= '9') {
            game.onlinePort.push_back(static_cast<char>(text.unicode));
        }
    }
    game.txtOnlineIpLabel->setString("IP: " + game.onlineIp);
    game.txtOnlinePortLabel->setString("Port: " + game.onlinePort);
    centrerTexte(*game.txtOnlineIpLabel, game.btnOnlineIp);
    centrerTexte(*game.txtOnlinePortLabel, game.btnOnlinePort);
}
