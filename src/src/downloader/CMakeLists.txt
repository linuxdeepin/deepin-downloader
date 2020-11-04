#设置包含目录
include_directories(${PROJECT_SOURCE_DIR}/src/include/ui
                    ${PROJECT_SOURCE_DIR}/src/include/ui/mainFrame
                    ${PROJECT_SOURCE_DIR}/src/include/ui/settings
                    ${PROJECT_SOURCE_DIR}/src/include/log
                    ${CMAKE_BINARY_DIR}
    )
link_directories(${CMAKE_BINARY_DIR}/lib)


#设置输入执行文件名称
add_executable(downloader main.cpp)

target_link_libraries(downloader dlmcontrolui dlmlog Qt5::Core)

#INSTALL(TARGETS downloader DESTINATION share/downloader)
#install(TARGETS downloader DESTINATION bin)

