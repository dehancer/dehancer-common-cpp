include(CMakePackageConfigHelpers)

FILE(GLOB PUBLIC_INCLUDE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/dehancer/*.h
        )

FILE(GLOB PUBLIC_WINDOWS_INCLUDE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/dehancer/windows/utf8/*.h
        )

FILE(GLOB PUBLIC_JSON_INCLUDE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/../external/nlohmann/*.h
        )

FILE(GLOB PUBLIC_OBFY_INCLUDE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/../external/obfy/*.h
        )

FILE(GLOB PUBLIC_PROMISE_INCLUDE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/../external/promisecpp/*.hpp
        )

FILE(GLOB PUBLIC_PROMISE_DETAIL_INCLUDE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/../external/promisecpp/promise/*.hpp
        )

FILE(GLOB PUBLIC_PROMISE_TASK_INCLUDE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/../external/promisecpp/simple_task/*.hpp
        )

FILE(GLOB PUBLIC_SQLITE_ORM_INCLUDE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/../external/sqlite/*.h
        )

set(sqlite3_src)
if (sqlite3_POPULATED)
    set(sqlite3_src ${sqlite3_SOURCE_DIR}/sqlite3.c)
endif ()

file (GLOB COMMON_SRC ${COMMON_SRC}
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/*.c
        ${sqlite3_src}
        )

if(WIN32)
    file (GLOB EXT_COMMON_SRC ${EXT_COMMON_SRC}
            ${CMAKE_CURRENT_SOURCE_DIR}/../src/platforms/windows/utf8/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/../src/platforms/windows/utf8/*.c
            ${CMAKE_CURRENT_SOURCE_DIR}/../src/platforms/common/*.cpp
            )
elseif(IOS)
    file (GLOB EXT_COMMON_SRC ${EXT_COMMON_SRC}
            ${CMAKE_CURRENT_SOURCE_DIR}/../src/platforms/ios/*.mm
            )
else()
    #set (EXT_COMMON_SRC)
    file (GLOB EXT_COMMON_SRC ${EXT_COMMON_SRC}
            ${CMAKE_CURRENT_SOURCE_DIR}/../src/platforms/common/*.cpp
            )
endif()

message("EXT_COMMON_SRC: ${EXT_COMMON_SRC}")

add_library(${PROJECT_NAME}
        ${COMMON_SRC}
        ${EXT_COMMON_SRC}
)

if (COMMON_DEPENDENCIES)
    add_dependencies(${PROJECT_NAME} ${COMMON_DEPENDENCIES})
endif()


set(config_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")
set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

# Configuration
set(version_config "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake")
set(project_config "${generated_dir}/${PROJECT_NAME}Config.cmake")
set(TARGETS_EXPORT_NAME "${PROJECT_NAME}")
set(namespace "${PROJECT_NAME}::")

write_basic_package_version_file(
        "${version_config}" COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
        "cmake/Config.cmake.in"
        "${project_config}"
        INSTALL_DESTINATION "${config_install_dir}"
)

if (NOT CMAKE_INSTALL_LIBDIR)
    set(CMAKE_INSTALL_LIBDIR lib)
endif ()

if (NOT CMAKE_INSTALL_INCLUDEDIR)
    set(CMAKE_INSTALL_INCLUDEDIR include)
endif ()

install(FILES   ${PUBLIC_INCLUDE_FILES} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dehancer)
install(FILES   ${PUBLIC_WINDOWS_INCLUDE_FILES} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dehancer/windows/utf8)
install(FILES   ${PUBLIC_JSON_INCLUDE_FILES} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nlohmann)
install(FILES   ${PUBLIC_OBFY_INCLUDE_FILES} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dehancer/obfy)
install(FILES   ${PUBLIC_PROMISE_INCLUDE_FILES} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/promisecpp)
install(FILES   ${PUBLIC_PROMISE_DETAIL_INCLUDE_FILES} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/promisecpp/promise)
install(FILES   ${PUBLIC_PROMISE_TASK_INCLUDE_FILES} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/promisecpp/simple_task)

install(DIRECTORY
        ${EXTERNAL_INSTALL_LOCATION}/include
        DESTINATION ${CMAKE_INSTALL_PREFIX}
        FILES_MATCHING
        PATTERN *.h
        PATTERN *.hpp
        PATTERN gtest* EXCLUDE
        PATTERN gmock* EXCLUDE
        )

install(DIRECTORY
        ${EXTERNAL_LOCATION}/sqlite
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include
        FILES_MATCHING
        PATTERN *.h
        PATTERN *.hpp
        )

install(DIRECTORY
        ${EXTERNAL_INSTALL_LOCATION}/lib
        DESTINATION ${CMAKE_INSTALL_PREFIX}
        FILES_MATCHING
#        PATTERN *sqlite3*.*
        PATTERN *gtest* EXCLUDE
        PATTERN *gmock* EXCLUDE
        )

install(DIRECTORY
        ${EXTERNAL_INSTALL_LOCATION}/cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        FILES_MATCHING
        PATTERN SQLite3*
        )

install(FILES   ${project_config} ${version_config} DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_LIB})
install(FILES cmake/${PROJECT_LIB}.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_LIB})

message("PROJECT_LIB: ${PROJECT_LIB}")

install(TARGETS ${PROJECT_LIB}   DESTINATION ${CMAKE_INSTALL_LIBDIR})
