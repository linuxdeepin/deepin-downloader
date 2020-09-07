#设置包含目录
include_directories(${PROJECT_SOURCE_DIR}/src/include/ui
                    ${PROJECT_SOURCE_DIR}/src/include/log
                    ${CMAKE_BINARY_DIR}
    )
link_directories(${CMAKE_BINARY_DIR}/lib)


#设置输入执行文件名称
add_executable(Downloader main.cpp)

target_link_libraries(Downloader dlmcontrolui dlmlog Qt5::Core)

INSTALL(TARGETS Downloader DESTINATION share/Downloader)
install(TARGETS Downloader DESTINATION bin)

