# Install script for directory: /Users/denn/Develop/Dehancer/Dehancer-Plugins/dehancer-common-cpp/cmake-build-debug/base64cpplib-prefix/src/base64cpplib/lib

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local/base64cpp")
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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/denn/Develop/Dehancer/Dehancer-Plugins/dehancer-common-cpp/cmake-build-debug/base64cpplib-prefix/src/base64cpplib-build/lib/libbase64cpp.a")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libbase64cpp.a" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libbase64cpp.a")
    execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libbase64cpp.a")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES "/Users/denn/Develop/Dehancer/Dehancer-Plugins/dehancer-common-cpp/cmake-build-debug/base64cpplib-prefix/src/base64cpplib/include/base64cpp.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/base64cpp" TYPE FILE FILES
    "/Users/denn/Develop/Dehancer/Dehancer-Plugins/dehancer-common-cpp/cmake-build-debug/base64cpplib-prefix/src/base64cpplib-build/lib/generated/base64cppConfig.cmake"
    "/Users/denn/Develop/Dehancer/Dehancer-Plugins/dehancer-common-cpp/cmake-build-debug/base64cpplib-prefix/src/base64cpplib-build/lib/generated/base64cppConfigVersion.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/base64cpp" TYPE FILE FILES "/Users/denn/Develop/Dehancer/Dehancer-Plugins/dehancer-common-cpp/cmake-build-debug/base64cpplib-prefix/src/base64cpplib/lib/cmake/base64cpp.cmake")
endif()

