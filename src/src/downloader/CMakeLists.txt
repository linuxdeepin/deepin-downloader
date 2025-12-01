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
    ${PROJECT_SOURCE_DIR}/src/include/downlaoder/accessiblewidget.h
)

#设置输入执行文件名称
add_executable(downloader main.cpp
    accessiblewidget.cpp
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

