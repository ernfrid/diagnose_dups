cmake_minimum_required(VERSION 2.8)

project(diagnose_dups)

if(NOT CMAKE_INSTALL_PREFIX)
    set(CMAKE_INSTALL_PREFIX "/usr")
endif()

set(CMAKE_MODULE_PATH
    ${CMAKE_SOURCE_DIR}/cmake
    ${CMAKE_SOURCE_DIR}/build-common/cmake
)

include(TestHelper)
include(VersionHelper)
include(ProjectHelper)
include(PlatformChecks)
include(PreDepends)
PreDependsInit()

configure_file(config/config.hpp.in config/config.hpp @ONLY)
include_directories(${PROJECT_BINARY_DIR}/config)

configure_file(version/version.h.in version/version.h @ONLY)
include_directories(${PROJECT_BINARY_DIR}/version)

# Build dependencies (samtools and boost)
include(BuildHtslib)
include_directories(${HTSlib_INCLUDE_DIRS})

set(REQUIRED_BOOST_LIBS program_options timer system chrono )
include(BuildBoost)
include_directories(${Boost_INCLUDE_DIRS})

if(${CMAKE_C_COMPILER_ID} MATCHES "GNU|Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -ansi -pedantic")
endif ()

if(${CMAKE_C_COMPILER_ID} MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-long-long -Wall -ansi")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wno-variadic-macros")
endif ()

# Find clock_gettime function
find_library_providing(clock_gettime CLOCK_GETTIME_FOUND CLOCK_GETTIME_LIB rt)
if(NOT CLOCK_GETTIME_FOUND)
    message(WARNING "Unable to find the clock_gettime function! Attempting proceed without linking.")
endif(NOT CLOCK_GETTIME_FOUND)

if (CLOCK_GETTIME_LIB)
    link_libraries(${CLOCK_GETTIME_LIB})
endif (CLOCK_GETTIME_LIB)

# make sure to pick up headers from library dirs
include_directories("src/lib")

###########################################################################
## Main project
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
add_projects(src/lib)
add_projects(src/exe)

## Testing
set(TEST_DATA_DIRECTORY ${CMAKE_SOURCE_DIR}/test-data)

configure_file(test-data/TestData.hpp.in test-data/TestData.hpp @ONLY)
include_directories(${PROJECT_BINARY_DIR}/test-data)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/test-bin)

include_directories(${CMAKE_SOURCE_DIR}/test)
enable_testing(true)
add_projects(test/lib)

# integration testing
add_subdirectory(integration-test)

## Packaging
include(PackageInfo.cmake)
