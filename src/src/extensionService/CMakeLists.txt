# SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: GPL-3.0-or-later

include_directories(${PROJECT_SOURCE_DIR}/src/include/extensionService
                    ${PROJECT_SOURCE_DIR}/src/include/ui/mainFrame)
link_directories(${CMAKE_BINARY_DIR}/lib)

#预定义宏，主要用于dll导出
ADD_DEFINITIONS(-DLM_EXTENSIONSERVICE_LIB )

#需要moc(加Q_OBJECT)的头文件
set (DWM_EXTENSIONSERVICE_HEADERS
    ${PROJECT_SOURCE_DIR}/src/include/extensionService/websocketclientwrapper.h
    ${PROJECT_SOURCE_DIR}/src/include/extensionService/websockethandle.h
    ${PROJECT_SOURCE_DIR}/src/include/extensionService/websockettransport.h
    ${PROJECT_SOURCE_DIR}/src/include/extensionService/extensionservice.h
)

#需要生成的moc文件，输出文件名称放在变量 mocfiles中，必须在find QT5 package才能调用
qt_wrap_cpp (DWM_EXTENSIONSERVICE_HEADERS_MOC ${DWM_EXTENSIONSERVICE_HEADERS})

#设置输出dll的名称、类型(动态库、静态库)及所用到的文件
add_executable(dlmextensionservice
    websocketclientwrapper.cpp
    websockettransport.cpp
    websockethandle.cpp
    extensionservice.cpp
    main.cpp
    ${DWM_EXTENSIONSERVICE_HEADERS_MOC}
)

target_link_libraries(dlmextensionservice
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::WebChannel
    Qt${QT_VERSION_MAJOR}::WebSockets
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::DBus
    Dtk${DTK_VERSION}::Core
    Dtk${DTK_VERSION}::Gui
    Dtk${DTK_VERSION}::Widget
)

install(TARGETS dlmextensionservice DESTINATION bin)
