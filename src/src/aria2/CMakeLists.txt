include_directories(${PROJECT_SOURCE_DIR}/src/include/aria2
                    ${PROJECT_SOURCE_DIR}/src/include/log
    )
link_directories(${CMAKE_BINARY_DIR}/lib)

#预定义宏，主要用于dll导出
ADD_DEFINITIONS(-DARIA_LIB )

#需要moc(加Q_OBJECT)的头文件
set (DWM_ARIA_HEADERS
    ${PROJECT_SOURCE_DIR}/src/include/aria2/aria2rpcinterface.h
)

#需要生成的moc文件，输出文件名称放在变量 mocfiles中，必须在find QT5 package才能调用
QT5_WRAP_CPP (DWM_ARIA_HEADERS_MOC ${DWM_ARIA_HEADERS})


#设置输出dll的名称、类型(动态库、静态库)及所用到的文件
add_library(aria2 SHARED
    aria2rpcinterface.cpp
    ${DWM_ARIA_HEADERS_MOC}
)

#依赖库
if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    target_link_libraries(aria2 liblogd.so Qt5::Core Qt5::Widgets libdtkcore.so libdtkgui.so libdtkwidget.so libdtkwm.so)
else()
    target_link_libraries(aria2 liblog.so Qt5::Core Qt5::Widgets libdtkcore.so libdtkgui.so libdtkwidget.so libdtkwm.so)
endif()
