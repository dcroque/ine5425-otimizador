cmake_minimum_required(VERSION 3.15)
project(otimizador CXX)

add_executable(otimizador)

find_package(Threads REQUIRED)
target_link_libraries(otimizador PRIVATE Threads::Threads)

target_sources(
    otimizador
    PRIVATE
        main.cpp
        library.hpp
        library.cpp
)