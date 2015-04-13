cmake_minimum_required(VERSION 2.8)

set(TEST_LIBS 
    diagnose_dups 
    ${HTSlib_LIBRARIES}
    ${Boost_LIBRARIES})
include_directories(${GTEST_INCLUDE_DIRS})

add_unit_tests(TestDiagnoseDupsLib
    TestRead.cpp
    TestSignature.cpp
    TestTile.cpp
    )
