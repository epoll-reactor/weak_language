cmake_minimum_required(VERSION 3.20)
project(WeakLanguage)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_COMPILER "clang++")
set(OPTIMIZE_FLAGS "-fPIC -O3 -msse3 -march=native -mtune=generic")
set(EXTRA_WARN_FLAGS "-Wall -Wextra -Wcast-qual -Wnon-virtual-dtor -Wreturn-type -Wsign-promo -Wno-unused -Wcast-align -Woverloaded-virtual -Wno-parentheses -Wall -Wextra -pedantic -Wshadow -Wunused-parameter")
#set(LEAK_DETECTOR_FLAGS "-fsanitize=leak -fstack-protector")
set(LEAK_DETECTOR_FLAGS "")

set(CMAKE_CXX_FLAGS "${OPTIMISE_FLAGS} ${EXTRA_WARN_FLAGS} ${LEAK_DETECTOR_FLAGS}")

file(GLOB_RECURSE SRC include/*.hpp src/*.cpp)
add_executable(WeakLanguage ${SRC} include/storage/crc32.hpp)
