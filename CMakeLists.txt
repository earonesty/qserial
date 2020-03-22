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
    set(COVERAGE_COMPILER_FLAGS "-g -fprofile-arcs -ftest-coverage")

    function(append_coverage_compiler_flags)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COVERAGE_COMPILER_FLAGS}" PARENT_SCOPE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COVERAGE_COMPILER_FLAGS}" PARENT_SCOPE)
        set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} ${COVERAGE_COMPILER_FLAGS}" PARENT_SCOPE)
        message(STATUS "Appending code coverage compiler flags: ${COVERAGE_COMPILER_FLAGS}")
    endfunction() # append_coverage_compiler_flags

    append_coverage_compiler_flags()
endif()
