

set(LIBRARY_NAME noscommon)

add_library(${LIBRARY_NAME} STATIC logger.cpp serial_port.cpp tcp_server.cpp)

## 添加依赖
# add_dependencies(${LIBRARY_NAME} build_libs)

## install rule
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include)
install(TARGETS ${LIBRARY_NAME} DESTINATION lib)
