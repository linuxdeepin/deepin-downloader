add_executable(dlmdatabaseTest database.cpp dbinstance.cpp)

target_link_libraries(dlmdatabaseTest gtest gtest_main)

target_link_libraries(dlmdatabaseTest dlmdatabase)

add_test(NAME that-test-I-made COMMAND dlmdatabaseTest)
