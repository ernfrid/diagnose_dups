project(diagnose_dups)

set(SOURCES
    Read.hpp
    Read.cpp
)

add_library(diagnose_dups ${SOURCES})
target_link_libraries(diagnose_dups ${Boost_LIBRARIES})
