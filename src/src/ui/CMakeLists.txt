#设置包含目录
include_directories( ${PROJECT_SOURCE_DIR}/src/include/ui
                     ${PROJECT_SOURCE_DIR}/src/include/aria2
                     ${PROJECT_SOURCE_DIR}/src/include/log
                     ${CMAKE_BINARY_DIR}
)

link_directories(${CMAKE_BINARY_DIR}/lib)

#预定义宏，主要用于dll导出
ADD_DEFINITIONS(-DCONTROLUI_LIB )


#需要moc(加Q_OBJECT)的头文件 
set (UDLC_UI_HEADERS
    ${PROJECT_SOURCE_DIR}/src/include/ui/mainframe.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/tableView.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/tableModel.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/headerView.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/itemDelegate.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/global.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/topButton.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/createtaskwidget.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/btinfodelegate.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/btinfodialog.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/btinfotableview.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/monitoringsettingwidget.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/notificationssettiingwidget.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/settingswidget.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/basicsettingwidget.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/clipboardtimer.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/downloadsettingwidget.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/filesavepathchooser.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/groupselectionwidget.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/itemselectionwidget.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/settinginfoinputwidget.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/settings.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/messagebox.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/btheaderview.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/deleteitemthread.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/tabledatacontrol.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/settingslabel.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/urlthread.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/analysisurl.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/taskdelegate.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/dlmapplication.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/diagnostictool.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/func.h
)

#添加资源文件
set(QRC_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/resource/resource.qrc)
qt5_add_resources(QRC_FILES ${QRC_SOURCE_FILES})

#发布的时候生成qm文件
if (NOT (${CMAKE_BUILD_TYPE} MATCHES "Debug"))
    #生成 ts qm文件
    set(TS_FILES  ${PROJECT_SOURCE_DIR}  "${PROJECT_SOURCE_DIR}/translations/downloader_zh_CN.ts")
    qt5_create_translation(QM_FILES
        ${PROJECT_SOURCE_DIR}/translations
        ${TS_FILES}
        OPTIONS -source-language en_US -no-obsolete)
endif ()


#需要生成的moc文件，输出文件名称放在变量 mocfiles中，必须在find QT5 package才能调用
QT5_WRAP_CPP (UDLC_UI_HEADERS_MOC ${UDLC_UI_HEADERS})


#设置输出dll的名称、类型(动态库、静态库)及所用到的文件
add_library(dlmcontrolui SHARED
       mainframe.cpp
       tableView.cpp
       tableModel.cpp
       tabledatacontrol.cpp
       headerView.cpp
       itemDelegate.cpp
       topButton.cpp
       createtaskwidget.cpp
       btinfodelegate.cpp
       btinfodialog.cpp
       btinfotableview.cpp
       monitoringsettingwidget.cpp
       notificationssettiingwidget.cpp
       settingswidget.cpp
       basicsettingwidget.cpp
       clipboardtimer.cpp
       downloadsettingwidget.cpp
       filesavepathchooser.cpp
       groupselectionwidget.cpp
       itemselectionwidget.cpp
       settinginfoinputwidget.cpp
       settings.cpp
       messagebox.cpp
       btheaderview.cpp
       deleteitemthread.cpp
       settingslabel.cpp
       urlthread.cpp
       analysisurl.cpp
       taskdelegate.cpp
       dlmapplication.cpp
       diagnostictool.cpp
       func.cpp
       ${UDLC_UI_HEADERS_MOC}
       ${QRC_FILES}
       ${QM_FILES}
)

target_link_libraries(dlmcontrolui dlmaria2 dlmdatabase dtkcore dtkwidget Qt5::Core Qt5::Widgets Qt5::Svg  Qt5::Multimedia Qt5::DBus)

install(TARGETS dlmcontrolui DESTINATION lib)

