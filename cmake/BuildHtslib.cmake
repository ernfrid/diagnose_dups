cmake_minimum_required(VERSION 2.8)

set(HTSLIB_ROOT ${CMAKE_BINARY_DIR}/vendor/htslib)
set(HTSLIB_LOG ${HTSLIB_ROOT}/build.log)
set(HTSLIB_LIB ${HTSLIB_ROOT}/${CMAKE_FIND_LIBRARY_PREFIXES}hts${CMAKE_STATIC_LIBRARY_SUFFIX})

find_package(ZLIB)
if (NOT ZLIB_FOUND)
    set(ZLIB_ROOT ${CMAKE_BINARY_DIR}/vendor/zlib)
    set(ZLIB_SRC ${CMAKE_BINARY_DIR}/vendor/zlib-src)
    set(ZLIB_INCLUDE_DIRS ${ZLIB_ROOT}/include)
    set(ZLIB_LIBRARIES ${ZLIB_ROOT}/lib/${CMAKE_FIND_LIBRARY_PREFIXES}z${CMAKE_STATIC_LIBRARY_SUFFIX})
    ExternalDependency_Add(
        zlib
        BUILD_BYPRODUCTS ${ZLIB_LIBRARIES}
        ARGS
            URL ${CMAKE_SOURCE_DIR}/vendor/zlib-1.2.8.tgz
            SOURCE_DIR ${ZLIB_SRC}
            BINARY_DIR ${ZLIB_SRC}
            CONFIGURE_COMMAND ./configure --prefix=${ZLIB_ROOT}
            BUILD_COMMAND make
            INSTALL_COMMAND make install
    )
endif (NOT ZLIB_FOUND)

ExternalDependency_Add(
    htslib-1.2.1
    BUILD_BYPRODUCTS ${HTSLIB_LIB}
    ARGS
        URL ${CMAKE_SOURCE_DIR}/vendor/htslib-1.2.1.tgz
        SOURCE_DIR ${HTSLIB_ROOT}
        BINARY_DIR ${HTSLIB_ROOT}
        CONFIGURE_COMMAND echo "Building htslib, build log at ${HTSLIB_LOG}"
        BUILD_COMMAND make INCLUDES=-I${ZLIB_INCLUDE_DIRS} > ${HTSLIB_LOG} 2>&1
        INSTALL_COMMAND "true"
)

set(HTSlib_INCLUDE_DIRS ${ZLIB_INCLUDE_DIRS};${HTSLIB_ROOT}/htslib)
set(HTSlib_LIBRARIES ${HTSLIB_LIB} m ${ZLIB_LIBRARIES})

if (NOT ZLIB_FOUND)
    add_dependencies(htslib-1.2.1 zlib)
endif (NOT ZLIB_FOUND)
