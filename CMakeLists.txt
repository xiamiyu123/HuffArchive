cmake_minimum_required(VERSION 3.12)
project(Huffman LANGUAGES CXX)

# 指定 C++ 标准
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 启用 Qt 自动化工具
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 若未指定构建类型，默认为 Debug
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# 不同构建类型下的编译选项
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(MSVC)
        # MSVC 编译器使用 /W4 开启高等级警告
        add_compile_options(/W4)
    else()
        # GCC/Clang 编译器使用 -Wall -Wextra
        add_compile_options(-Wall -Wextra)
    endif()
else()
    if(MSVC)
        # MSVC 优化选项通常默认已有，或者使用 /O2
        add_compile_options(/O2)
    else()
        add_compile_options(-O2)
    endif()
endif()

# 查找所需的 Qt6 模块
find_package(Qt6 COMPONENTS Core Gui Widgets Svg OpenGL OpenGLWidgets Concurrent Test REQUIRED)

# 源文件与头文件
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS include/*.h)

# 从 SOURCES 中移除 main.cpp，以便在测试中重用其他源文件
list(FILTER SOURCES EXCLUDE REGEX ".*main\\.cpp$")

# 资源目录与文件列表
set(IMAGE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/resource")
file(GLOB_RECURSE IMAGE_FILES CONFIGURE_DEPENDS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${IMAGE_DIR}/*")

# 生成 qrc 文件路径
set(GENERATED_QRC "${CMAKE_BINARY_DIR}/auto_resources.qrc")

# 构建 qrc 内容到变量
set(QRC_CONTENT "<RCC>\n  <qresource prefix=\"/\">\n")
foreach(img IN LISTS IMAGE_FILES)
    string(APPEND QRC_CONTENT "    <file>${img}</file>\n")
endforeach()
string(APPEND QRC_CONTENT "  </qresource>\n</RCC>\n")

# 仅当内容变化时才写入 qrc 文件，以避免重复触发资源编译
if(EXISTS ${GENERATED_QRC})
    file(READ ${GENERATED_QRC} OLD_QRC_CONTENT)
else()
    set(OLD_QRC_CONTENT "")
endif()

if(NOT QRC_CONTENT STREQUAL OLD_QRC_CONTENT)
    file(WRITE ${GENERATED_QRC} "${QRC_CONTENT}")
endif()

# 添加 Qt 资源
qt6_add_resources(QRC_SOURCES ${GENERATED_QRC})

# ==========================================
# 核心库定义 (包含所有业务逻辑和 GUI 代码)
# ==========================================
add_library(HuffmanCore STATIC
    ${SOURCES}
    ${HEADERS}
    ${QRC_SOURCES}
)

# 配置核心库的依赖 (PUBLIC 让链接这个库的目标自动获得这些依赖)
target_link_libraries(HuffmanCore PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Svg
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    Qt6::Concurrent
)

target_include_directories(HuffmanCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_definitions(HuffmanCore PUBLIC
    $<$<CONFIG:Release>:QT_NO_DEBUG_OUTPUT>
    $<$<CONFIG:Release>:QT_MESSAGELOGCONTEXT>
    QT_USE_QSTRINGBUILDER
    QT_FORCE_ASSERTS
)

# ==========================================
# 主程序
# ==========================================
add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE HuffmanCore)

# Release 模式下的编译与链接优化
if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GL /Gy /Oi /Ot /Ox")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -ffast-math")
endif()

file(COPY ${CMAKE_SOURCE_DIR}/resource DESTINATION ${CMAKE_BINARY_DIR})

# Windows 下自动运行 windeployqt 部署依赖的 DLL
if(WIN32)
    # 尝试查找 windeployqt 工具
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${Qt6_DIR}/../../../bin")
    
    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${WINDEPLOYQT_EXECUTABLE} --no-translations --no-system-d3d-compiler --no-opengl-sw "$<TARGET_FILE:${PROJECT_NAME}>"
            COMMENT "Running windeployqt to deploy Qt dependencies..."
        )
    else()
        message(WARNING "windeployqt not found! You may need to copy DLLs manually.")
    endif()
endif()

# ==========================================
# 单元测试配置
# ==========================================
enable_testing()
add_subdirectory(tests)

