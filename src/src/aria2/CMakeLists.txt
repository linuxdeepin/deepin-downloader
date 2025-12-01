# SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: GPL-3.0-or-later

include_directories(${PROJECT_SOURCE_DIR}/src/include/aria2
                    ${PROJECT_SOURCE_DIR}/src/include/log
                    ${CMAKE_BINARY_DIR}
    )
message(${CMAKE_BINARY_DIR})
message(aria2 123)
link_directories(${CMAKE_BINARY_DIR}/lib)
#SET_TARGET_PROPERTIES(dlmaria2 PROPERTIES VERSION PROJECT_VERSION SOVERSION 1)
#预定义宏，主要用于共享库导出
#ADD_DEFINITIONS(-DARIA_LIB )

#需要moc(加Q_OBJECT)的头文件
set (DWM_ARIA_HEADERS
    ${PROJECT_SOURCE_DIR}/src/include/aria2/aria2rpcinterface.h
)

#需要生成的moc文件，输出文件名称放在变量 mocfiles中，必须在find QT5 package才能调用
# 根据Qt版本选择合适的命令
if (QT_VERSION VERSION_GREATER_EQUAL "6.0")
    qt_wrap_cpp (DWM_ARIA_HEADERS_MOC ${DWM_ARIA_HEADERS})
else()
    qt5_wrap_cpp (DWM_ARIA_HEADERS_MOC ${DWM_ARIA_HEADERS})
endif()

#设置输出dll的名称、类型(动态库、静态库)及所用到的文件
add_library(dlmaria2 SHARED
    aria2rpcinterface.cpp
    aria2cbtinfo.cpp
    ${DWM_ARIA_HEADERS_MOC}
)

target_link_libraries(dlmaria2 dlmlog Qt${QT_VERSION_MAJOR}::Core)

#install(TARGETS dlmaria2 DESTINATION lib)

