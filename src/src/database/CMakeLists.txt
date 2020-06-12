include_directories(${PROJECT_SOURCE_DIR}/src/include/database)

link_directories(${CMAKE_BINARY_DIR}/lib)

#预定义宏，主要用于dll导出
ADD_DEFINITIONS(-DATABASE_LIB )

#需要moc(加Q_OBJECT)的头文件
set (DWM_DATABASE_HEADERS
    ${PROJECT_SOURCE_DIR}/src/include/database/database.h
    ${PROJECT_SOURCE_DIR}/src/include/database/dbinstance.h
    ${PROJECT_SOURCE_DIR}/src/include/database/dbdefine.h
)

#需要生成的moc文件，输出文件名称放在变量 mocfiles中，必须在find QT5 package才能调用
QT5_WRAP_CPP (DWM_DATABASE_HEADERS_MOC ${DWM_DATABASE_HEADERS})

#设置输出dll的名称、类型(动态库、静态库)及所用到的文件
add_library(database SHARED
    database.cpp
    dbinstance.cpp
    ${DWM_DATABASE_HEADERS_MOC}
)

target_link_libraries(database log Qt5::Core Qt5::Widgets Qt5::Sql)

install(TARGETS database DESTINATION lib)
