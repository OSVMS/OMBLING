## Prérequis
- CMake (>= 3.10 recommandé)
- SFML (si le projet dépend de SFML)

Sur Debian/Ubuntu :
```bash
sudo apt update
sudo apt install build-essential cmake libsfml-dev
```

Sur macOS (avec Homebrew) :
```bash
brew install cmake sfml
```

Sur Windows : installer CMake, avec le workload C++.

## Compilation Linux
- Assurer que `cmake` est installé.
- Depuis la racine du projet :
```bash
chmod +x build_linux.sh
./build_linux.sh
```
Le binaire sera placé dans `build_linux/app` ou un lien `app` sera créé à la racine.

commandes manuelles :
```bash
mkdir -p build_linux
cmake -S . -B build_linux -DCMAKE_BUILD_TYPE=Release
cmake --build build_linux --parallel
```


## Compilation macOS
Un helper existe : [build_macos.sh](build_macos.sh)
```bash
chmod +x build_macos.sh
./build_macos.sh
```
Le script cherche automatiquement SFML via Homebrew et crée `build_macos/app` puis un lien `app`.

## Compilation Windows
Un script helper existe : [build_windows.bat](build_windows.bat)
Exemple (PowerShell) :
```powershell
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DSFML_DIR="D:\libs\SFML-3.1.0\lib\cmake\SFML"
cmake --build build --config Release
```
Le binaire attendu est `build\Release\app.exe` ou `build\app.exe` selon la configuration.

## Exécution
Après compilation, lancer le binaire généré (`./app` sur Unix, `app.exe` sur Windows). Selon la configuration et les assets, veiller à exécuter depuis la racine du projet ou à fournir le chemin vers le dossier `asset/`.

## Structure du projet (vue générale)
- `src/main.cpp` — point d'entrée du programme.
- `src/core/` — logique du jeu : modes de jeu (`game_mode_solo.cpp`, `game_mode_multi.cpp`, `game_mode_test.cpp`), résolution des tours, moteur de prédiction.
- `src/ai/` — implémentations des IA (`ai_easy`, `ai_medium`, `ai_hard`, `ai_strategy`).
- `src/model/` — modèles de données (cartes, structures de jeu).
- `src/jokers/` — effets spéciaux / jokers.
- `src/ui/` — contrôleurs d'entrée et rendu (`input_controller`, `render_controller`, `ui_helpers`).

## Description des fonctions principales
- Initialisation et boucle principale : `main.cpp` initialise les systèmes (fenêtre, ressources) et lance la boucle du jeu.
- `Game` (`src/core/game.cpp` / `game.h`) : orchestre l'état du jeu, transitions de tours, et communication entre les modules (IA, UI, modèle).
- `Game State` (`game_state.h`) : structure contenant l'état courant (joueurs, mains, pioche, scores).
- `Turn Resolver` (`turn_resolver.cpp`) : logique de résolution des actions d'un tour (application des jokers, calcul des effets).
- `Prediction Engine` (`prediction_engine.cpp`) : module qui évalue et prédit issues possibles pour aider l'IA ou l'affichage (probabilités, meilleures actions).
- IA (`ai/*`) : plusieurs niveaux d'IA ; `ai_strategy` contient comportements réutilisables, `ai_easy/medium/hard` implémentent des heuristiques ou simulations.
- `Joker Effects` (`jokers/joker_effect.cpp`) : encapsule les effets spéciaux et leur application sur l'état du jeu.
- UI (`ui/*`) : gestion des entrées et rendu via SFML (ou abstrait si SFML absent). Les helpers contiennent utilitaires graphiques et de mise en page.

## Dépannage
- Assets manquants : assurer que le dossier `asset/` est présent à la racine avec les sous-dossiers `carte/`, `fond/`, etc.

