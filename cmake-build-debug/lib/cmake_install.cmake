# Install script for directory: /Users/denn/Develop/Dehancer/Dehancer-Plugins/dehancer-common-cpp/lib

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/tmp/dehancer")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/denn/Develop/Dehancer/Dehancer-Plugins/dehancer-common-cpp/cmake-build-debug/lib/libdehancer_common_cpp.a")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libdehancer_common_cpp.a" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libdehancer_common_cpp.a")
    execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libdehancer_common_cpp.a")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/dehancer" TYPE FILE FILES
    "/Users/denn/Develop/Dehancer/Dehancer-Plugins/dehancer-common-cpp/include/dehancer/Base64.h"
    "/Users/denn/Develop/Dehancer/Dehancer-Plugins/dehancer-common-cpp/include/dehancer/Common.h"
    "/Users/denn/Develop/Dehancer/Dehancer-Plugins/dehancer-common-cpp/include/dehancer/Expected.h"
    "/Users/denn/Develop/Dehancer/Dehancer-Plugins/dehancer-common-cpp/include/dehancer/License.h"
    "/Users/denn/Develop/Dehancer/Dehancer-Plugins/dehancer-common-cpp/include/dehancer/Utils.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/dehancer_common_cpp" TYPE FILE FILES
    "/Users/denn/Develop/Dehancer/Dehancer-Plugins/dehancer-common-cpp/cmake-build-debug/lib/generated/dehancer_common_cppConfig.cmake"
    "/Users/denn/Develop/Dehancer/Dehancer-Plugins/dehancer-common-cpp/cmake-build-debug/lib/generated/dehancer_common_cppConfigVersion.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/dehancer_common_cpp" TYPE FILE FILES "/Users/denn/Develop/Dehancer/Dehancer-Plugins/dehancer-common-cpp/lib/cmake/dehancer_common_cpp.cmake")
endif()

