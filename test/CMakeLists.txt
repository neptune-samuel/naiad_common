
link_libraries(common fmt::fmt uv_a)

add_executable(test_libuv test_libuv.cpp)
add_executable(test_tcpserver test_tcp_server.cpp)
add_executable(test_serial test_serial.cpp)
add_executable(test_vofa test_vofa.cpp)
