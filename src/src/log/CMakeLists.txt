include_directories(${PROJECT_SOURCE_DIR}/src/include/log)

add_library(log SHARED
    log.cpp
    )
target_link_libraries(log Qt5::Core)
