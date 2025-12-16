# Integration Tests
file(GLOB INTEGRATION_TEST_SOURCES CONFIGURE_DEPENDS "*.cpp")

foreach(TEST_SOURCE ${INTEGRATION_TEST_SOURCES})
    get_filename_component(TEST_TARGET ${TEST_SOURCE} NAME_WE)
    
    message(STATUS "Configuring integration test: ${TEST_TARGET}")
    
    add_executable(${TEST_TARGET} ${TEST_SOURCE})
    target_link_libraries(${TEST_TARGET} PRIVATE HuffmanCore Qt6::Test)
    add_test(NAME ${TEST_TARGET} COMMAND ${TEST_TARGET})
endforeach()
