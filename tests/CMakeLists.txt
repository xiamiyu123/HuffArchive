
# ==========================================
# 测试组织结构：按模块分组
# ==========================================

# 添加子目录，每个子目录有自己的 CMakeLists.txt
add_subdirectory(structure)  # Structure 数据结构测试
add_subdirectory(io)         # IO 模块测试
add_subdirectory(model)      # Model 模块测试
add_subdirectory(view)       # View 模块测试
add_subdirectory(command)    # Command 模块测试
