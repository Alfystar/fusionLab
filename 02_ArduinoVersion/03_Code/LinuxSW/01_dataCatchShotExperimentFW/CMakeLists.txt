cmake_minimum_required(VERSION 3.0)
project(DataCatch)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(EMPlibs)
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC EMPlibs)
