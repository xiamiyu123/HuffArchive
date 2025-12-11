# DataStructureA05
[![CI](https://github.com/xiamiyu123/DataStructureA05/actions/workflows/ci.yml/badge.svg)](https://github.com/xiamiyu123/DataStructureA05/actions/workflows/ci.yml)

哈夫曼压缩软件设计
【问题描述】
采用哈夫曼树求得的用于通信的二进制编码称为哈夫曼编码。利用哈夫曼编码对文本或图像进行数据压缩，设计数据压缩软件。
【设计要求】
设计基于哈夫曼编码的文本和图像压缩软件。
（1）采用静态链表的二叉树等数据结构。
（2）可以随机、文件及人工输入数据。
（3）创建哈夫曼树，生成哈夫曼编码和译码。
（4）源码、编码和压缩后的信息均以文件形式保存。
（5）可以查询和更新数据。
（6）其它完善性或扩展性功能。


## 构建指南

### 环境要求
- C++23 编译器 (MSVC, GCC, Clang)
- CMake 3.12+
- Qt 6

### 构建步骤

1. 克隆仓库
   ```bash
   git clone https://github.com/xiamiyu123/DataStructureA05.git
   cd DataStructureA05
   ```

2. 创建构建目录
   ```bash
   mkdir build
   cd build
   ```

3. 配置项目
   ```bash
   cmake ..
   ```

4. 编译
   ```bash
   cmake --build .
   ```

## TODOlist
1. Structure (数据结构层) 

- ✅ HuffmanNode.h: 哈夫曼树节点定义。
- ✅ ArrayList.h: 动态数组实现的静态链表
- ✅ HuffmanTree.h & .cpp: 哈夫曼树核心算法
- ✅ String.h & .cpp: 自定义字符串类
- ✅ PriorityQueue.h: 优先队列
- ✅ HashMap.h: 哈希表

2. Model (数据模型层) 

- 📝 DataModel.h & .cpp: 应用数据状态管理
- 📝 FileRecord.h & .cpp: 文件记录结构

3. IO (输入输出层)

- ✅ FileHandler.h & .cpp: 文件读写操作
- ✅ BitStream.h & .cpp: 位流操作
- 📝 DataGenerator.h & .cpp: 随机数据生成器(不知道为什么要求里面要有这东西...)

4. View (视图界面层)

- 📝 MainWindow.h & .cpp: 主窗口
- 📝 InputPanel.h & .cpp: 输入面板
- 📝 CodeTableView.h & .cpp: 编码表展示视图
- ✅ TreeVisualizer.h & .cpp: 哈夫曼树可视化视图

5. Command (命令控制层) 

- 📝 CompressCommand.h & .cpp: 压缩命令封装
- 📝 DecompressCommand.h & .cpp: 解压命令封装
