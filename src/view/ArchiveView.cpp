#include "view/ArchiveView.h"
#include "command/DecompressDirectoryCommand.h"
#include <QIcon>
#include <QFileInfo>
#include <QDateTime>
#include <fstream>
#include <iostream>

namespace View {

ArchiveView::ArchiveView(const Structure::String& archivePath, QWidget *parent)
    : QWidget(parent),
      m_archivePath(archivePath),
      m_mainLayout(nullptr),
      m_toolBar(nullptr),
      m_fileList(nullptr),
      m_statusLabel(nullptr)
{
    setupUI();
    setupConnections();
    loadArchive();
}

ArchiveView::~ArchiveView() {
}

void ArchiveView::setupUI() {
    // 创建主布局
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);
    
    // 设置工具栏
    setupToolBar();
    
    // 设置文件列表
    setupFileList();
    
    // 创建状态栏
    m_statusLabel = new QLabel(this);
    m_statusLabel->setStyleSheet(
        "QLabel {"
        "   padding: 4px 8px;"
        "   background-color: #F5F5F5;"
        "   border-top: 1px solid #E0E0E0;"
        "}"
    );
    m_mainLayout->addWidget(m_statusLabel);
}

void ArchiveView::setupToolBar() {
    m_toolBar = new QToolBar(this);
    m_toolBar->setMovable(false);
    m_toolBar->setIconSize(QSize(32, 32));
    m_toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    
    m_toolBar->setStyleSheet(
        "QToolBar {"
        "   background-color: #F8F8F8;"
        "   border-bottom: 1px solid #E0E0E0;"
        "   spacing: 2px;"
        "   padding: 4px;"
        "}"
        "QToolButton {"
        "   background-color: transparent;"
        "   border: 1px solid transparent;"
        "   border-radius: 4px;"
        "   padding: 4px;"
        "   margin: 2px;"
        "}"
        "QToolButton:hover {"
        "   background-color: #E3F2FD;"
        "   border-color: #90CAF9;"
        "}"
        "QToolButton:pressed {"
        "   background-color: #BBDEFB;"
        "}"
    );
    
    // 创建工具按钮
    m_openBtn = new QPushButton("打开", this);
    m_extractBtn = new QPushButton("解压", this);
    m_newFolderBtn = new QPushButton("新建", this);
    m_addBtn = new QPushButton("添加", this);
    m_deleteBtn = new QPushButton("删除", this);
    m_testBtn = new QPushButton("测试", this);
    m_propertiesBtn = new QPushButton("查看", this);
    m_helpBtn = new QPushButton("代码页", this);
    
    // 添加按钮到工具栏
    m_toolBar->addWidget(m_openBtn);
    m_toolBar->addSeparator();
    m_toolBar->addWidget(m_extractBtn);
    m_toolBar->addSeparator();
    m_toolBar->addWidget(m_newFolderBtn);
    m_toolBar->addWidget(m_addBtn);
    m_toolBar->addWidget(m_deleteBtn);
    m_toolBar->addSeparator();
    m_toolBar->addWidget(m_testBtn);
    m_toolBar->addSeparator();
    m_toolBar->addWidget(m_propertiesBtn);
    m_toolBar->addSeparator();
    m_toolBar->addWidget(m_helpBtn);
    
    m_mainLayout->addWidget(m_toolBar);
}

void ArchiveView::setupFileList() {
    m_fileList = new QTreeWidget(this);
    
    // 设置列
    QStringList headers;
    headers << "名称" << "修改日期" << "类型" << "压缩后大小" << "大小";
    m_fileList->setHeaderLabels(headers);
    
    // 设置列宽
    m_fileList->setColumnWidth(0, 300);
    m_fileList->setColumnWidth(1, 200);
    m_fileList->setColumnWidth(2, 150);
    m_fileList->setColumnWidth(3, 120);
    m_fileList->setColumnWidth(4, 120);
    
    // 设置样式
    m_fileList->setStyleSheet(
        "QTreeWidget {"
        "   background-color: white;"
        "   border: 1px solid #E0E0E0;"
        "   alternate-background-color: #F9F9F9;"
        "}"
        "QTreeWidget::item {"
        "   padding: 4px;"
        "}"
        "QTreeWidget::item:selected {"
        "   background-color: #E3F2FD;"
        "   color: black;"
        "}"
        "QTreeWidget::item:hover {"
        "   background-color: #F5F5F5;"
        "}"
        "QHeaderView::section {"
        "   background-color: #F0F0F0;"
        "   padding: 6px;"
        "   border: none;"
        "   border-right: 1px solid #E0E0E0;"
        "   border-bottom: 1px solid #E0E0E0;"
        "   font-weight: bold;"
        "}"
    );
    
    m_fileList->setAlternatingRowColors(true);
    m_fileList->setRootIsDecorated(true);
    m_fileList->setSortingEnabled(true);
    m_fileList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    
    m_mainLayout->addWidget(m_fileList);
}

