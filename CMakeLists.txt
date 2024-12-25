cmake_minimum_required(VERSION 3.10)
project(timer VERSION 0.1.0 LANGUAGES C CXX)

add_executable(timer main.cpp timer.cpp time_wheel.cpp)

target_include_directories(timer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})