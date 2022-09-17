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
  build
  ./build/"$PROJECT_NAME" && echo "$?"
}
