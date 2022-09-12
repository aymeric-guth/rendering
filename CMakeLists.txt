cmake_minimum_required(VERSION 3.10.2)
project(playground)

add_compile_options(-W -Wall -Werror)
# add_executable(${PROJECT_NAME} renderer.c)
add_executable(${PROJECT_NAME} renderervec.c)
