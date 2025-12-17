#include "view/ArchiveView.h"
#include "view/DecompressDialog.h"
#include "command/DecompressDirectoryCommand.h"
#include "command/SelectiveDecompressCommand.h"
#include "command/AddFileCommand.h"
#include "view/ArchivePropertiesDialog.h"
#include "util/CryptoUtils.h"
#include <QIcon>
#include <QFileInfo>
#include <QDateTime>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <QHeaderView>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QFileDialog>
#include <QProgressDialog>
#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>
#include <QApplication>
#include <QInputDialog>

namespace View {

ArchiveView::ArchiveView(const Structure::String& archivePath, QWidget *parent)
    : QWidget(parent),
      m_archivePath(archivePath)
{
    setupUI();
    setupConnections();
    loadArchive();
}

ArchiveView::~ArchiveView() {
}

void ArchiveView::setupUI() {
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    
    setupToolBar();
    setupFileList();
    
    // Status Bar
    m_statusLabel = new QLabel(this);
    m_statusLabel->setStyleSheet("padding: 8px; background: #F3F3F3; color: #666666; border-top: 1px solid #E0E0E0;");
    m_mainLayout->addWidget(m_statusLabel);
    
    // Global Styles
    setStyleSheet(
        "QWidget { background-color: #FFFFFF; }"
        "QTreeWidget { border: none; }"
        "QHeaderView::section { "
        "   background-color: #FAFAFA; "
        "   border: none; "
        "   border-bottom: 1px solid #E0E0E0; "
        "   border-right: 1px solid #F0F0F0; "
        "   padding: 6px; "
        "   font-weight: bold; "
        "   color: #666666; "
        "}"
    );
}

void ArchiveView::setupToolBar() {
    m_topPanel = new QWidget(this);
    m_topPanel->setStyleSheet("background-color: #F9F9F9; border-bottom: 1px solid #E0E0E0;");
    
    QVBoxLayout* panelLayout = new QVBoxLayout(m_topPanel);
    panelLayout->setSpacing(10);
    panelLayout->setContentsMargins(10, 10, 10, 10);
    
    // 1. Action Buttons Row
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(8);
    
    auto createBtn = [this](const QString& text, const QString& iconName, const QString& objName = "") -> QPushButton* {
        QPushButton* btn = new QPushButton(text, this);
        btn->setIcon(QIcon::fromTheme(iconName));
        if (!objName.isEmpty()) btn->setObjectName(objName);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet(
            "QPushButton { "
            "   border: 1px solid transparent; "
            "   border-radius: 4px; "
            "   padding: 6px 12px; "
            "   background: transparent; "
            "   color: #333333; "
            "   font-weight: 500; "
            "}"
            "QPushButton:hover { background: #EAEAEA; border-color: #D0D0D0; }"
            "QPushButton:pressed { background: #DADADA; }"
            "QPushButton#PrimaryAction { "
            "   background: #0078D4; "
            "   color: white; "
            "}"
            "QPushButton#PrimaryAction:hover { background: #106EBE; }"
        );
        return btn;
    };
    
    m_extractBtn = createBtn("解压全部", "archive-extract", "PrimaryAction");
    m_extractSelectedBtn = createBtn("解压选中", "archive-extract");
    m_addBtn = createBtn("添加文件", "list-add");
    m_deleteBtn = createBtn("删除", "edit-delete");
    m_infoBtn = createBtn("属性信息", "dialog-information");
    
    btnLayout->addWidget(m_extractBtn);
    btnLayout->addWidget(m_extractSelectedBtn);
    btnLayout->addWidget(m_addBtn);
    btnLayout->addWidget(m_deleteBtn);
    btnLayout->addStretch();
    btnLayout->addWidget(m_infoBtn);
    
    // 2. Address Bar Row
    QHBoxLayout* addressLayout = new QHBoxLayout();
    
    QLabel* pathLabel = new QLabel("位置:", this);
    pathLabel->setStyleSheet("color: #666666; font-weight: bold;");
    
    m_pathEdit = new QLineEdit(this);
    m_pathEdit->setReadOnly(true);
    m_pathEdit->setText(QString::fromStdString(m_archivePath.c_str()));
    m_pathEdit->setStyleSheet(
        "QLineEdit { "
        "   border: 1px solid #E0E0E0; "
        "   border-radius: 4px; "
        "   padding: 6px; "
        "   background: #FFFFFF; "
        "   color: #333333; "
        "}"
    );
    
    addressLayout->addWidget(pathLabel);
    addressLayout->addWidget(m_pathEdit);
    
    panelLayout->addLayout(btnLayout);
    panelLayout->addLayout(addressLayout);
    
    m_mainLayout->addWidget(m_topPanel);
}

void ArchiveView::setupFileList() {
    m_fileList = new QTreeWidget(this);
    m_fileList->setHeaderLabels({"名称", "大小", "类型", "修改日期"});
    m_fileList->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_fileList->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_fileList->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_fileList->header()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    
    m_fileList->setAlternatingRowColors(true);
    m_fileList->setRootIsDecorated(true);
    m_fileList->setSortingEnabled(true);
    m_fileList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_fileList->setIndentation(20);
    
    // Item Style
    m_fileList->setStyleSheet(
        "QTreeWidget::item { padding: 4px; }"
        "QTreeWidget::item:selected { background-color: #E3F2FD; color: #333333; }"
        "QTreeWidget::item:hover { background-color: #F5F5F5; }"
    );
    
    m_mainLayout->addWidget(m_fileList);
}

void ArchiveView::setupConnections() {
    connect(m_extractBtn, &QPushButton::clicked, this, &ArchiveView::onExtract);
    connect(m_extractSelectedBtn, &QPushButton::clicked, this, &ArchiveView::onExtractSelected);
    connect(m_addBtn, &QPushButton::clicked, this, &ArchiveView::onAdd);
    connect(m_deleteBtn, &QPushButton::clicked, this, &ArchiveView::onDelete);
    connect(m_infoBtn, &QPushButton::clicked, this, &ArchiveView::onInfo);
    connect(m_fileList, &QTreeWidget::itemDoubleClicked, this, &ArchiveView::onItemDoubleClicked);
}

void ArchiveView::loadArchive() {
    m_fileList->clear();
    
    // 读取压缩文件头信息
    std::ifstream inFile(std::filesystem::path(reinterpret_cast<const char8_t*>(m_archivePath.c_str())), std::ios::binary);
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

    // 读取 Flag
    char flag = 0;
    inFile.read(&flag, 1);
    bool isEncrypted = (flag & 0x01);

    Util::CryptoUtils::StreamCipher* cipher = nullptr;

    if (isEncrypted) {
        char salt[8];
        inFile.read(salt, 8);
        
        char fileHash[16];
        inFile.read(fileHash, 16);

        bool ok;
        QString password = QInputDialog::getText(this, "输入密码",
                                             "该文件已加密，请输入密码：", QLineEdit::Password,
                                             "", &ok);
        if (!ok || password.isEmpty()) {
            m_statusLabel->setText("已取消或密码为空");
            inFile.close();
            return;
        }

        // 验证密码
        unsigned char computedHash[16];
        Util::CryptoUtils::hashPassword(password.toStdString(), reinterpret_cast<const unsigned char*>(salt), computedHash);
        
        if (memcmp(fileHash, computedHash, 16) != 0) {
            QMessageBox::critical(this, "错误", "密码错误！");
            m_statusLabel->setText("密码错误");
            inFile.close();
            return;
        }

        // 初始化 Cipher
        cipher = new Util::CryptoUtils::StreamCipher(password.toStdString(), reinterpret_cast<const unsigned char*>(salt));
        
        // 保存密码供后续解压使用
        m_password = password.toStdString();
    }

    auto readEncrypted = [&](char* buf, size_t size) {
        inFile.read(buf, size);
        if (cipher) {
            cipher->process(buf, size);
        }
    };
    
    // 读取频率表大小并跳过
    int mapSize;
    readEncrypted(reinterpret_cast<char*>(&mapSize), sizeof(int));
    for (int i = 0; i < mapSize; ++i) {
        unsigned char c;
        int f;
        readEncrypted(reinterpret_cast<char*>(&c), 1);
        readEncrypted(reinterpret_cast<char*>(&f), sizeof(int));
    }
    
    // 读取文件数量
    int fileCount;
    readEncrypted(reinterpret_cast<char*>(&fileCount), sizeof(int));
    
    // 读取文件列表
    long long totalOriginalSize = 0;
    long long totalCompressedSize = 0;
    
    for (int i = 0; i < fileCount; ++i) {
        // 读取路径长度
        int pathLen;
        readEncrypted(reinterpret_cast<char*>(&pathLen), sizeof(int));
        
        // 读取路径
        std::vector<char> pathBuf(pathLen + 1);
        readEncrypted(pathBuf.data(), pathLen);
        pathBuf[pathLen] = '\0';
        QString path = QString::fromUtf8(pathBuf.data());
        
        // 读取大小信息
        long long originalSize, compressedSize, offset;
        readEncrypted(reinterpret_cast<char*>(&originalSize), sizeof(long long));
        readEncrypted(reinterpret_cast<char*>(&compressedSize), sizeof(long long));
        readEncrypted(reinterpret_cast<char*>(&offset), sizeof(long long));
        
        totalOriginalSize += originalSize;
        totalCompressedSize += compressedSize;
        
        // 添加到列表
        QTreeWidgetItem* item = new QTreeWidgetItem(m_fileList);
        item->setText(0, path); // 暂时直接显示完整路径，后续可优化为树状结构
        item->setIcon(0, QIcon::fromTheme("text-x-generic"));
        item->setText(1, QString::number(originalSize / 1024.0, 'f', 1) + " KB");
        item->setText(2, "文件");
        item->setText(3, "-");
    }
    
    if (cipher) delete cipher;
    inFile.close();
    
    // 更新状态栏
    m_statusLabel->setText(
        QString("包含 %1 个文件 | 总大小: %2 MB")
            .arg(fileCount)
            .arg(totalOriginalSize / (1024.0 * 1024.0), 0, 'f', 2)
    );
}

void ArchiveView::onExtract() {
    QString qArchivePath = QString::fromStdString(m_archivePath.c_str());
    DecompressDialog dialog(qArchivePath, this);
    dialog.setPassword(m_password);
    
    dialog.exec();
    // Dialog handles execution and success message now
}

void ArchiveView::onExtractSelected() {
    // 获取选中的文件
    QList<QTreeWidgetItem*> selectedItems = m_fileList->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::information(this, "提示", "请先选择要解压的文件");
        return;
    }
    
