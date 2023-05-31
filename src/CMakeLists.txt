set(LIB_LOGGER_NAME logger)
set(LIB_NOSCOMMON_NAME noscommon)

## build liblogger
add_library(${LIB_LOGGER_NAME} STATIC slog_logger.cpp slog_sink_spdlog.cpp)
## build libnoscommon
add_library(${LIB_NOSCOMMON_NAME} uv_helper.cpp serial_port.cpp tcp_server.cpp slog_logger.cpp slog_sink_spdlog.cpp)

## install rule
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include)
install(TARGETS ${LIB_NOSCOMMON_NAME} DESTINATION lib)
install(TARGETS ${LIB_LOGGER_NAME} DESTINATION lib)