void ArchiveView::setupConnections() {
    connect(m_openBtn, &QPushButton::clicked, this, &ArchiveView::onOpen);
    connect(m_extractBtn, &QPushButton::clicked, this, &ArchiveView::onExtract);
    connect(m_newFolderBtn, &QPushButton::clicked, this, &ArchiveView::onNewFolder);
    connect(m_addBtn, &QPushButton::clicked, this, &ArchiveView::onAdd);
    connect(m_deleteBtn, &QPushButton::clicked, this, &ArchiveView::onDelete);
    connect(m_testBtn, &QPushButton::clicked, this, &ArchiveView::onTest);
    connect(m_propertiesBtn, &QPushButton::clicked, this, &ArchiveView::onProperties);
    connect(m_helpBtn, &QPushButton::clicked, this, &ArchiveView::onHelp);
}

void ArchiveView::loadArchive() {
    m_fileList->clear();
    
    // 读取压缩文件头信息
    std::ifstream inFile(m_archivePath.c_str(), std::ios::binary);
    if (!inFile) {
        m_statusLabel->setText("无法打开文件: " + QString::fromStdString(m_archivePath.c_str()));
        return;
    }
    
    // 验证魔数
    char magic[5] = {0};
    inFile.read(magic, 4);
    if (std::string(magic) != "HUFF") {
        m_statusLabel->setText("不是有效的 HUFF 压缩文件");
        inFile.close();
        return;
    }
    
    // 读取频率表大小并跳过
    int mapSize;
    inFile.read(reinterpret_cast<char*>(&mapSize), sizeof(int));
    for (int i = 0; i < mapSize; ++i) {
        unsigned char c;
        int f;
        inFile.read(reinterpret_cast<char*>(&c), 1);
        inFile.read(reinterpret_cast<char*>(&f), sizeof(int));
    }
    
    // 读取文件数量
    int fileCount;
    inFile.read(reinterpret_cast<char*>(&fileCount), sizeof(int));
    
    // 读取目录信息
    long long totalOriginalSize = 0;
    long long totalCompressedSize = 0;
    
    for (int i = 0; i < fileCount; ++i) {
        int pathLen;
        inFile.read(reinterpret_cast<char*>(&pathLen), sizeof(int));
        
        char* pathBuf = new char[pathLen + 1];
        inFile.read(pathBuf, pathLen);
        pathBuf[pathLen] = '\0';
        QString relativePath = QString::fromUtf8(pathBuf);
        delete[] pathBuf;
        
        long long origSize, compSize, offset;
        inFile.read(reinterpret_cast<char*>(&origSize), sizeof(long long));
        inFile.read(reinterpret_cast<char*>(&compSize), sizeof(long long));
        inFile.read(reinterpret_cast<char*>(&offset), sizeof(long long));
        
        totalOriginalSize += origSize;
        totalCompressedSize += compSize;
        
        // 创建树形项
        QTreeWidgetItem* item = new QTreeWidgetItem(m_fileList);
        item->setText(0, relativePath);
        item->setText(1, ""); // 修改日期（暂时为空）
        
        // 根据扩展名判断类型
        QString ext = relativePath.split('.').last().toLower();
        QString fileType;
        if (ext == "txt") fileType = "文本文档";
        else if (ext == "cpp" || ext == "h") fileType = "C++ 源文件";
        else if (ext == "json") fileType = "JSON 源文件";
        else if (ext == "html") fileType = "Microsoft Edge HTML 文档";
        else if (ext == "ini") fileType = "配置文件";
        else fileType = ext.toUpper() + " 文件";
        
        item->setText(2, fileType);
        item->setText(3, QString::number(compSize));
        item->setText(4, QString::number(origSize));
        
        m_fileList->addTopLevelItem(item);
    }
    
    inFile.close();
    
    // 更新状态栏
    double compressionRatio = totalOriginalSize > 0 ? 
        (double)totalCompressedSize / totalOriginalSize * 100.0 : 0.0;
    
    m_statusLabel->setText(
        QString("文件: %1, 文件夹: 0, 压缩后大小: %2 MB")
            .arg(fileCount)
            .arg(totalCompressedSize / (1024.0 * 1024.0), 0, 'f', 2)
    );
}

void ArchiveView::onOpen() {
    // TODO: 打开选中的文件
}

void ArchiveView::onExtract() {
    // TODO: 解压选中的文件
}

void ArchiveView::onNewFolder() {
    // TODO: 在压缩包中新建文件夹
}

void ArchiveView::onAdd() {
    // TODO: 添加文件到压缩包
}

void ArchiveView::onDelete() {
    // TODO: 从压缩包删除文件
}

void ArchiveView::onTest() {
    // TODO: 测试压缩文件完整性
}

void ArchiveView::onProperties() {
    // TODO: 显示文件属性
}

void ArchiveView::onHelp() {
    // TODO: 显示代码页帮助
}

}
