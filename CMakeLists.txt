#cmake 最小版本要求
cmake_minimum_required(VERSION 3.1.0)


#工程名
project(downloadmanager)

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

if (NOT (${CMAKE_BUILD_TYPE} MATCHES "Debug"))
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")

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
set(QT Core Gui Widgets Network DBus Sql LinguistTools Svg)
find_package(Qt5 COMPONENTS ${QT} REQUIRED)


#应用DTK库
set(DTK Widget Core Gui )
find_package(Dtk COMPONENTS ${DTK} REQUIRED)


#加载子模块
add_subdirectory(src)
add_subdirectory(translations)

#安装
# qm files
file(GLOB QM_FILES "translations/*.qm")
install(FILES ${QM_FILES} DESTINATION share/downloadmanager/translations)

#desktop
install(FILES desktop/downloadmanager.desktop DESTINATION share/applications)
#install(FILES desktop/downloadmanager.desktop DESTINATION share/downloadmanager/desktop)

#datebases
install(FILES data/uos-downloadmanager-task.db DESTINATION share/downloadmanager/database)

# conf file
install(FILES docs/aria2.conf DESTINATION share/downloadmanager/config)
install(FILES docs/config.conf DESTINATION share/downloadmanager/config)

#aria2c
install(FILES /usr/bin/aria2c DESTINATION share/downloadmanager)

#icon
install(FILES src/src/ui/images/icon/downloader4.svg DESTINATION share/icons/hicolor/scalable/apps/)
install(FILES src/src/ui/images/icon/downloader4.svg DESTINATION share/downloadmanager/icons/logo/)


install(TARGETS downloadmanager log database aria2 ui
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib
ARCHIVE DESTINATION libstatic
)

