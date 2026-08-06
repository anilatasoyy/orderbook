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

target_include_directories(orderbook PRIVATE include)
