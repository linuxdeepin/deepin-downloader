include_directories(${PROJECT_SOURCE_DIR}/src/include/database)
add_executable(dlmdatabaseTest database.cpp dbinstance.cpp)

target_link_libraries(dlmdatabaseTest gtest gtest_main dlmdatabase)

add_test(NAME that-test-I-made COMMAND dlmdatabaseTest)
