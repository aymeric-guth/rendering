cmake_minimum_required(VERSION 3.10.2)
project(playground)
enable_language(C)

add_compile_options(-W -Wall -Werror)
add_executable(${PROJECT_NAME} renderervec.c)
target_link_libraries(${PROJECT_NAME} PUBLIC m)
target_link_libraries(${PROJECT_NAME} PUBLIC pthread)
target_link_libraries(${PROJECT_NAME} PUBLIC ncurses)
