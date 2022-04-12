cmake_minimum_required(VERSION 3.12)

#
# Project
#
set(PROJECT_LIB dehancer_common_cpp)
project(${PROJECT_LIB} VERSION 0.7.3)

#
# Options
#
option(BUILD_TESTING "Enable creation of  tests." OFF)
option(DEHANCER_TARGET_ARCH "")
option(VCPKG_TARGET_TRIPLET "")

option(DEHANCER_COMMON_DIR  "")
if (NOT DEHANCER_COMMON_DIR)
    set(DEHANCER_COMMON_DIR ${CMAKE_BINARY_DIR}/tmp)
endif ()

option(EXTERNAL_INSTALL_LOCATION  "")
if (NOT EXTERNAL_INSTALL_LOCATION)
    set (EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/external)
endif ()

if (DEHANCER_TARGET_ARCH)
    #
    # x86_64-apple-macos10.13
    # or
    # arm64-apple-macos11
    #
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -target ${DEHANCER_TARGET_ARCH}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -target ${DEHANCER_TARGET_ARCH}")
endif ()


#
# Global setttings
#
set(VERSION_FILE "version.txt")
set(CMAKE_CXX_STANDARD 17)

if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -EHsc -Wno-deprecated-declarations")
    add_definitions(/MT /DWIN32=1 /D_ITERATOR_DEBUG_LEVEL=0)
    string(TOLOWER  ${CMAKE_BUILD_TYPE} BUILD_TYPE)
    if (${BUILD_TYPE} STREQUAL "debug")
        add_definitions(/MTd)
    endif()
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -rdynamic")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -Wno-unused-command-line-argument")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
endif ()

# googletes, etc
set (EXTERNAL_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/external)
set (EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/external)
set (EXTERNAL_INCLUDE_DIR ${EXTERNAL_INSTALL_LOCATION}/include)

#
# Generators
#
file (WRITE "${VERSION_FILE}" "${PROJECT_NAME} ${PROJECT_VERSION}")
configure_file(include/dehancer/version.h.in ${CMAKE_BINARY_DIR}/generated/version.h)

#
# Packages
#
find_package (PkgConfig QUIET)
find_package (Threads REQUIRED)
find_package (SQLite3)

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

    #message( FATAL_ERROR "The project does not support MSVC compiler" )

endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPRINT_DEBUG -DDEBUG_BUILD=1")

#set(CMAKE_LINK_LIBRARY_FILE_FLAG "${CMAKE_LINK_LIBRARY_FILE_FLAG} -Wl, -fvisibility=hidden")
#set(CMAKE_LINK_LIBRARY_FILE_FLAG "${CMAKE_LINK_LIBRARY_FILE_FLAG} -Wl")

#
# Package manager
#
include(ExternalProject)

#
# External project dependencies
#
set(COMMON_DEPENDENCIES)
set(COMMON_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})

if (SQLite3_FOUND)
    message("SQLITE FOUND...")
    set(COMMON_LIBRARIES ${COMMON_LIBRARIES};${SQLite3_LIBRARIES})
    include_directories(
            ${SQLite3_INCLUDE_DIRS}
    )
endif ()

string(STRIP "${CMAKE_C_FLAGS} -D_ITERATOR_DEBUG_LEVEL=0" C_FLAGS)
string(STRIP "${CMAKE_CXX_FLAGS} -D_ITERATOR_DEBUG_LEVEL=0" CXX_FLAGS)

#
# Google Test
#
if (BUILD_TESTING)
    find_package(GTest)
    if (NOT GTEST_FOUND)
        ExternalProject_Add(googletest
                GIT_REPOSITORY https://github.com/google/googletest
                GIT_PROGRESS YES
                GIT_SHALLOW YES
                GIT_TAG release-1.11.0
                CMAKE_ARGS
                -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
                -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
                -DCMAKE_C_FLAGS=${C_FLAGS}
                -DCMAKE_CXX_FLAGS=${CXX_FLAGS}
                PREFIX ${DEHANCER_COMMON_DIR}
                )
        set(COMMON_DEPENDENCIES ${COMMON_DEPENDENCIES};googletest)
    endif ()
endif ()

if(NOT SQLite3_FOUND)
    ExternalProject_Add(sqlite3lib
            GIT_REPOSITORY https://github.com/dehancer/sqlite-amalgamation
            GIT_PROGRESS YES
            GIT_SHALLOW YES
            CMAKE_ARGS
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DBUILD_SHELL=ON
            -DBUILD_SHELL_STATIC=ON
            -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
            -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
            -DCMAKE_C_FLAGS=${C_FLAGS}
            -DCMAKE_CXX_FLAGS=${CXX_FLAGS}
            -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
            -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
            PREFIX ${DEHANCER_COMMON_DIR}
            )
    set(COMMON_DEPENDENCIES ${COMMON_DEPENDENCIES};sqlite3lib)
    string(TOLOWER  ${CMAKE_BUILD_TYPE} BUILD_TYPE)
    if (${BUILD_TYPE} STREQUAL "debug")
        set(COMMON_LIBRARIES ${COMMON_LIBRARIES};sqlite3d)
    else()
        set(COMMON_LIBRARIES ${COMMON_LIBRARIES};sqlite3)
    endif()
endif ()

ExternalProject_Add(ed25519cpplib
        GIT_REPOSITORY https://github.com/dnevera/ed25519cpp
        GIT_PROGRESS YES
        GIT_SHALLOW YES
        CMAKE_ARGS
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
        -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
        -DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}
        -DBUILD_TESTING=OFF
        -DCMAKE_C_FLAGS=${C_FLAGS}
        -DCMAKE_CXX_FLAGS=${CXX_FLAGS}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DDEHANCER_TARGET_ARCH=${DEHANCER_TARGET_ARCH}
        -DDEHANCER_COMMON_DIR=${DEHANCER_COMMON_DIR}
        -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
        -DVCPKG_TARGET_TRIPLET=x64-windows-static
        PREFIX ${DEHANCER_COMMON_DIR}
        )
set(COMMON_DEPENDENCIES ${COMMON_DEPENDENCIES};ed25519cpplib)

ExternalProject_Add(base64cpplib
        GIT_REPOSITORY https://github.com/dnevera/base64cpp
        GIT_PROGRESS YES
        GIT_SHALLOW YES
        CMAKE_ARGS
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
        -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
        -DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}
        -DBUILD_TESTING=OFF
        -DCMAKE_C_FLAGS=${C_FLAGS}
        -DCMAKE_CXX_FLAGS=${CXX_FLAGS}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DDEHANCER_TARGET_ARCH=${DEHANCER_TARGET_ARCH}
        -DDEHANCER_COMMON_DIR=${DEHANCER_COMMON_DIR}
        -DEXTERNAL_INSTALL_LOCATION=${EXTERNAL_INSTALL_LOCATION}
        -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
        -DVCPKG_TARGET_TRIPLET=x64-windows-static
        PREFIX ${DEHANCER_COMMON_DIR}
        )
set(COMMON_DEPENDENCIES ${COMMON_DEPENDENCIES};base64cpplib)
set(COMMON_LIBRARIES ${COMMON_LIBRARIES};ed25519cpp;base64cpp)

#
# Details
#
include_directories(
        ${EXTERNAL_INSTALL_LOCATION}/include
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/external
        ${SQLITE3_INCLUDE_DIRS}
)

link_directories(
        ${EXTERNAL_INSTALL_LOCATION}/lib
        ${EXTERNAL_INSTALL_LOCATION}/lib64
)

add_subdirectory(lib)

if(BUILD_TESTING)
    include(CTest)
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
