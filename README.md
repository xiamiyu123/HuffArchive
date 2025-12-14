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

