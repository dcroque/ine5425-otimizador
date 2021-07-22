cmake_minimum_required(VERSION 3.15)
project(otimizador CXX)

add_executable(otimizador)

target_sources(
    otimizador
    PRIVATE
        main.cpp
        structures.hpp
        structures.cpp
)