    // 收集相对路径
    Structure::ArrayList<Structure::String> filesToExtract;
    for (QTreeWidgetItem* item : selectedItems) {
        QString relativePath = item->text(0);  // 第一列是名称（相对路径）
        filesToExtract.add(Structure::String(relativePath.toStdString().c_str()));
    }
    
    // 弹出解压对话框
    QString qArchivePath = QString::fromStdString(m_archivePath.c_str());
    DecompressDialog dialog(qArchivePath, this);
    dialog.setFilesToExtract(filesToExtract);
    dialog.setPassword(m_password);
    
    dialog.exec();
    // Dialog handles execution and success message now
}

void ArchiveView::onAdd() {
    QStringList files = QFileDialog::getOpenFileNames(this, "选择要添加的文件");
    if (files.isEmpty()) return;

    Structure::ArrayList<Structure::String> newFiles;
    for (const QString& f : files) {
        newFiles.add(Structure::String(f.toStdString().c_str()));
    }

    QProgressDialog progress("正在准备...", "取消", 0, 100, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(0);
    progress.setValue(0);

    // Use a local model for the operation
    Model::DataModel model;
    Command::AddFileCommand cmd(&model, m_archivePath, newFiles, m_archivePath);
    
    cmd.setProgressCallback([&progress](float p, const std::string& msg) {
        QMetaObject::invokeMethod(&progress, "setValue", Qt::QueuedConnection, Q_ARG(int, (int)(p * 100)));
        QMetaObject::invokeMethod(&progress, "setLabelText", Qt::QueuedConnection, Q_ARG(QString, QString::fromStdString(msg)));
    });
    
    cmd.setCheckCancelCallback([&progress]() {
        return progress.wasCanceled();
    });

    QFutureWatcher<void> watcher;
    QEventLoop loop;
    connect(&watcher, &QFutureWatcher<void>::finished, &loop, &QEventLoop::quit);
    connect(&progress, &QProgressDialog::canceled, &loop, &QEventLoop::quit);

    QFuture<void> future = QtConcurrent::run([&cmd]() {
        cmd.execute();
    });
    watcher.setFuture(future);
    
    progress.show();
    loop.exec();

    if (progress.wasCanceled()) {
        watcher.waitForFinished(); // Ensure thread finishes cleanup
        QMessageBox::information(this, "提示", "操作已取消");
    } else {
        loadArchive();
        QMessageBox::information(this, "完成", "文件添加成功");
    }
}

void ArchiveView::onDelete() {
    // TODO: 从压缩包删除文件
}

void ArchiveView::onInfo() {
    ArchivePropertiesDialog dialog(m_archivePath, this);
    dialog.exec();
}

void ArchiveView::onItemDoubleClicked(QTreeWidgetItem* item, int column) {
    Q_UNUSED(column);
    if (!item) return;
    
    // 获取相对路径
    QString relativePath = item->text(0);
    extractAndOpenFile(Structure::String(relativePath.toStdString().c_str()));
}

void ArchiveView::extractAndOpenFile(const Structure::String& relativePath) {
    // 1. 准备临时目录
    std::filesystem::path tempDir = std::filesystem::current_path() / "data" / "tmp";
    std::error_code ec;
    std::filesystem::create_directories(tempDir, ec);
    
    Structure::String tempDirStr(tempDir.string().c_str());
    
    // 2. 准备解压命令
    Structure::ArrayList<Structure::String> filter;
    filter.add(relativePath);
    
    Command::SelectiveDecompressCommand cmd(m_archivePath, tempDirStr, filter, m_password);
    
    // 3. 执行解压 (同步执行，因为只是单个文件，通常很快)
    // 显示等待光标
    QApplication::setOverrideCursor(Qt::WaitCursor);
    bool success = cmd.execute();
    QApplication::restoreOverrideCursor();
    
    if (!success) {
        QMessageBox::warning(this, "错误", "无法解压文件: " + QString::fromStdString(cmd.getErrorMessage().c_str()));
        return;
    }
    
    // 4. 打开文件
    std::filesystem::path extractedPath = tempDir / std::filesystem::path(reinterpret_cast<const char8_t*>(relativePath.c_str()));
    QString qPath = QString::fromStdString(extractedPath.string());
    
    if (!QDesktopServices::openUrl(QUrl::fromLocalFile(qPath))) {
        QMessageBox::warning(this, "错误", "无法打开文件: " + qPath);
    }
}

}
