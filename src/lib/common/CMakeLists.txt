project(common)

set(SOURCES
    Options.cpp
    Options.hpp
    Utility.hpp
    Utility.cpp
)

add_library(common ${SOURCES})
target_link_libraries(common rt ${Boost_LIBRARIES})
