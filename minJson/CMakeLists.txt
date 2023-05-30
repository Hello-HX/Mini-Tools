cmake_minimum_required(VERSION 3.24)
project(Json)

set(CMAKE_CXX_STANDARD 17)

add_executable(Json include/JSON.h src/JSON.cpp src/Parser.cpp include/Parser.h include/Test.h src/main.cpp)

