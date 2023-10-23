cmake_minimum_required(VERSION 3.1-3.8)
project(text_editor)

add_executable(main "./src/main.c")
target_include_directories(main PUBLIC "./includes/*.h")


