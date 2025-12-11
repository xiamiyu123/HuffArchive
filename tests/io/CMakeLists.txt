# IO Tests
file(GLOB IO_TEST_SOURCES CONFIGURE_DEPENDS "*.cpp")

foreach(TEST_SOURCE ${IO_TEST_SOURCES})
    get_filename_component(TEST_TARGET ${TEST_SOURCE} NAME_WE)
    
    message(STATUS "Configuring IO test: ${TEST_TARGET}")
    
    add_executable(${TEST_TARGET} ${TEST_SOURCE})
    target_link_libraries(${TEST_TARGET} PRIVATE HuffmanCore Qt6::Test)
    add_test(NAME ${TEST_TARGET} COMMAND ${TEST_TARGET})
    
    if(WIN32)
        if(NOT WINDEPLOYQT_EXECUTABLE)
            find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${Qt6_DIR}/../../../bin")
        endif()
        
        if(WINDEPLOYQT_EXECUTABLE)
            add_custom_command(TARGET ${TEST_TARGET} POST_BUILD
                COMMAND ${WINDEPLOYQT_EXECUTABLE} --no-translations --no-system-d3d-compiler --no-opengl-sw "$<TARGET_FILE:${TEST_TARGET}>"
                COMMENT "Running windeployqt for ${TEST_TARGET}..."
            )
        endif()
    endif()
endforeach()
