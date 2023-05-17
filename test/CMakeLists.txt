
link_libraries(spdlog::spdlog ${LIB_UV} noscommon)

add_executable(test_libuv test_libuv.cpp)

add_executable(test_tcpserver test_tcp_server.cpp)

add_executable(test_serial test_serial.cpp)
