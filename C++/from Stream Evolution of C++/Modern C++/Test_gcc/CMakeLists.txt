cmake_minimum_required(VERSION 3.5)

project(Test_gcc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-std=c++20 -finput-charset=utf-8 -fexec-charset=iso-8859-15)

include_directories(..)
add_executable(Test_gcc ../Test.cpp)

install(TARGETS Test_gcc
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
