set (TEST check-license-test)
include(../CMakeCommon.in)

target_link_libraries(${TEST} PRIVATE ed25519cpp::ed25519cpp)
