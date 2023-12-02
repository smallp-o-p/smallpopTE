include_directories(${CMAKE_SOURCE_DIR}/src/includes)
add_executable(featureTest ./featureTests.c)
link_libraries(row)

target_link_directories(featureTest PUBLIC "${CMAKE_SOURCE_DIR}/src/includes")

add_test(featureTesting featureTest)
