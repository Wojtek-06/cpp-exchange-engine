include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

add_executable(exchange_engine_tests
    test_order_book.cpp
)

target_link_libraries(exchange_engine_tests
    PRIVATE
        exchange_engine_lib
        GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(exchange_engine_tests)