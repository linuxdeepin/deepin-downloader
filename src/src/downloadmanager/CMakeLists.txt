#设置包含目录
include_directories(${PROJECT_SOURCE_DIR}/src/include/ui
                    ${PROJECT_SOURCE_DIR}/src/include/log
    )
link_directories(${CMAKE_BINARY_DIR}/lib)


#设置输入执行文件名称
add_executable(downloadmanager main.cpp)

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    target_link_libraries(downloadmanager libuid.so liblogd.so Qt5::Core Qt5::Widgets libdtkcore.so libdtkgui.so libdtkwidget.so libdtkwm.so)
else()
    target_link_libraries(downloadmanager libui.so liblog.so Qt5::Core Qt5::Widgets libdtkcore.so libdtkgui.so libdtkwidget.so libdtkwm.so)
endif()

INSTALL(TARGETS downloadmanager
RUNTIME DESTINATION share/downloadmanager
)

