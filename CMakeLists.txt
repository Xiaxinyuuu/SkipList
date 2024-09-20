cmake_minimum_required(VERSION 3.28)
project(cpp_good_coder)
set(CMAKE_CXX_STANDARD 17)
include_directories(.)
find_package(GTEST REQUIRED)


set(THRIFT_INCLUDE_DIR "/opt/homebrew/Cellar/thrift/0.20.0/include")
set(THRIFT_LIBRARIES "/opt/homebrew/Cellar/thrift/0.20.0/lib/libthrift.dylib")
set(BOOST_INCLUDE_DIR "/opt/homebrew/Cellar/boost/1.85.0/include")


include_directories(${THRIFT_INCLUDE_DIR})
include_directories(${BOOST_INCLUDE_DIR})

# 服务端程序的可执行文件
add_executable(cpp_good_coder
        server.cpp
        skip_list.h
        skip_list.cpp
        storage.h
        storage.cpp
        server.cpp
)

# 客户端程序的可执行文件
add_executable(client
        client.cpp
        storage.h
        storage.cpp
)

# 单测程序的可执行文件
add_executable(skip_list_test 
        test_skip_list.cpp
        skip_list.cpp
)

# 性能程序的可执行文件
add_executable(skip_list_perf_test
        test_skip_list_perf.cpp
        skip_list.cpp
)

target_link_libraries(skip_list_test ${GTEST_LIBRARIES})
target_link_libraries(cpp_good_coder ${THRIFT_LIBRARIES})
target_link_libraries(client ${THRIFT_LIBRARIES})



