cmake_minimum_required(VERSION 2.8)

include(CheckFunctionExists)
include(CheckCXXSourceRuns)

check_cxx_source_runs("
    int main(int argc, char** argv) {
        int x = 5;
        return __builtin_expect(x == 6, 0);
    }
    " HAVE_BUILTIN_EXPECT)

if (HAVE_BUILTIN_EXPECT)
    set(DD_C_LIKELY_X_EXPR "__builtin_expect(!!(x), 1)")
    set(DD_C_UNLIKELY_X_EXPR "__builtin_expect(!!(x), 0)")
else()
    set(DD_C_LIKELY_X_EXPR "(x)")
    set(DD_C_UNLIKELY_X_EXPR "(x)")
endif()

function(find_library_providing func FOUND LIB)
    set(POTENTIAL_LIBRARIES "" ${ARGN})

    set(ORIG_CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES})
    foreach(lib ${POTENTIAL_LIBRARIES})
        set(CMAKE_REQUIRED_LIBRARIES ${ORIG_CMAKE_REQUIRED_LIBRARIES} ${lib})
        check_function_exists(${func} ${FOUND})
        if (${FOUND})
            message(STATUS "* ${func} found in library ${lib}")
            set(${LIB} ${lib} PARENT_SCOPE)
            break()
        endif (${FOUND})
    endforeach(lib ${POTENTIAL_LIBRARIES})
    set(CMAKE_REQUIRED_LIBRARIES ${ORIG_CMAKE_REQUIRED_LIBRARIES})
endfunction(find_library_providing func FOUND LIB)

