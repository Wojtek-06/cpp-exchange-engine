cmake_minimum_required(VERSION 3.25)

project(
    CppExchangeEngine
    VERSION 0.1.0
    DESCRIPTION "A modern C++20 event-driven electronic exchange simulator."
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(exchange_engine
    src/main.cpp
    src/order_book.cpp
    src/matching_engine.cpp
)

target_include_directories(exchange_engine
    PRIVATE
        include
)