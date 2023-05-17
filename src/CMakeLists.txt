

set(LIBRARY_NAME noscommon)

add_library(${LIBRARY_NAME} STATIC logger.cpp serial_port.cpp tcp_server.cpp)

target_link_libraries(${LIBRARY_NAME} PUBLIC spdlog::spdlog ${LIB_UV})

## install rule
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include)
install(TARGETS ${LIBRARY_NAME} DESTINATION lib)
