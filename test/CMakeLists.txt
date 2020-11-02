# 设置测试程序名字
set(PROJECT_NAME_TEST
    ${PROJECT_NAME}_test)

include(../cmake/googletest.cmake)
fetch_googletest(
    ${PROJECT_SOURCE_DIR}/cmake
    ${PROJECT_BINARY_DIR}/googletest
    )

#src files
AUX_SOURCE_DIRECTORY(. SRC_LIST)
AUX_SOURCE_DIRECTORY(./ MAIN)
LIST(APPEND SRC_LIST ${MAIN})
AUX_SOURCE_DIRECTORY(./aria2 ARIA2)
LIST(APPEND SRC_LIST ${ARIA2})
AUX_SOURCE_DIRECTORY(./database DATABASE)
LIST(APPEND SRC_LIST ${DATABASE})
AUX_SOURCE_DIRECTORY(./log LOG)
LIST(APPEND SRC_LIST ${LOG})
AUX_SOURCE_DIRECTORY(./ui UI)
LIST(APPEND SRC_LIST ${UI})
AUX_SOURCE_DIRECTORY(./ui UI)
LIST(APPEND SRC_LIST ${UI})
AUX_SOURCE_DIRECTORY(./ui/createTask CREATETASK)
LIST(APPEND SRC_LIST ${CREATETASK})
AUX_SOURCE_DIRECTORY(./ui/mainFrame MAINFRAME)
LIST(APPEND SRC_LIST ${MAINFRAME})
AUX_SOURCE_DIRECTORY(./ui/settings SETTINGS)
LIST(APPEND SRC_LIST ${SETTINGS})

#aux_source_directory(${PROJECT_SOURCE_DIR}/src/src/database ALLSRC)

add_executable(${PROJECT_NAME_TEST} ${SRC_LIST})

target_link_libraries(${PROJECT_NAME_TEST} gmock gmock_main gtest gtest_main  Qt5::Core dlmdatabase dlmaria2 dlmcontrolui dlmlog)

# 添加 QTest 测试
add_test(${PROJECT_NAME_TEST} that-test-I-made COMMAND ${PROJECT_NAME_TEST})


#------------------------------ 创建'make test'指令---------------------------------------
add_custom_target(test
    COMMAND mkdir -p coverageResult
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

add_custom_command(TARGET test
    COMMAND echo " =================== CREAT LCOV REPROT BEGIN ==================== "

    #1.清理原先的gcov信息
    COMMAND lcov --directory ./CMakeFiles/${PROJECT_NAME_TEST}.dir --zerocounters
    COMMAND lcov --directory ./CMakeFiles/${PROJECT_NAME}.dir --zerocounters
    COMMAND ./${PROJECT_NAME_TEST}

    #2.收集gcov信息到.info文件中
    COMMAND lcov --directory . --capture --output-file ./coverageResult/${PROJECT_NAME}_Coverage.info

    #3.过滤一些我们不感兴趣的文件的覆盖率信息
    COMMAND echo " =================== do filter begin ==================== "
    COMMAND lcov --remove ./coverageResult/${PROJECT_NAME}_Coverage.info
    '*/${PROJECT_NAME_TEST}_autogen/*' '*/${PROJECT_NAME}_autogen/*' '*/usr/include/*' '*/tests/*' '*/googletest/*'
    -o ./coverageResult/${PROJECT_NAME}_Coverage.info
    COMMAND echo " =================== do filter end ==================== "

    #3.将.info信息生成报告到reprot文件夹中
    COMMAND genhtml -o ./coverageResult/report ./coverageResult/${PROJECT_NAME}_Coverage.info

    COMMAND echo " =================== CREAT LCOV REPROT END ==================== "

    COMMAND echo " Coverage files have been output to ${CMAKE_BINARY_DIR}/coverageResult/report "

    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )

#'make test'命令依赖与我们的测试程序
add_dependencies(test ${PROJECT_NAME_TEST})

include_directories(${PROJECT_SOURCE_DIR}/src/include/database)
include_directories(${PROJECT_SOURCE_DIR}/src/include/aria2)
include_directories(${PROJECT_SOURCE_DIR}/src/include/ui)
include_directories(${PROJECT_SOURCE_DIR}/src/include/log)



#target_link_libraries(Qt5::Core)


