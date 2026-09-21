file(GLOB sources CONFIGURE_DEPENDS ../src/*.cpp ../src/*.c)
if(WIN32)
    file(GLOB platform_sources CONFIGURE_DEPENDS ../src/platforms/windows/utf8/*.cpp ../src/platforms/windows/utf8/*.c ../src/platforms/common/*.cpp)
elseif(CMAKE_SYSTEM_NAME STREQUAL "iOS")
    enable_language(OBJCXX)
    file(GLOB platform_sources CONFIGURE_DEPENDS ../src/platforms/ios/*.mm)
else()
    file(GLOB platform_sources CONFIGURE_DEPENDS ../src/platforms/common/*.cpp)
endif()
add_library(dehancer_common_cpp ${sources} ${platform_sources})
add_library(dehancer_common_cpp::dehancer_common_cpp ALIAS dehancer_common_cpp)
target_compile_features(dehancer_common_cpp PUBLIC cxx_std_17)
set_target_properties(dehancer_common_cpp PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(dehancer_common_cpp PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
target_compile_definitions(dehancer_common_cpp PRIVATE
    $<$<BOOL:${PRINT_DEBUG}>:PRINT_DEBUG=1>
    $<$<BOOL:${DEHANCER_DEBUG}>:DEBUG=1;DEBUG_BUILD=1>)
if(MSVC AND DEHANCER_DEBUG)
    target_compile_options(dehancer_common_cpp PRIVATE /Od /Ob0 /Z7 /UNDEBUG)
endif()
if(WIN32)
    target_compile_definitions(dehancer_common_cpp PUBLIC WIN32=1
        $<$<BOOL:${USE_UNICODE}>:UNICODE;_UNICODE>)
endif()
target_include_directories(dehancer_common_cpp PUBLIC "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/external>")
target_link_libraries(dehancer_common_cpp PUBLIC base64cpp::base64cpp SQLite3::SQLite3 Threads::Threads
    PRIVATE ed25519cpp::ed25519cpp)
if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
    target_link_libraries(dehancer_common_cpp PRIVATE "-framework Foundation")
endif()
install(DIRECTORY ../external/nlohmann ../external/promisecpp ../external/sqlite
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")
install(DIRECTORY ../external/dehancer/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dehancer
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")

install(DIRECTORY ../include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")
set(config_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/dehancer_common_cpp")
configure_package_config_file(cmake/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/dehancer_common_cppConfig.cmake"
    INSTALL_DESTINATION "${config_install_dir}")
write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/dehancer_common_cppConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion)
install(TARGETS dehancer_common_cpp EXPORT dehancer_common_cppTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(EXPORT dehancer_common_cppTargets NAMESPACE dehancer_common_cpp:: DESTINATION "${config_install_dir}")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/dehancer_common_cppConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/dehancer_common_cppConfigVersion.cmake" DESTINATION "${config_install_dir}")
