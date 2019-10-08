
file (GLOB COMMON_SRC ${COMMON_SRC}
        ../src/*.cpp
        ../src/*.c
        )

add_library(${PROJECT_NAME} ${COMMON_SRC})

if (COMMON_DEPENDECIES)
    add_dependencies(${PROJECT_NAME} ${COMMON_DEPENDECIES})
endif()