cmake_minimum_required(VERSION 3.1-3.8)
project(text_editor)
set(C_STANDARD 11)

find_package(Curses REQUIRED)
include(CTest)
enable_testing()

add_subdirectory(tests)
add_subdirectory(src)
