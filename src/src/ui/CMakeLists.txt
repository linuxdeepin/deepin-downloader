#设置包含目录
include_directories( ${PROJECT_SOURCE_DIR}/src/include/ui)

#预定义宏，主要用于dll导出
ADD_DEFINITIONS(-DUI_LIB )

#需要moc(加Q_OBJECT)的头文件 
set (UDLC_UI_HEADERS
    ${PROJECT_SOURCE_DIR}/src/include/ui/mainframe.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/tableView.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/tableModel.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/topButton.h
)

#需要生成的moc文件，输出文件名称放在变量 mocfiles中，必须在find QT5 package才能调用
QT5_WRAP_CPP (UDLC_UI_HEADERS_MOC ${UDLC_UI_HEADERS})


#设置输出dll的名称、类型(动态库、静态库)及所用到的文件
add_library(ui SHARED
       mainframe.cpp
       tableView.cpp
       tableModel.cpp
       topButton.cpp
       ${UDLC_UI_HEADERS_MOC}
       ${qrc_files}
)

#依赖库
if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    target_link_libraries(ui Qt5::Core Qt5::Widgets libdtkcore.so libdtkgui.so libdtkwidget.so libdtkwm.so)
    message("ui debug")
else()
    target_link_libraries(ui Qt5::Core Qt5::Widgets libdtkcore.so libdtkgui.so libdtkwidget.so libdtkwm.so)
    message("ui release")
endif()



