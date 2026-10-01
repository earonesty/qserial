# Force cmake to reports its compiler name as apple clang if it's using apple clang
# Otherwise it will report it as clang and the version will be one that doesn't even exist
# for clang such as 11
cmake_policy(SET CMP0025 NEW)
enable_language(CXX)

project(qserial)

# cmake settings
cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

option(VALGRIND "Run tests with valgrind" OFF)
option(BENCHMARK "Build benchmark test" OFF)

add_library(qserial INTERFACE)
target_include_directories(qserial INTERFACE ./)

enable_testing()

file(GLOB TEST_SOURCES "test_*.cpp")
add_executable(qserial-test ${TEST_SOURCES})
target_link_libraries(qserial-test PRIVATE qserial)

if(${BENCHMARK})
    find_package(Protobuf REQUIRED)
    protobuf_generate_cpp(BENCH_PROTO_SOURCES BENCH_PROTO_HEADERS bench/bench.proto)
    add_executable(qserial-bench bench/bench_qserial.cpp ${BENCH_PROTO_SOURCES} ${BENCH_PROTO_HEADERS})
    target_include_directories(qserial-bench PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${Protobuf_INCLUDE_DIRS})
    target_link_libraries(qserial-bench qserial ${Protobuf_LITE_LIBRARIES})
    add_test(NAME qserial-bench COMMAND qserial-bench)
endif()

if(${VALGRIND})
    add_test(NAME qserial-test COMMAND valgrind --show-reachable=yes --track-origins=yes --error-exitcode=3 --leak-check=full $<TARGET_FILE:qserial-test>)
else()
    add_test(NAME qserial-test COMMAND qserial-test)
endif()

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --verbose --no-compress-output --output-on-failure)
add_dependencies(check qserial-test)
if(BENCHMARK)
    add_dependencies(check qserial-bench)
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
    # Instrument library tests, but keep profiling overhead out of the benchmark.
    target_compile_options(qserial-test PRIVATE -g -fprofile-arcs -ftest-coverage)
    target_link_libraries(qserial-test PRIVATE gcov)
endif()
