cmake_minimum_required(VERSION 3.7)

add_definitions(-DTEST_)
add_executable(draw draw.cpp)

add_executable(sincostest sincostest.cc)
