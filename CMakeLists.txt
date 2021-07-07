cmake_minimum_required(VERSION 3.19)

project(btlejack_parser)

set(CMAKE_CXX_STANDARD 20)
file(GLOB SOURCES *.cpp)#Reload all .cpp files into variable SOURCES

add_executable(btlejack_parser ${SOURCES})