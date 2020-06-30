#设置包含目录
include_directories(${PROJECT_SOURCE_DIR}/src/include/ui
                    ${PROJECT_SOURCE_DIR}/src/include/log
    )
link_directories(${CMAKE_BINARY_DIR}/lib)


#设置输入执行文件名称
add_executable(downloadmanager main.cpp)

target_link_libraries(downloadmanager dlmcontrolui dlmlog Qt5::Core)

INSTALL(TARGETS downloadmanager DESTINATION share/downloadmanager)
install(TARGETS downloadmanager DESTINATION bin)

