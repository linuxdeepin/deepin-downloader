include_directories(${PROJECT_SOURCE_DIR}/src/include/log)

link_directories(${CMAKE_BINARY_DIR}/lib)

#预定义宏，主要用于dll导出
ADD_DEFINITIONS(-LOG_LIB )

add_library(dlmlog SHARED
    log.cpp
    )
target_link_libraries(dlmlog Qt5::Core)

install(TARGETS dlmlog DESTINATION lib)
