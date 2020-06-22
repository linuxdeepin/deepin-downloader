#设置包含目录
include_directories( ${PROJECT_SOURCE_DIR}/src/include/ui
                     ${PROJECT_SOURCE_DIR}/src/include/aria2
                     ${PROJECT_SOURCE_DIR}/src/include/log
)

link_directories(${CMAKE_BINARY_DIR}/lib)

#预定义宏，主要用于dll导出
ADD_DEFINITIONS(-DUI_LIB )


#需要moc(加Q_OBJECT)的头文件 
set (UDLC_UI_HEADERS
    ${PROJECT_SOURCE_DIR}/src/include/ui/mainframe.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/tableView.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/tableModel.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/headerView.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/itemDelegate.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/global.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/topButton.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/newtaskwidget.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/btinfodelegate.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/btinfodialog.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/btinfotableview.h
    ${PROJECT_SOURCE_DIR}/src/include/ui/monitoringsettingwidget.h
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
)

#添加资源文件
set(QRC_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/images/images.qrc)
qt5_add_resources(QRC_FILES ${QRC_SOURCE_FILES})

#发布的时候生成qm文件
if (NOT (${CMAKE_BUILD_TYPE} MATCHES "Debug"))
    #生成 ts qm文件
    set(TS_FILES  ${PROJECT_SOURCE_DIR}  "${PROJECT_SOURCE_DIR}/translations/downloadmanager.ts")
    qt5_create_translation(QM_FILES
        ${PROJECT_SOURCE_DIR}/translations
        ${TS_FILES}
        OPTIONS -source-language en_US -no-obsolete)
endif ()


#需要生成的moc文件，输出文件名称放在变量 mocfiles中，必须在find QT5 package才能调用
QT5_WRAP_CPP (UDLC_UI_HEADERS_MOC ${UDLC_UI_HEADERS})


#设置输出dll的名称、类型(动态库、静态库)及所用到的文件
add_library(ui SHARED
       mainframe.cpp
       tableView.cpp
       tableModel.cpp
       tabledatacontrol.cpp
       headerView.cpp
       itemDelegate.cpp
       topButton.cpp
       newtaskwidget.cpp
       btinfodelegate.cpp
       btinfodialog.cpp
       btinfotableview.cpp
       monitoringsettingwidget.cpp
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
       ${UDLC_UI_HEADERS_MOC}
       ${QRC_FILES}
       ${QM_FILES}
)

target_link_libraries(ui aria2 database dtkcore dtkgui dtkwidget dtkwm Qt5::Core Qt5::Widgets Qt5::Svg)

install(TARGETS ui DESTINATION lib)

