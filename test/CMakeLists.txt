find_package(Threads REQUIRED)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)

set(GTEST_ROOT ${PROJECT_SOURCE_DIR}/ext/googletest/build/install/usr/local/)
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(test_ws_network ./test_ws_network.cpp)
target_link_libraries(test_ws_network ${CMAKE_THREAD_LIBS_INIT} ${GTEST_BOTH_LIBRARIES})
add_test([Unitest]ws_network test_ws_network)

add_executable(test_tau_matrix test_time_delay_matrix.cpp)
target_link_libraries(test_tau_matrix ${CMAKE_THREAD_LIBS_INIT} ${GTEST_BOTH_LIBRARIES})
add_test([Unitest]tau_matrix test_tau_matrix)

add_executable(test_fhn_model ./test_fhn_model.cpp)
target_link_libraries(test_fhn_model ${CMAKE_THREAD_LIBS_INIT} ${GTEST_BOTH_LIBRARIES})
add_test([Unitest]fhn_model test_fhn_model)