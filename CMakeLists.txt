cmake_minimum_required(VERSION 3.15)
project(minesweeper)
set(CURSES_LIBRARY "C:\\MinGW\\lib\\libncursesw.a")
set(CURSES_INCLUDE_PATH "C:\\MinGW\\lib\\ncursesw")
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

set(CMAKE_CXX_STANDARD 11)

add_executable(minesweeper main.cpp)
target_link_libraries(minesweeper PUBLIC ncursesw)
