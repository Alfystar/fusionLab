cmake_minimum_required(VERSION 3.19)
project(linux_code)

set(CMAKE_CXX_STANDARD 14)

add_executable(linux_code main.cpp)
add_subdirectory(lib/EMPlibs)
target_link_libraries(linux_code EMPlibs)