cmake_minimum_required(VERSION 3.17)
project(MyDB)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src)

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(mydb ${SOURCES})
