project(diagnose_dups)

set(SOURCES
    Read.hpp
    Read.cpp
    Signature.hpp
    Signature.cpp
)

xadd_library(diagnose_dups ${SOURCES})
target_link_libraries(diagnose_dups common ${HTSlib_LIBRARIES} ${Boost_LIBRARIES})
