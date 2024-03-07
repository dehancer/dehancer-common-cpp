cmake_minimum_required(VERSION 3.12)

#
# Project
#
set(PROJECT_LIB dehancer_common_cpp)
project(${PROJECT_LIB} VERSION 0.13.0)

#
# Options
#
option(PRINT_DEBUG OFF)
option(BUILD_TESTING "Enable creation of  tests." OFF)
option(DEHANCER_TARGET_ARCH "")
option(VCPKG_TARGET_TRIPLET "")
option(DEHANCER_DEBUG OFF)
option(USE_UNICODE OFF)
option(DEHANCER_CONTROLLED_SINGLETON OFF)


#
# Global settings
#
set(VERSION_FILE "version.txt")
set(CMAKE_CXX_STANDARD 17)


if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -EHsc -Wno-deprecated-declarations")

    if (DEHANCER_DEBUG)
        string(REPLACE "/O2" "" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
        string(REPLACE "/O2" "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
        string(REPLACE "/Ob2" "" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
        string(REPLACE "/Ob2" "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
        string(REPLACE "/DNDEBUG" "" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
        string(REPLACE "/DNDEBUG" "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG=1 -DZ7=1")
        add_definitions(/DEBUG /Z7)
    endif ()

    if (USE_UNICODE)
        add_definitions(/D_UNICODE /DUNICODE)
    endif ()

    add_definitions(/MT /DWIN32=1 /D_ITERATOR_DEBUG_LEVEL=0)

    string(TOLOWER  ${CMAKE_BUILD_TYPE} BUILD_TYPE)
    if (${BUILD_TYPE} STREQUAL "debug")
        add_definitions(/MTd)
    endif()
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -rdynamic")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=default -Wno-unused-command-line-argument")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
endif ()

if(DEHANCER_CONTROLLED_SINGLETON)
    add_definitions(-DDEHANCER_CONTROLLED_SINGLETON=1)
endif ()

# googletest, etc
set (EXTERNAL_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/external)

if(DEHANCER_CONTROLLED_SINGLETON)
    add_definitions(-DDEHANCER_CONTROLLED_SINGLETON=1)
endif()

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
# Programs
#
find_program(CCACHE_FOUND ccache)

#
# CCache settings
#
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

if(PRINT_DEBUG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPRINT_DEBUG -DDEBUG_BUILD=1")
endif ()

#
# Project dependencies
#
set(COMMON_DEPENDENCIES)
set(COMMON_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})

include(FetchContent)

set(FETCHCONTENT_QUIET FALSE)

if (SQLite3_FOUND)
    message("SQLITE FOUND...")
    set(COMMON_LIBRARIES ${COMMON_LIBRARIES};${SQLite3_LIBRARIES})
    include_directories(
            ${SQLite3_INCLUDE_DIRS}
    )
else()
    add_definitions("-DUSE_SQLITE3_CONFIG=1")

    FetchContent_Declare(
            sqlite3
            GIT_REPOSITORY git@github.com:dehancer/sqlite-amalgamation.git
            GIT_TAG      dehancer-imp-1.0
            GIT_PROGRESS TRUE
            SOURCE_DIR "${CMAKE_BINARY_DIR}/sqlite3-src"
            BINARY_DIR "${CMAKE_BINARY_DIR}/sqlite3-build"
    )

    FetchContent_MakeAvailable(sqlite3)

    include_directories(
            ${sqlite3_SOURCE_DIR}
    )

    set(SQLite3_INCLUDE_DIR ${sqlite3_SOURCE_DIR} CACHE INTERNAL "SQLite3_INCLUDE_DIR")
    set(SQLite3_LIB_DIR ${sqlite3_BINARY_DIR} CACHE INTERNAL "SQLite3_LIB_DIR")

    set(COMMON_DEPENDENCIES ${COMMON_DEPENDENCIES};SQLite3)

endif ()

string(STRIP "${CMAKE_C_FLAGS} -D_ITERATOR_DEBUG_LEVEL=0" C_FLAGS)
string(STRIP "${CMAKE_CXX_FLAGS} -D_ITERATOR_DEBUG_LEVEL=0" CXX_FLAGS)


#
# Google Test
#
if (BUILD_TESTING)

    find_package(GTest)

    if (NOT GTest_FOUND)

        string(STRIP "${CMAKE_C_FLAGS} -D_ITERATOR_DEBUG_LEVEL=0" C_FLAGS)
        string(STRIP "${CMAKE_CXX_FLAGS} -D_ITERATOR_DEBUG_LEVEL=0" CXX_FLAGS)

        set(CMAKE_OSX_ARCHITECTURES "${CMAKE_OSX_ARCHITECTURES}" CACHE STRING "" FORCE)

        FetchContent_Declare(
                googletest
                GIT_REPOSITORY https://github.com/google/googletest.git
                GIT_TAG      v1.14.0
                GIT_PROGRESS TRUE
                SOURCE_DIR "${CMAKE_BINARY_DIR}/googletest-src"
        )

        FetchContent_GetProperties(googletest)

        if(NOT googletest_POPULATED)
            FetchContent_Populate(googletest)
            add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
        endif()

    endif ()

endif ()


FetchContent_Declare(
        ed25519cpp
        GIT_REPOSITORY git@github.com:dehancer/ed25519cpp.git
        GIT_TAG      develop
        GIT_PROGRESS TRUE
        SOURCE_DIR ${CMAKE_BINARY_DIR}/ed25519cpp-src
        BINARY_DIR ${CMAKE_BINARY_DIR}/ed25519cpp-build
)

FetchContent_Declare(
        base64cpp
        GIT_REPOSITORY git@github.com:dehancer/base64cpp.git
        GIT_TAG      develop
        GIT_PROGRESS TRUE
        SOURCE_DIR ${CMAKE_BINARY_DIR}/base64cpp-src
        BINARY_DIR ${CMAKE_BINARY_DIR}/base64cpp-build
)

FetchContent_MakeAvailable(ed25519cpp base64cpp)

include_directories(
        ${base64cpp_SOURCE_DIR}/include
        ${ed25519cpp_SOURCE_DIR}/include
)

set(COMMON_LIBRARIES ${COMMON_LIBRARIES};ed25519cpp;base64cpp)

#
# Details
#
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/external
        ${SQLITE3_INCLUDE_DIRS}
)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib)

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

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/dehancer-common-cpp.pc.in ${CMAKE_CURRENT_BINARY_DIR}/dehancer-common-cpp.pc @ONLY)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/dehancer-common-cpp.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
        )
