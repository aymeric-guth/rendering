#!/bin/sh

[ -f "$DOTFILES/funcs/project_c.sh" ] && . "$DOTFILES/funcs/project_c.sh"

clean() {
  # c_project_clean || return 1
  c_clean || return 1
}

build() {
  c_clean || return 1
  c_build_cmake || return 1
}

gen() {
  c_tools_gen || return 1
}

run() {
  c_project_run || return 1
  # OBJ_FILE="cube.obj" python3 obj_loader.py || return 1
  OBJ_FILE="teapot2.obj" python3 obj_loader.py || return 1
  build || return 1
  # GPU accelerated Panel, only for X11
  # kitty --config=./kitty.conf --start-as=maximized +kitten panel sh -c "./build/$PROJECT_NAME"
  # kitty --config=./kitty.conf --start-as=maximized sh -c "./a.out"
  kitty --config=./kitty.conf --start-as=maximized sh -c "./build/$PROJECT_NAME" || return 1
  # ./build/"$PROJECT_NAME" && echo "$?"
}
