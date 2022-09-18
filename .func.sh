#!/bin/sh

build() {
  [ ! "$(basename "$PWD")" = "$PROJECT_NAME" ] && return 1
  rm -rf ./build && mkdir ./build || return 1
  cmake \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -S . \
    -B ./build \
    -G Ninja \
    && ninja \
    -C ./build
}

run() {
  [ ! "$(basename "$PWD")" = "$PROJECT_NAME" ] && return 1
  python3 gen_scene.py
  build
  kitty --config=./kitty.conf --start-as=maximized sh -c "./build/$PROJECT_NAME"
  # ./build/"$PROJECT_NAME" && echo "$?"
}
