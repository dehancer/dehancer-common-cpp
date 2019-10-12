cmake_minimum_required(VERSION 3.12)

#
# Project
#
set(PROJECT_LIB dehancer_common_cpp)
project(${PROJECT_LIB} VERSION 0.3.3)

#
# Options
#
option(BUILD_TESTING "Enable creation of Armadilli tests." OFF)

#
# Global setttings
#
set(VERSION_FILE "version.txt")
set(CMAKE_CXX_STANDARD 17)

# googletes, etc
set (EXTERNAL_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/external)
set (EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/external)
set (EXTERNAL_INCLUDE_DIR ${EXTERNAL_INSTALL_LOCATION}/include)
set (EXIVEDK_DB_DIR ${CMAKE_SOURCE_DIR}/external/exiv2)

#
# Generators
#
file (WRITE "${VERSION_FILE}" "${PROJECT_NAME} ${PROJECT_VERSION}")
configure_file(include/dehancer/version.h.in ${CMAKE_BINARY_DIR}/generated/version.h)

#
# Packages
#
find_package (PkgConfig QUIET)
find_package (BLAS REQUIRED)
find_package (LAPACK REQUIRED)
find_package (Threads REQUIRED)
find_package (EXPAT REQUIRED)
find_package (ZLIB REQUIRED)
find_package (Iconv REQUIRED)

if(NOT PKG_CONFIG_FOUND)
    message(FATAL_ERROR "pkg-config not found!" )
    message(FATAL_ERROR "sudo apt-get install pkg-config, or brew install pkg-config")
endif()

#
# Programms
#
find_program(CCACHE_FOUND ccache)
find_program(MAKE_EXE NAMES gmake nmake make)

#
# CCache settings
#
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)


#
# Compiler settings
#

message(STATUS "CMAKE_CXX_COMPILER_ID: " ${CMAKE_CXX_COMPILER_ID})

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")

    message(STATUS "Using Clang" )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wc++17-extensions")

elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

    message(STATUS "Using gcc" )

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -W -Wall -Wextra -Wno-ignored-qualifiers -Wvariadic-macros -pthread -static-libgcc -static-libstdc++")
    set(CMAKE_CXX_FLAGS "-fvisibility=hidden -Wno-pedantic")
    set(CMAKE_LINK_DEPENDS_NO_SHARED TRUE)

    add_compile_options(-lpthread)

    if (${CMAKE_SYSTEM_NAME} STREQUAL Linux)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DX86_LINUX")
    endif()

elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")

    message( FATAL_ERROR "The project does not support Intel compiler" )

elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")

    message( FATAL_ERROR "The project does not support MSVC compiler" )

endif()

if (MSVC)
    add_definitions (/W4)
elseif (CMAKE_COMPILER_IS_GNUCXX)
endif ()

#
# Package manager
#
include(ExternalProject)

#
# External project dependencies
#
set(COMMON_DEPENDECIES)
set(COMMON_LIBRARIES ${BLAS_LIBRARIES};${LAPACK_LIBRARIES};${CMAKE_THREAD_LIBS_INIT};${MKL_LIBRARIES}; ${EXPAT_LIBRARIES}; ${ZLIB_LIBRARIES}; ${Iconv_LIBRARIES}; )

#
# Google Test
#
find_package(GTest)
if (NOT GTEST_FOUND)
    ExternalProject_Add(googletest
            GIT_REPOSITORY https://github.com/google/googletest
            CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
            )
    set(COMMON_DEPENDECIES googletest)
endif ()

pkg_check_modules(armadillo armadillo)

if (armadillo_FOUND)

    include_directories(
            ${armadillo_INCLUDE_DIRS}
    )
    link_directories(
            ${armadillo_LIBRARY_DIRS}
    )
    set(COMMON_LIBRARIES ${COMMON_LIBRARIES};${armadillo_LIBRARIES})

else()

    ExternalProject_Add(armadillo-lib
            GIT_REPOSITORY https://github.com/dehancer/armadillo-code
            GIT_TAG 9.700.x
            CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
            -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
            -DBUILD_TESTING=OFF
            )

    set(COMMON_DEPENDECIES ${COMMON_DEPENDECIES};armadillo-lib)
    set(COMMON_LIBRARIES ${COMMON_LIBRARIES};armadillo)

endif()

ExternalProject_Add(ed25519cpplib
        GIT_REPOSITORY https://github.com/dnevera/ed25519cpp
        CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
        -DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}
        )
set(COMMON_DEPENDECIES ${COMMON_DEPENDECIES};ed25519cpplib)

ExternalProject_Add(base64cpplib
        GIT_REPOSITORY https://github.com/dnevera/base64cpp
        CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
        -DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}
        )
set(COMMON_DEPENDECIES ${COMMON_DEPENDECIES};base64cpplib)

ExternalProject_Add(
        exiv2_lib
        SOURCE_DIR ${EXIVEDK_DB_DIR}
        CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
        -DBUILD_SHARED_LIBS=Off
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON
        -DEXIV2_BUILD_SAMPLES=Off
        -DEXIV2_BUILD_EXIV2_COMMAND=off
        -DBUILD_WITH_CCACHE=ON
        -DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}
)
set(COMMON_DEPENDECIES ${COMMON_DEPENDECIES};exiv2_lib)
set(COMMON_LIBRARIES ${COMMON_LIBRARIES};ed25519cpp;base64cpp;exiv2;exiv2-xmp)


#
# Details
#
include_directories(
        ${CMAKE_BINARY_DIR}/external/include
        ${CMAKE_SOURCE_DIR}/include
)

link_directories(
        ${CMAKE_BINARY_DIR}/external/lib
)

add_subdirectory(lib)

if(BUILD_TESTING)
    add_subdirectory(tests)
    enable_testing ()
endif()

# Install package
#
if (NOT CMAKE_INSTALL_LIBDIR)
    set(CMAKE_INSTALL_LIBDIR  lib)
endif ()
if (NOT CMAKE_INSTALL_INCLUDEDIR)
    set(CMAKE_INSTALL_INCLUDEDIR  include)
endif ()

set(prefix ${CMAKE_INSTALL_PREFIX})
set(exec_prefix "\${prefix}")
set(libdir "\${exec_prefix}/${CMAKE_INSTALL_LIBDIR}")
set(includedir "\${prefix}/${CMAKE_INSTALL_INCLUDEDIR}")

configure_file(${CMAKE_SOURCE_DIR}/dehancer-common-cpp.pc.in ${CMAKE_CURRENT_BINARY_DIR}/dehancer-common-cpp.pc @ONLY)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/dehancer-common-cpp.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
        )
