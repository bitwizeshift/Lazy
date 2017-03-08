cmake_minimum_required(VERSION 3.1)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/modules")

include(GitBuildInformation)
include(MakeVersionHeader)
include(AddIndependenceCheck)
include(AddTestExecutable)

project("Lazy")

if( CMAKE_BUILD_TYPE STREQUAL "" )
  message(STATUS "CMAKE_BUILD_TYPE not set; defaulting to 'Debug'")
  set(CMAKE_BUILD_TYPE DEBUG)
endif()

#-----------------------------------------------------------------------------
# Options
#-----------------------------------------------------------------------------

option(LAZY_COMPILE_INDEPENDENCE_TESTS "Include each header independently in a .cpp file to determine header independence" on)
option(LAZY_COMPILE_UNIT_TESTS "Compile and run the unit tests for this library" on)

#-----------------------------------------------------------------------------
# Version Info
#-----------------------------------------------------------------------------

git_build_information( MINOR_VARIABLE LAZY_VERSION_MINOR
                       PATCH_VARIABLE LAZY_VERSION_PATCH
                       BUILD_VARIABLE LAZY_BUILD_NUMBER
                       BRANCH_VARIABLE LAZY_BRANCH
                       HASH de3c5ad1771707793a3ba54bf17e11f8f29a95a5
)

set(LAZY_VERSION_MAJOR 1 CACHE STRING "major version of Lazy" FORCE)
set(LAZY_VERSION_MINOR ${LAZY_VERSION_MINOR} CACHE STRING "minor version of Lazy" FORCE)
set(LAZY_VERSION_PATCH ${LAZY_VERSION_PATCH} CACHE STRING "patch version of Lazy" FORCE)
set(LAZY_VERSION "${LAZY_VERSION_MAJOR}.${LAZY_VERSION_MINOR}.${LAZY_VERSION_PATCH}" CACHE STRING "version of Lazy" FORCE)
set(LAZY_BUILD_NUMBER "${LAZY_BUILD_NUMBER}" CACHE STRING "build number of Lazy" FORCE)

message(STATUS "Lazy ${LAZY_VERSION} (${LAZY_BRANCH} ${LAZY_BUILD_NUMBER})")

#-----------------------------------------------------------------------------
# Lazy
#-----------------------------------------------------------------------------

set(CMAKE_CXX_VERSION 14)

make_version_header("${CMAKE_CURRENT_BINARY_DIR}/include/lazy/version.hpp"
  MAJOR ${LAZY_VERSION_MAJOR}
  MINOR ${LAZY_VERSION_MINOR}
  PATCH ${LAZY_VERSION_PATCH}
  BUILD ${LAZY_BUILD_NUMBER}
  PREFIX LAZY
  SUFFIX "a"
  TAG ${LAZY_BRANCH}
)

include_directories("${CMAKE_CURRENT_BINARY_DIR}/include")

set(headers
      "include/lazy/Lazy.hpp"
)

add_library(Lazy INTERFACE)
target_include_directories(Lazy INTERFACE include)
target_compile_options(Lazy INTERFACE -std=c++14)

#-----------------------------------------------------------------------------
# Lazy : Independence Tests
#-----------------------------------------------------------------------------

if( LAZY_COMPILE_INDEPENDENCE_TESTS )

  add_independence_check(Lazy_independence ${headers})
  target_compile_options(Lazy_independence PRIVATE -std=c++14)

endif()

#-----------------------------------------------------------------------------
# Lazy : Unit Tests
#-----------------------------------------------------------------------------

if( LAZY_COMPILE_UNIT_TESTS )

  add_subdirectory(test)

endif()
