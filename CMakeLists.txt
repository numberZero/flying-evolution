cmake_minimum_required(VERSION 2.6)
project(subsimulation)

add_executable(subsimulation main.cpp)

install(TARGETS subsimulation RUNTIME DESTINATION bin)
