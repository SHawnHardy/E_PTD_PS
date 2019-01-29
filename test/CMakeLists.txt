add_executable(test_ws_network ./test_ws_network.cpp)
target_link_libraries(test_ws_network ${CMAKE_THREAD_LIBS_INIT} ${GTEST_BOTH_LIBRARIES})
add_test(test test_ws_network)
