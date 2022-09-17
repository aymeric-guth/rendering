cmake_minimum_required(VERSION 3.10.2)
project(playground)
enable_language(C)

# add_compile_options(-W -Wall -Werror)
add_executable(${PROJECT_NAME} main.c fifo.c matrix.c)
add_executable(kbfind kbfind.c fifo.c)
target_link_libraries(${PROJECT_NAME} PUBLIC m)
target_link_libraries(${PROJECT_NAME} PUBLIC pthread)

target_link_libraries(kbfind PUBLIC termcap)
target_link_libraries(kbfind PUBLIC m)
target_link_libraries(kbfind PUBLIC pthread)
target_link_libraries(kbfind PUBLIC ncurses)
