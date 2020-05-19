#cmake 最小版本要求
cmake_minimum_required(VERSION 3.1.0)

#工程名
project(downloadmanager)

# 设置C++标准为 C++ 11
set(CMAKE_CXX_STANDARD 11)

#在debug下库名后面加d
set(CMAKE_DEBUG_POSTFIX "d")

#设置Debug  Release
set(CMAKE_BUILD_TYPE "Debug")

#库 和 执行文件 的设置输出的路径
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

message(STATUS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

#包含当前路径，使用*.ui文件时，需要加上这句，否则找不到头文件
set(CMAKE_INCLUDE_CURRENT_DIR ON)
#打开全局moc
set(CMAKE_AUTOMOC ON)
#打开全局rcc，本示例中没有使用qrc
set(CMAKE_AUTORCC ON)
#打开全局uic
set(CMAKE_AUTOUIC ON)


#应用qt库
set(QT Core Gui Widgets Network DBus Sql)
find_package(Qt5 COMPONENTS ${QT} REQUIRED)

#应用DTK库
set(DTK Widget Core Gui Wm)
find_package(Dtk COMPONENTS ${DTK} REQUIRED)


#加载子模块
add_subdirectory(src)
