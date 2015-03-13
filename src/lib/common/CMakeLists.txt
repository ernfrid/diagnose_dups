project(common)

set(SOURCES
    Options.cpp
    Options.hpp
)

add_library(common ${SOURCES})
target_link_libraries(common ${Boost_LIBRARIES})
