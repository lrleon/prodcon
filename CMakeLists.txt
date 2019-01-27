cmake_minimum_required(VERSION 3.10)
include_directories(include)

set(WARN " -Wall -Wextra -Wcast-align -Wno-sign-compare -Wno-write-strings -Wno-parentheses ")

set(FLAGS " -O0 -DDEBUG -g -pthread")
#set(FLAGS " -Ofast -DNDEBUG -pthread")
set(OPTIONS "${FLAGS} ${WARN}")
set(CMAKE_CXX_FLAGS " -std=c++17 ${OPTIONS} ")

add_executable(prodcon prodcon.cc)
add_executable(priocon priocon.cc)
add_executable(gen-input gen-input.cc)


