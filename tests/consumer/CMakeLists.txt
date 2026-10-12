cmake_minimum_required(VERSION 3.28)
project(pfadfinder_consumer_test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(pfadfinder REQUIRED)

add_executable(consumer main.cpp)
target_link_libraries(consumer PRIVATE pfadfinder::pfadfinder)
