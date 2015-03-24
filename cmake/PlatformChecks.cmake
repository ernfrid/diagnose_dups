cmake_minimum_required(VERSION 2.8)

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

