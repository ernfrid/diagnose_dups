configure_file(testdata.py.in testdata.py @ONLY)
set(BC_PYTHONPATH_EXTRA ${CMAKE_CURRENT_BUILD_DIR})
def_integration_test(diagnose-dups TestDiagnoseDups diagnose_dups_test.py)
