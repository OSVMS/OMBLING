#!/usr/bin/env bash
set -euo pipefail

if ! command -v cmake >/dev/null 2>&1; then
  echo "CMake est introuvable. Installe-le puis reessaie."
  exit 1
fi

# Permet de surcharger SFML_DIR via variable d'environnement.
if [[ -z "${SFML_DIR:-}" ]]; then
  if [[ -d "/opt/homebrew/opt/sfml/lib/cmake/SFML" ]]; then
    SFML_DIR="/opt/homebrew/opt/sfml/lib/cmake/SFML"
  elif [[ -d "/usr/local/opt/sfml/lib/cmake/SFML" ]]; then
    SFML_DIR="/usr/local/opt/sfml/lib/cmake/SFML"
  else
    SFML_DIR=""
  fi
fi

cmake_args=(
  -S .
  -B build_macos
  -DCMAKE_BUILD_TYPE=Release
)

if [[ -n "$SFML_DIR" ]]; then
  cmake_args+=("-DSFML_DIR=$SFML_DIR")
fi

cmake "${cmake_args[@]}"
cmake --build build_macos --config Release

ln -sf build_macos/app app

echo "Build termine. Binaires: build_macos/app (et lanceur ./app)"