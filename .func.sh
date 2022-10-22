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
  # OBJ_FILE="cube.obj" python3 obj_loader.py || return 1
  OBJ_FILE="teapot2.obj" python3 obj_loader.py || return 1
  build || return 1
  # GPU accelerated Panel, only for X11
  # kitty --config=./kitty.conf --start-as=maximized +kitten panel sh -c "./build/$PROJECT_NAME"
  # kitty --config=./kitty.conf --start-as=maximized sh -c "./a.out"
  kitty --config=./kitty.conf --start-as=maximized sh -c "./build/$PROJECT_NAME" || return 1
  # ./build/"$PROJECT_NAME" && echo "$?"
}
