cmake_minimum_required(VERSION 3.25)

project(
    CppExchangeEngine
    VERSION 0.1.0
    DESCRIPTION "A modern C++20 electronic exchange simulator."
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(exchange_engine_lib
    src/order_book.cpp
)

target_include_directories(exchange_engine_lib
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

if(MSVC)
    target_compile_options(exchange_engine_lib PRIVATE /W4 /permissive-)
else()
    target_compile_options(exchange_engine_lib PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(exchange_engine
    src/main.cpp
)

target_link_libraries(exchange_engine
    PRIVATE
        exchange_engine_lib
)

option(BUILD_TESTING "Build automated tests" ON)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()