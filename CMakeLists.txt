cmake_minimum_required(VERSION 3.1-3.8)
project(text_editor)
set(C_STANDARD 11)

find_package(Curses REQUIRED) 
include_directories(${CURSES_INCLUDE_DIR})

add_executable(main 
        ${CMAKE_SOURCE_DIR}/src/main.c 
        ${CMAKE_SOURCE_DIR}/src/terminal.c 
        ${CMAKE_SOURCE_DIR}/src/error.c
        ${CMAKE_SOURCE_DIR}/src/io/input.c
        ${CMAKE_SOURCE_DIR}/src/io/output.c
        ${CMAKE_SOURCE_DIR}/src/io/file.c
        ${CMAKE_SOURCE_DIR}/src/row.c
        ${CMAKE_SOURCE_DIR}/src/editorFeatures.c
        ${CMAKE_SOURCE_DIR}/src/shortcuts.c
        ${CMAKE_SOURCE_DIR}/src/data_structures/stack.c
        )

target_include_directories(main PUBLIC "${CMAKE_SOURCE_DIR}/includes")
target_link_libraries(main ${CURSES_LIBRARIES})


