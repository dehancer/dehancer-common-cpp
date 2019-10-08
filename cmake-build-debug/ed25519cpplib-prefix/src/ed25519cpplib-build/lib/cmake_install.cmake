# Install script for directory: /Users/denn/Develop/Dehancer/Dehancer-Plugins/dehancer-common-cpp/cmake-build-debug/ed25519cpplib-prefix/src/ed25519cpplib/lib

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local/ed25519cpp")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/denn/Develop/Dehancer/Dehancer-Plugins/dehancer-common-cpp/cmake-build-debug/ed25519cpplib-prefix/src/ed25519cpplib-build/lib/libed25519cpp.a")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libed25519cpp.a" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libed25519cpp.a")
    execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libed25519cpp.a")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES "/Users/denn/Develop/Dehancer/Dehancer-Plugins/dehancer-common-cpp/cmake-build-debug/ed25519cpplib-prefix/src/ed25519cpplib/lib/../include/ed25519.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ed25519/c++17" TYPE FILE FILES "/Users/denn/Develop/Dehancer/Dehancer-Plugins/dehancer-common-cpp/cmake-build-debug/ed25519cpplib-prefix/src/ed25519cpplib/lib/../include/ed25519/c++17/variant.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/ed25519cpp" TYPE FILE FILES
    "/Users/denn/Develop/Dehancer/Dehancer-Plugins/dehancer-common-cpp/cmake-build-debug/ed25519cpplib-prefix/src/ed25519cpplib-build/lib/generated/ed25519cppConfig.cmake"
    "/Users/denn/Develop/Dehancer/Dehancer-Plugins/dehancer-common-cpp/cmake-build-debug/ed25519cpplib-prefix/src/ed25519cpplib-build/lib/generated/ed25519cppConfigVersion.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/ed25519cpp" TYPE FILE FILES "/Users/denn/Develop/Dehancer/Dehancer-Plugins/dehancer-common-cpp/cmake-build-debug/ed25519cpplib-prefix/src/ed25519cpplib/lib/cmake/ed25519cpp.cmake")
endif()

