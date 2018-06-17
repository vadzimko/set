cmake_minimum_required(VERSION 3.9)
project(set)

set(CMAKE_CXX_STANDARD 11)

add_executable(set main.cpp Set.h)
add_executable(list  sett.h)

add_executable(tests
        tests.cpp
        gtest/gtest_main.cc
        gtest/gtest.h
        gtest/gtest-all.cc
        Set.h)