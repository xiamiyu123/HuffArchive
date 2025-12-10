
# ==========================================
# 自动发现并生成单元测试
# ==========================================

# 查找当前目录下所有的 .cpp 文件
file(GLOB TEST_SOURCES "*.cpp")

foreach(TEST_SOURCE ${TEST_SOURCES})
    # 获取文件名（不含扩展名）作为测试目标名称，例如 tst_io.cpp -> tst_io
    get_filename_component(TEST_TARGET ${TEST_SOURCE} NAME_WE)

    message(STATUS "Configuring test: ${TEST_TARGET}")

    # 创建测试可执行文件
    add_executable(${TEST_TARGET} ${TEST_SOURCE})

    # 链接核心库和 Qt 测试模块
    target_link_libraries(${TEST_TARGET} PRIVATE
        HuffmanCore
        Qt6::Test
    )

    # 注册到 CTest
    add_test(NAME ${TEST_TARGET} COMMAND ${TEST_TARGET})

    # Windows 下自动运行 windeployqt 部署依赖的 DLL
    if(WIN32)
        # 尝试查找 windeployqt 工具 (如果父级 CMake 没找到，这里再找一次)
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
