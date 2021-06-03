cmake_minimum_required(VERSION 3.17)
project(le)

set(CMAKE_CXX_STANDARD 14)

add_executable(le main.cpp tgaimage.cpp model.cpp model.h geometry.h)