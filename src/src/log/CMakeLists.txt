include_directories(${PROJECT_SOURCE_DIR}/src/include/log)

add_library(dlmlog SHARED
    log.cpp
    )
target_link_libraries(dlmlog Qt5::Core)

install(TARGETS dlmlog DESTINATION lib)
