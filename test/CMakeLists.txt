cmake_minimum_required(VERSION 3.1)

enable_testing()

find_package(Catch)

#-----------------------------------------------------------------------------
# Unit Test Library
#-----------------------------------------------------------------------------

set(sources
      main.test.cpp
      lazy/Lazy.test.cpp
)

add_executable(Lazy_test ${sources})

target_link_libraries(Lazy_test PRIVATE Lazy "philsquared::Catch")
set_target_properties(Lazy_test PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    COMPILE_DEFINITIONS "$<$<CXX_COMPILER_ID:MSVC>:_SCL_SECURE_NO_WARNINGS>"
    COMPILE_OPTIONS "$<$<CXX_COMPILER_ID:MSVC>:/EHsc;$<$<CONFIG:Release>:/Od>>"
)

#-----------------------------------------------------------------------------
# Testing
#-----------------------------------------------------------------------------

add_test(NAME "Lazy_test_default"
         COMMAND "Lazy_test"
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

#-----------------------------------------------------------------------------

add_test(NAME "Lazy_test_all"
         COMMAND "Lazy_test" "*"
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
