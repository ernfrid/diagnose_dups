project(diagnose_dups)

set(SOURCES
    diagnose_dups.cpp
)

set(EXECUTABLE_NAME diagnose-dups)
xadd_executable(${EXECUTABLE_NAME} ${SOURCES})
target_link_libraries(${EXECUTABLE_NAME}
    common 
    diagnose_dups
    ${HTSlib_LIBRARIES}
    ${Boost_LIBRARIES})
set_target_properties(${EXECUTABLE_NAME} PROPERTIES PACKAGE_OUTPUT_NAME ${EXECUTABLE_NAME}${EXE_VERSION_SUFFIX})
install(TARGETS ${EXECUTABLE_NAME} DESTINATION bin/)
