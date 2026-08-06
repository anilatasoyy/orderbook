cmake_minimum_required(VERSION 3.16)
project(orderbook CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

add_executable(orderbook
    src/main.cpp
    src/Order.cpp
    src/Stock.cpp
)
add_executable(benchmark 
    src/benchmark.cpp 
    src/Order.cpp 
    src/Stock.cpp
)

target_include_directories(orderbook PRIVATE include)
target_include_directories(benchmark PRIVATE include)
target_compile_definitions(orderbook PRIVATE ORDERBOOK_TRACE)