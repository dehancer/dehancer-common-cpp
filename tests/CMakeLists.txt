function(add_dehancer_test test_name source)
    add_executable(${test_name} ${source})
    target_link_libraries(${test_name} PRIVATE
        dehancer_common_cpp::dehancer_common_cpp
        GTest::gtest_main
        ${ARGN}
    )
    add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()

add_dehancer_test(check-subscription-test subscription/subscription.cpp ed25519cpp::ed25519cpp)
add_dehancer_test(check-license-test license/license.cpp ed25519cpp::ed25519cpp)
add_dehancer_test(simple-sql-test sqlite/simple.cpp)
add_dehancer_test(basic-sema-test semaphore/basic.cpp)
