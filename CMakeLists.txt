cmake_minimum_required(VERSION 2.6)
project(gift_buyer)

add_executable(gift_buyer main.cpp)

install(TARGETS gift_buyer RUNTIME DESTINATION bin)
