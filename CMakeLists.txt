# Force cmake to reports its compiler name as apple clang if it's using apple clang
# Otherwise it will report it as clang and the version will be one that doesn't even exist
# for clang such as 11
cmake_policy(SET CMP0025 NEW)
enable_language(CXX)

project(qserial)

# cmake settings
cmake_minimum_required(VERSION 3.11)
set(CMAKE_CXX_STANDARD 17)

option(VALGRIND "Run tests with valgrind" OFF)

add_library(qserial INTERFACE)
target_include_directories(qserial INTERFACE ./)

enable_testing()

file(GLOB TEST_SOURCES "*.cpp")
add_executable(qserial-test ${TEST_SOURCES})
target_link_libraries(qserial-test qserial)

if(${VALGRIND})
    add_test(NAME qserial-test COMMAND valgrind --show-reachable=yes --track-origins=yes --error-exitcode=3 --leak-check=full $<TARGET_FILE:qserial-test>)
else()
    add_test(NAME qserial-test COMMAND qserial-test)
endif()

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --verbose --no-compress-output --output-on-failure)
add_dependencies(check qserial-test)

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
    include(CodeCoverage)
    append_coverage_compiler_flags()
    setup_target_for_coverage_lcov(
        NAME coverage 
        EXECUTABLE $<TARGET_FILE:qserial-test> 
        DEPENDENCIES qserial-test 
        EXCLUDE "catch.hpp" "7/*" "/usr/*")
endif()
