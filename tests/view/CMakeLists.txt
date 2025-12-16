file(GLOB VIEW_TEST_SOURCES CONFIGURE_DEPENDS "*.cpp")

foreach(TEST_SOURCE ${VIEW_TEST_SOURCES})
    get_filename_component(TEST_TARGET ${TEST_SOURCE} NAME_WE)
    
    message(STATUS "Configuring view test: ${TEST_TARGET}")
    
    add_executable(${TEST_TARGET} ${TEST_SOURCE})
    target_link_libraries(${TEST_TARGET} PRIVATE HuffmanCore Qt6::Test Qt6::Widgets)
    add_test(NAME ${TEST_TARGET} COMMAND ${TEST_TARGET})
endforeach()
