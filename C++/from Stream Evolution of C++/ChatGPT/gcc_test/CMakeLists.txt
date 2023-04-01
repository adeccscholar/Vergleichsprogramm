cmake_minimum_required(VERSION 3.5)

project(gcc_test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(..)
add_executable(gcc_test ../Test.cpp ../MyAddress.cpp ../MyData.cpp ../MyLocation.cpp ../ChatGPT.cpp )

install(TARGETS gcc_test
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
