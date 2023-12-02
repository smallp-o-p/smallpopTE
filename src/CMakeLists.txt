include_directories(${CURSES_INCLUDE_DIR})
include_directories("${CMAKE_SOURCE_DIR}/src/includes" "${CMAKE_SOURCE_DIR}/src")
add_executable(smallpop 
        main.c 
        error.c
        editorFeatures.c
        row.c
        shortcuts.c
        terminal.c 
        ./io/input.c
        ./io/output.c
        ./io/file.c
        ./data_structures/stack.c
        )

target_include_directories(smallpop PUBLIC "${CMAKE_SOURCE_DIR}/src/includes")

add_library(row SHARED "${CMAKE_SOURCE_DIR}/src/row.c")
target_link_libraries(smallpop ${CURSES_LIBRARIES})
