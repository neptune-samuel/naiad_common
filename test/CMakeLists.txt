
link_libraries(noscommon fmt::fmt uv_a)

add_executable(test_libuv test_libuv.cpp)

add_executable(test_tcpserver test_tcp_server.cpp)

add_executable(test_serial test_serial.cpp)

# add_executable(test_tcpserial test_tcp_serial.cpp)
