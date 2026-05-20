#!/usr/bin/env bash
set -euo pipefail

if ! command -v cmake >/dev/null 2>&1; then
  echo "CMake est introuvable. Installez CMake puis reessayez."
  exit 1
fi

# Permet de surcharger SFML_DIR via variable d'environnement.
if [[ -z "${SFML_DIR:-}" ]]; then
  SFML_DIR=""
fi

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$ROOT_DIR/build_linux"

cmake_args=(
  -S "$ROOT_DIR"
  -B "$BUILD_DIR"
  -DCMAKE_BUILD_TYPE=Release
)

if [[ -n "$SFML_DIR" ]]; then
  cmake_args+=("-DSFML_DIR=$SFML_DIR")
fi

cmake "${cmake_args[@]}"
cmake --build "$BUILD_DIR" --parallel

# Create a symlink named 'app' to the built binary if available
if [[ -f "$BUILD_DIR/app" ]]; then
  ln -sf "$BUILD_DIR/app" "$ROOT_DIR/app"
  echo "Build termine. Binaires: $BUILD_DIR/app (lanceur ./app)"
else
  echo "Build termine. Verifiez le dossier: $BUILD_DIR"
fi
