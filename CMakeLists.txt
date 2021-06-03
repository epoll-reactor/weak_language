cmake_minimum_required(VERSION 3.20)
project(Test)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SRC include/*.hpp src/*.cpp)
add_executable(Test ${SRC})
target_link_libraries(Test dl)
