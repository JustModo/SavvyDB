cmake_minimum_required(VERSION 3.10)

project(SavvyDB C)

set(CMAKE_C_STANDARD 99)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

include_directories(${CMAKE_SOURCE_DIR}/includes)

add_executable(savvy src/main.c src/menus.c)

target_link_libraries(savvy ncursesw)