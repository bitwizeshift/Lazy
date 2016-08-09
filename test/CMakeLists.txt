cmake_minimum_required(VERSION 3.0)
project(unit_test LANGUAGES CXX)

enable_testing()

# The unit test executable.
set(UNITTEST_TARGET_NAME "unit_tests")
add_executable("unit_tests"
               "catch.hpp"
               "unit.cpp"
               "unit-assignment.cpp"
               "unit-casting.cpp"
               "unit-constructor.cpp"
               "unit-operators.cpp"
)

set_target_properties(${UNITTEST_TARGET_NAME} PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
    COMPILE_DEFINITIONS "$<$<CXX_COMPILER_ID:MSVC>:_SCL_SECURE_NO_WARNINGS>"
    COMPILE_OPTIONS "$<$<CXX_COMPILER_ID:MSVC>:/EHsc;$<$<CONFIG:Release>:/Od>>"
)

target_include_directories(${UNITTEST_TARGET_NAME} PRIVATE "../include")

add_test(NAME "${UNITTEST_TARGET_NAME}_default"
         COMMAND ${UNITTEST_TARGET_NAME}
         WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
add_test(NAME "${UNITTEST_TARGET_NAME}_all"
         COMMAND ${UNITTEST_TARGET_NAME} "*"
         WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
