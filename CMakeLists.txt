cmake_minimum_required(VERSION 3.1-3.8)
project(text_editor)
set(C_STANDARD 11)
add_executable(main 
        ${CMAKE_SOURCE_DIR}/src/main.c 
        ${CMAKE_SOURCE_DIR}/src/terminal.c 
        ${CMAKE_SOURCE_DIR}/src/error.c
        ${CMAKE_SOURCE_DIR}/src/io/input.c
        ${CMAKE_SOURCE_DIR}/src/io/output.c
        ${CMAKE_SOURCE_DIR}/src/io/file.c
        )
target_include_directories(main PUBLIC "${CMAKE_SOURCE_DIR}/includes")


