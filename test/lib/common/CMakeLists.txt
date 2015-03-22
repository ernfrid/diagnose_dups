cmake_minimum_required(VERSION 2.8)

set(TEST_LIBS 
    common
    ${HTSlib_LIBRARIES}
    ${Boost_LIBRARIES})
include_directories(${GTEST_INCLUDE_DIRS})

add_unit_tests(TestCommonLib
    TestUtility.cpp
    TestParse.cpp
    )
