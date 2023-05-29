


set(LIB_LOGGER_NAME logger)
set(LIB_NOSCOMMON_NAME noscommon)


## build liblogger
add_library(${LIB_LOGGER_NAME} STATIC logger.cpp)
target_link_directories(${LIB_LOGGER_NAME} PRIVATE ${LIBS_INSTALL_DIR})
target_include_directories(${LIB_LOGGER_NAME} PRIVATE ${LIBS_INSTALL_DIR}/include)


add_library(${LIB_NOSCOMMON_NAME} uv_helper.cpp serial_port.cpp tcp_server.cpp logger.cpp)

## 添加依赖
# add_dependencies(${LIBRARY_NAME} build_libs)

## install rule
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include)
install(TARGETS ${LIB_NOSCOMMON_NAME} DESTINATION lib)
install(TARGETS ${LIB_LOGGER_NAME} DESTINATION lib)
