include(CMakePackageConfigHelpers)

FILE(GLOB PUBLIC_INCLUDE_FILES
        ${CMAKE_SOURCE_DIR}/include/dehancer/*.h
        )

FILE(GLOB PUBLIC_JSON_INCLUDE_FILES
        ${CMAKE_SOURCE_DIR}/external/nlohmann/*.h
        )

FILE(GLOB PUBLIC_PROMISE_INCLUDE_FILES
        ${CMAKE_SOURCE_DIR}/external/promisecpp/*.hpp
        )

FILE(GLOB PUBLIC_PROMISE_DETAIL_INCLUDE_FILES
        ${CMAKE_SOURCE_DIR}/external/promisecpp/promise/*.hpp
        )

FILE(GLOB PUBLIC_PROMISE_TASK_INCLUDE_FILES
        ${CMAKE_SOURCE_DIR}/external/promisecpp/simple_task/*.hpp
        )

FILE(GLOB PUBLIC_SQLITE_ORM_INCLUDE_FILES
        ${CMAKE_SOURCE_DIR}/external/sqlite/*.h
        )

file (GLOB COMMON_SRC ${COMMON_SRC}
        ../src/*.cpp
        ../src/*.c
        )

add_library(${PROJECT_NAME} ${COMMON_SRC})

if (COMMON_DEPENDECIES)
    add_dependencies(${PROJECT_NAME} ${COMMON_DEPENDECIES})
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


install(TARGETS ${PROJECT_LIB}   DESTINATION lib)
install(FILES   ${PUBLIC_INCLUDE_FILES} DESTINATION include/dehancer)
install(FILES   ${PUBLIC_JSON_INCLUDE_FILES} DESTINATION include/nlohmann)
install(FILES   ${PUBLIC_PROMISE_INCLUDE_FILES} DESTINATION include/promisecpp)
install(FILES   ${PUBLIC_PROMISE_DETAIL_INCLUDE_FILES} DESTINATION include/promisecpp/promise)
install(FILES   ${PUBLIC_PROMISE_TASK_INCLUDE_FILES} DESTINATION include/promisecpp/simple_task)
install(FILES   ${PUBLIC_SQLITE_ORM_INCLUDE_FILES} DESTINATION include/sqlite)
install(FILES   ${project_config} ${version_config} DESTINATION lib/cmake/${PROJECT_LIB})
install(FILES cmake/${PROJECT_LIB}.cmake DESTINATION lib/cmake/${PROJECT_LIB})
