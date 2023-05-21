cmake_minimum_required(VERSION 3.20)
project(chessinator)

set(CMAKE_CXX_COMPILER clang++-15)
#set(CMAKE_CXX_COMPILER g++-12)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wno-c99-designator") # -Wconversion ")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -ggdb -Ofast -flto -march=native ")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -fno-omit-frame-pointer -ggdb ")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined ")

add_executable(chessinator main.cpp)
