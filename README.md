# DataStructureA05
[![CI](https://github.com/xiamiyu123/DataStructureA05/actions/workflows/ci.yml/badge.svg)](https://github.com/xiamiyu123/DataStructureA05/actions/workflows/ci.yml)



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

## 发布

本项目使用 GitHub Actions 自动完成跨平台构建、测试和发布。

### CI 工作流

`CI` 工作流会在 `push` 到 `main/master` 以及 `pull request` 时自动执行，并在以下平台上完成构建与测试：

- Ubuntu
- macOS
- Windows

### Release 工作流

`Build and Release` 工作流支持两种触发方式：

1. 推送版本标签，例如 `v1.0.0`
2. 在 [Actions](https://github.com/xiamiyu123/DataStructureA05/actions) 页面手动触发，并填写版本号

工作流将自动：
- 在 Windows 上构建并生成 `Huffman-Windows.zip`
- 在 Linux 上构建并生成 `Huffman-Linux-x86_64.AppImage`
- 在 macOS 上构建并生成 `Huffman-macOS.dmg`
- 创建 GitHub Release 并上传以上产物

### 本地打包

打包脚本会自动切换到项目根目录，完成清理、配置、编译以及依赖项收集工作。

#### Windows (PowerShell)
```powershell
# 基础用法
.\scripts\package_win.ps1

# 如果 CMake 找不到 Qt，请手动指定 Qt 安装路径 (例如 Qt 6.x 的 msvc 或 mingw 目录)
.\scripts\package_win.ps1 -QtPath "C:\Qt\6.x.x\msvc2022_64"
```
打包完成后，项目根目录下会生成 `Huffman-Windows.zip`。

#### Linux AppImage (Bash)
```bash
# 赋予执行权限
chmod +x scripts/package_linux.sh
# 基础用法
./scripts/package_linux.sh
# 手动指定 Qt 路径
./scripts/package_linux.sh /path/to/qt/6.x.x/gcc_64
```
打包完成后，项目根目录下会生成 `Huffman-Linux-x86_64.AppImage`。

你也可以直接调用新的脚本入口：
```bash
chmod +x scripts/package_linux_appimage.sh
./scripts/package_linux_appimage.sh
```

#### macOS (Bash)
```bash
chmod +x scripts/package_macos.sh
./scripts/package_macos.sh

# 手动指定 Qt 路径
./scripts/package_macos.sh /path/to/Qt/6.x.x/macos
```
打包完成后，项目根目录下会生成 `Huffman-macOS.dmg`。

## 常见问题
- **Windows 下运行脚本提示权限错误**：请以管理员身份运行 PowerShell 并执行 `Set-ExecutionPolicy RemoteSigned -Scope CurrentUser`。
- **找不到 Qt 环境**：请确保 Qt 的 `bin` 目录已添加到系统环境变量 PATH 中。


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

9、红黑平衡二叉树及其应用
【问题描述】
一棵红黑树本身就是一棵二叉排序树。红黑树中的结点颜色是黑色或红色。从红黑树的根结点到叶子结点的路径上的黑色结点数目相同，最短的路径就是所有结点都是黑色。根据红黑树的性质，红黑树在平衡类二叉排序树中有许多应用。
【设计要求】
设计红黑平衡二叉树实现动态查找表及其应用。
（1）采用STL红黑平衡二叉树数据结构。
（2）应用基本运算，实现红黑树的简单应用。
