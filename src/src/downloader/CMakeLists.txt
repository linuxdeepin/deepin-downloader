# SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: GPL-3.0-or-later

#设置包含目录
include_directories(${PROJECT_SOURCE_DIR}/src/include/ui
                    ${PROJECT_SOURCE_DIR}/src/include/ui/mainFrame
                    ${PROJECT_SOURCE_DIR}/src/include/ui/settings
                    ${PROJECT_SOURCE_DIR}/src/include/ui/websocket
                    ${PROJECT_SOURCE_DIR}/src/include/log
                    ${PROJECT_SOURCE_DIR}/src/include/downloader
                    ${CMAKE_BINARY_DIR}
)
link_directories(${CMAKE_BINARY_DIR}/lib)

#需要moc(加Q_OBJECT)的头文件
set (DWM_EXTENSIONSERVICE_HEADERS
    ${PROJECT_SOURCE_DIR}/src/include/downloader/accessiblewidget.h
)

#需要生成的moc文件，输出文件名称放在变量 mocfiles中，必须在find QT5 package才能调用
# 根据Qt版本选择合适的命令
if (QT_VERSION VERSION_GREATER_EQUAL "6.0")
    qt_wrap_cpp (DWM_EXTENSIONSERVICE_HEADERS_MOC ${DWM_EXTENSIONSERVICE_HEADERS})
else()
    qt5_wrap_cpp (DWM_EXTENSIONSERVICE_HEADERS_MOC ${DWM_EXTENSIONSERVICE_HEADERS})
endif()

#设置输入执行文件名称
add_executable(downloader main.cpp
    accessiblewidget.cpp
    ${DWM_EXTENSIONSERVICE_HEADERS_MOC}
)

target_link_libraries(downloader dlmcontrolui dlmlog Qt${QT_VERSION_MAJOR}::Core)

# translation files
TRANSLATION_GENERATE(QM_FILES ${CMAKE_SOURCE_DIR}/translations)
add_custom_target(${PROJECT_NAME}_qm_files DEPENDS ${QM_FILES})
add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_qm_files)

# Install translations
install(FILES ${QM_FILES} DESTINATION share/${PROJECT_NAME}/translations)

#INSTALL(TARGETS downloader DESTINATION share/downloader)
#install(TARGETS downloader DESTINATION bin)

