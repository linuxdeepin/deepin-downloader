#cmake 最小版本要求
cmake_minimum_required(VERSION 3.1.0)


#工程名
project(downloader)
set(PROJECT_VERSION "5.2.0.18")
set(PROJECT_VERSION_MAJOR 5)
set(QT_MIN_VERSION "5.11.0")
set(DLM_NAME "downloader")
set(ARIA2_NAME "aria2c")

# 设置C++标准为 C++ 11
set(CMAKE_CXX_STANDARD 11)

#在debug下库名后面加d
set(CMAKE_DEBUG_POSTFIX "d")

#设置Debug  Release
set(CMAKE_BUILD_TYPE "Release")

#设置安装路径
set(CMAKE_INSTALL_PREFIX /usr)
## Install settings
#if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
#    set(CMAKE_INSTALL_PREFIX /usr)
#endif ()

# 是否开启单元测试编译
option(DOTEST "option for test" ON)

if (NOT (${CMAKE_BUILD_TYPE} MATCHES "Debug"))
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")
    message("release")
    # generate qm
    execute_process(COMMAND bash "translate_generation.sh"
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif ()

#库 和 执行文件 的设置输出的路径
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/lib)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/lib)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin)

#
configure_file(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h )

message(STATUS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

#包含当前路径，使用*.ui文件时，需要加上这句，否则找不到头文件
set(CMAKE_INCLUDE_CURRENT_DIR ON)
#打开全局moc
set(CMAKE_AUTOMOC ON)
#打开全局rcc，本示例中没有使用qrc
set(CMAKE_AUTORCC ON)
#打开全局uic
set(CMAKE_AUTOUIC ON)

# 开启单元测试
if(DOTEST)
    # 添加gocv覆盖率文件的输出
    set(CMAKE_CXX_FLAGS "-g -fprofile-arcs -ftest-coverage")
    # 设置Qt模块
    set(QT Core Gui Widgets Network DBus Sql LinguistTools Svg Multimedia Test)
    # 查找Qt相关的库链接
    find_package(Qt5 COMPONENTS ${QT} REQUIRED)

# 未开启单元测试
else()
    # 设置Qt模块
    set(QT Core Gui Widgets Network DBus Sql LinguistTools Svg Multimedia)
    # 查找Qt相关的库链接
    find_package(Qt5 COMPONENTS ${QT} REQUIRED)
endif()


#---------------------------------- 单元测试配置 -----------------------------------#
if(DOTEST)
# 设置测试程序名字
set(PROJECT_NAME_TEST
    ${PROJECT_NAME}_test)

# 设置测试程序的包含目录
#include_directories(./libs/googletest)
#include_directories(./libs/googletest/googletest-src/googlemock/include)
#include_directories(./libs/googletest/googletest-src/googletest/include)
#add_subdirectory(./libs/googletest)

#LINK_DIRECTORIES(/usr/lib/gtest)

include(cmake/googletest.cmake)
fetch_googletest(
    ${PROJECT_SOURCE_DIR}/cmake
    ${PROJECT_BINARY_DIR}/googletest
    )

#ADD_EXECUTABLE(${PROJECT_NAME_TEST} ${SRC_LIST})
#TARGET_LINK_LIBRARIES(${PROJECT_NAME} libgtest_dll.so)

# 定义测试用例文件
FILE (GLOB allHeaders
    "./src/aria2/*.h"
    "./src/database/*.h"
    "./src/downloader/*.h"
    "./src/log/*.h"
    "./src/ui/*.h"
    "./src/ui/createTask/*.h"
    "./src/ui/mainFrame/*.h"
    "./src/ui/settings/*.h"
    )

# 定义测试用例文件
FILE (GLOB allSource
    "./src/aria2/*.cpp"
    "./src/database/*.cpp"
    "./src/downloader/*.cpp"
    "./src/log/*.cpp"
    "./src/ui/*.cpp"
    "./src/ui/createTask/*.cpp"
    "./src/ui/mainFrame/*.cpp"
    "./src/ui/settings/*.cpp"
    )

FILE (GLOB allTestSource
    "./test/aria2/*.cpp"
    "./test/database/*.cpp"
    "./test/downloader/*.cpp"
    "./test/log/*.cpp"
    "./test/ui/*.cpp"
    "./test/ui/createTask/*.cpp"
    "./test/ui/mainFrame/*.cpp"
    "./test/ui/settings/*.cpp"
    )
## 定义测试资源文件
#set(AllQRC
#    ./src/ui/resource/resource.qrc)

add_executable(${PROJECT_NAME_TEST} ${allHeaders} ${allSource} ${allTestSource})

target_link_libraries(${PROJECT_NAME_TEST} gmock gmock_main gtest gtest_main)

# 添加 QTest 测试
add_test(${PROJECT_NAME_TEST} COMMAND ${PROJECT_NAME_TEST})


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

endif()

#应用DTK库
set(DTK Widget Core Gui Wm)
find_package(Dtk COMPONENTS ${DTK} REQUIRED)

#加载子模块
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(translations)

#安装
# qm files
file(GLOB QM_FILES "translations/*.qm")
install(FILES ${QM_FILES} DESTINATION share/downloader/translations)

#desktop
install(FILES desktop/downloader.desktop DESTINATION share/applications)

#datebases
install(FILES data/downloader.db DESTINATION share/downloader/database)

# conf file
file(GLOB CF_FILES "docs/*.conf")
install(FILES ${CF_FILES} DESTINATION share/downloader/config)


#icon
install(FILES data/downloader.svg DESTINATION share/icons/hicolor/scalable/apps/)
install(FILES data/downloader.svg DESTINATION share/downloader/icons/logo/)

install(TARGETS downloader dlmlog dlmdatabase dlmaria2 dlmcontrolui
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib
ARCHIVE DESTINATION libstatic
)

