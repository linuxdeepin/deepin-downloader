include_directories(${PROJECT_SOURCE_DIR}/src/include/aria2)


#设置输出dll的名称、类型(动态库、静态库)及所用到的文件
add_library(aria2 SHARED
)

#依赖库
if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    target_link_libraries(aria2 Qt5::Core Qt5::Widgets libdtkcore.so libdtkgui.so libdtkwidget.so libdtkwm.so)
    message("ui debug")
else()
    target_link_libraries(aria2 Qt5::Core Qt5::Widgets libdtkcore.so libdtkgui.so libdtkwidget.so libdtkwm.so)
    message("ui release")
endif()
