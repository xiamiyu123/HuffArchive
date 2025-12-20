#include "view/ArchiveView.h"
#include "view/DecompressDialog.h"
#include "command/DecompressDirectoryCommand.h"
#include "command/SelectiveDecompressCommand.h"
#include "command/AddFileCommand.h"
#include "view/ArchivePropertiesDialog.h"
#include "util/CryptoUtils.h"
#include <QCoreApplication>
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
    QWidget* bottomBar = new QWidget(this);
    bottomBar->setStyleSheet("background: #F8F9FA; border-top: 1px solid #E8EAED;");
    QHBoxLayout* bottomLayout = new QHBoxLayout(bottomBar);
    bottomLayout->setContentsMargins(0, 0, 12, 0);
    bottomLayout->setSpacing(10);

    m_statusLabel = new QLabel(this);
    m_statusLabel->setStyleSheet(
        "QLabel {"
        "   padding: 8px 12px;"
        "   background: transparent;"
        "   color: #5F6368;"
        "   border: none;"
        "   font-size: 12px;"
        "}"
    );
    bottomLayout->addWidget(m_statusLabel, 1);

    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 0);
    m_progressBar->setFixedWidth(150);
    m_progressBar->setFixedHeight(4);
    m_progressBar->setTextVisible(false);
    m_progressBar->setStyleSheet(
        "QProgressBar {"
        "   border: none;"
        "   background-color: #E0E0E0;"
        "   border-radius: 2px;"
        "}"
        "QProgressBar::chunk {"
        "   background-color: #1A73E8;"
        "   border-radius: 2px;"
        "}"
    );
    m_progressBar->hide();
    bottomLayout->addWidget(m_progressBar);

    m_mainLayout->addWidget(bottomBar);
    
    // Global Styles
    setStyleSheet(
        "QWidget { background-color: #FFFFFF; font-family: 'Segoe UI', sans-serif; }"
        "QTreeWidget { border: none; outline: none; }"
        "QHeaderView::section { "
        "   background-color: #FFFFFF; "
        "   border: none; "
        "   border-bottom: 1px solid #E0E0E0; "
        "   padding: 8px 12px; "
        "   font-weight: 600; "
        "   color: #5F6368; "
        "   text-align: left;"
        "}"
        "QHeaderView::section:hover { background-color: #F1F3F4; }"
    );
}

void ArchiveView::setupToolBar() {
    m_topPanel = new QWidget(this);
    m_topPanel->setStyleSheet("background-color: #FFFFFF; border-bottom: 1px solid #E0E0E0;");
    
    QVBoxLayout* panelLayout = new QVBoxLayout(m_topPanel);
    panelLayout->setSpacing(12);
    panelLayout->setContentsMargins(16, 12, 16, 12);
    
    // 1. Action Buttons Row
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(12);
    
    auto createBtn = [this](const QString& text, const QString& iconName, const QString& objName = "") -> QPushButton* {
        Q_UNUSED(iconName);
        QPushButton* btn = new QPushButton(text, this);
        // btn->setIcon(QIcon::fromTheme(iconName)); // Icons might not be available on Windows without theme
        if (!objName.isEmpty()) btn->setObjectName(objName);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet(
            "QPushButton { "
            "   border: 1px solid #DADCE0; "
            "   border-radius: 6px; "
            "   padding: 6px 16px; "
            "   background: #FFFFFF; "
            "   color: #3C4043; "
            "   font-weight: 500; "
            "   font-size: 13px;"
            "}"
            "QPushButton:hover { background: #F8F9FA; border-color: #DADCE0; color: #202124; }"
            "QPushButton:pressed { background: #F1F3F4; border-color: #DADCE0; }"
            "QPushButton#PrimaryAction { "
            "   background: #1A73E8; "
            "   color: white; "
            "   border: 1px solid #1A73E8;"
            "}"
            "QPushButton#PrimaryAction:hover { background: #1557B0; border-color: #1557B0; }"
            "QPushButton#PrimaryAction:pressed { background: #174EA6; border-color: #174EA6; }"
        );
        return btn;
    };
    
    m_extractBtn = createBtn("解压全部", "archive-extract", "PrimaryAction");
    m_extractSelectedBtn = createBtn("解压选中", "archive-extract");
    m_addBtn = createBtn("添加文件", "list-add");
    m_deleteBtn = createBtn("删除", "edit-delete");
    m_infoBtn = createBtn("属性", "dialog-information");
    
    btnLayout->addWidget(m_extractBtn);
    btnLayout->addWidget(m_extractSelectedBtn);
    btnLayout->addWidget(m_addBtn);
    btnLayout->addWidget(m_deleteBtn);
    btnLayout->addStretch();
    btnLayout->addWidget(m_infoBtn);
    
    // 2. Address Bar Row
    QHBoxLayout* addressLayout = new QHBoxLayout();
    addressLayout->setSpacing(10);
    
    QLabel* pathLabel = new QLabel("位置:", this);
    pathLabel->setStyleSheet("color: #5F6368; font-weight: 500; font-size: 13px;");
    
    m_pathEdit = new QLineEdit(this);
    m_pathEdit->setReadOnly(true);
    m_pathEdit->setText(QString::fromUtf8(m_archivePath.c_str()));
    m_pathEdit->setStyleSheet(
        "QLineEdit { "
        "   border: 1px solid #E0E0E0; "
        "   border-radius: 6px; "
        "   padding: 6px 10px; "
        "   background: #F8F9FA; "
        "   color: #3C4043; "
        "   font-size: 13px;"
        "}"
        "QLineEdit:focus { border: 1px solid #1A73E8; background: #FFFFFF; }"
    );
    
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("搜索文件...");
    m_searchEdit->setFixedWidth(200);
    m_searchEdit->setStyleSheet(
        "QLineEdit { "
        "   border: 1px solid #E0E0E0; "
        "   border-radius: 6px; "
        "   padding: 6px 10px; "
        "   background: #FFFFFF; "
        "   color: #3C4043; "
        "   font-size: 13px;"
        "}"
        "QLineEdit:focus { border: 1px solid #1A73E8; }"
    );
    
    addressLayout->addWidget(pathLabel);
    addressLayout->addWidget(m_pathEdit);
    addressLayout->addWidget(m_searchEdit);
    
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
    m_fileList->setUniformRowHeights(true);
    
    // Item Style
    m_fileList->setStyleSheet(
        "QTreeWidget { "
        "   background-color: #FFFFFF;"
        "   alternate-background-color: #FAFAFA;"
        "   selection-background-color: #E8F0FE;"
        "   selection-color: #1967D2;"
        "}"
        "QTreeWidget::item { "
        "   padding: 6px;"
        "   border: none;"
        "   color: #202124;"
        "}"
        "QTreeWidget::item:hover { "
        "   background-color: #F1F3F4;"
        "}"
        "QTreeWidget::item:selected { "
        "   background-color: #E8F0FE;"
        "   color: #1967D2;"
        "}"
        "QTreeWidget::item:selected:hover { "
        "   background-color: #D2E3FC;"
        "}"
    );
    
    m_mainLayout->addWidget(m_fileList);
}

void ArchiveView::setupConnections() {
    connect(m_extractBtn, &QPushButton::clicked, this, &ArchiveView::onExtract);
    connect(m_extractSelectedBtn, &QPushButton::clicked, this, &ArchiveView::onExtractSelected);
    connect(m_addBtn, &QPushButton::clicked, this, &ArchiveView::onAdd);
    connect(m_deleteBtn, &QPushButton::clicked, this, &ArchiveView::onDelete);
    connect(m_infoBtn, &QPushButton::clicked, this, &ArchiveView::onInfo);
    connect(m_searchEdit, &QLineEdit::textChanged, this, &ArchiveView::onSearchTextChanged);
    connect(m_fileList, &QTreeWidget::itemDoubleClicked, this, &ArchiveView::onItemDoubleClicked);
    connect(&m_openFileWatcher, &QFutureWatcher<std::pair<bool, std::string>>::finished, this, &ArchiveView::onOpenFileFinished);
}

void ArchiveView::loadArchive() {
    m_fileList->clear();
    
    // 读取压缩文件头信息
    std::ifstream inFile(std::filesystem::path(reinterpret_cast<const char8_t*>(m_archivePath.c_str())), std::ios::binary);
    if (!inFile) {
        m_statusLabel->setText("无法打开文件: " + QString::fromUtf8(m_archivePath.c_str()));
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
        Util::CryptoUtils::hashPassword(password.toUtf8().constData(), reinterpret_cast<const unsigned char*>(salt), computedHash);
        
        if (memcmp(fileHash, computedHash, 16) != 0) {
            QMessageBox::critical(this, "错误", "密码错误！");
            m_statusLabel->setText("密码错误");
            inFile.close();
            return;
        }

        // 初始化 Cipher
        cipher = new Util::CryptoUtils::StreamCipher(password.toUtf8().constData(), reinterpret_cast<const unsigned char*>(salt));
        
        // 保存密码供后续解压使用
        m_password = password.toUtf8().constData();
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

    // 应用当前搜索过滤
    onSearchTextChanged(m_searchEdit->text());
}

void ArchiveView::onExtract() {
    QString qArchivePath = QString::fromUtf8(m_archivePath.c_str());
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
        filesToExtract.add(Structure::String(relativePath.toUtf8().constData()));
    }
    
    // 弹出解压对话框
    QString qArchivePath = QString::fromUtf8(m_archivePath.c_str());
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
        newFiles.add(Structure::String(f.toUtf8().constData()));
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
        QMetaObject::invokeMethod(&progress, "setLabelText", Qt::QueuedConnection, Q_ARG(QString, QString::fromUtf8(msg.c_str())));
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
    ArchivePropertiesDialog dialog(m_archivePath, this, m_password);
    dialog.exec();
}

void ArchiveView::onSearchTextChanged(const QString& text) {
    for (int i = 0; i < m_fileList->topLevelItemCount(); ++i) {
        QTreeWidgetItem* item = m_fileList->topLevelItem(i);
        bool match = item->text(0).contains(text, Qt::CaseInsensitive);
        item->setHidden(!match);
    }
}

void ArchiveView::onItemDoubleClicked(QTreeWidgetItem* item, int column) {
    Q_UNUSED(column);
    if (!item) return;
    
    // 获取相对路径
    QString relativePath = item->text(0);
    extractAndOpenFile(Structure::String(relativePath.toUtf8().constData()));
}

void ArchiveView::extractAndOpenFile(const Structure::String& relativePath) {
    if (m_openFileWatcher.isRunning()) {
        return;
    }

    m_progressBar->show();
    m_statusLabel->setText(tr("正在打开文件..."));
    m_fileList->setEnabled(false);

    // Prepare parameters for the thread
    // Use system temp directory to avoid permission issues in Release/Packaged mode
    QString tempPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/HuffmanTool/tmp";
    std::filesystem::path tempDir = std::filesystem::path(reinterpret_cast<const char8_t*>(tempPath.toUtf8().constData()));
    std::error_code ec;
    std::filesystem::create_directories(tempDir, ec);
    
    std::u8string u8TempDir = tempDir.u8string();
    std::string tempDirStr(reinterpret_cast<const char*>(u8TempDir.c_str()));
    std::string archivePathStr = m_archivePath.c_str();
    std::string relPathStr = relativePath.c_str();
    std::string passwordStr = m_password;

    // Run extraction in background
    QFuture<std::pair<bool, std::string>> future = QtConcurrent::run([=]() -> std::pair<bool, std::string> {
        try {
            Structure::String sArchivePath(archivePathStr.c_str());
            Structure::String sTempDir(tempDirStr.c_str());
            Structure::String sRelPath(relPathStr.c_str());
            
            Structure::ArrayList<Structure::String> filter;
            filter.add(sRelPath);

            Command::SelectiveDecompressCommand cmd(
                sArchivePath,
                sTempDir,
                filter,
                passwordStr
            );
            
            if (cmd.execute()) {
                // Construct the full path to the extracted file
                std::filesystem::path outDir(tempDirStr);
                std::filesystem::path relPath(relPathStr);
                // SelectiveDecompressCommand preserves directory structure relative to output dir?
                // Usually it does. Let's assume it extracts to tempDir/relPath
                // But wait, if the archive has folders, does it create them? Yes.
                
                std::filesystem::path fullPath = outDir / relPath;
                std::u8string u8FullPath = fullPath.u8string();
                return {true, std::string(reinterpret_cast<const char*>(u8FullPath.c_str()))};
            } else {
                return {false, cmd.getErrorMessage().c_str()};
            }
        } catch (const std::exception& e) {
            return {false, std::string(e.what())};
        }
    });

    m_openFileWatcher.setFuture(future);
}

void ArchiveView::onOpenFileFinished() {
    m_progressBar->hide();
    m_fileList->setEnabled(true);
    m_statusLabel->setText(tr("就绪"));

    auto result = m_openFileWatcher.result();
    if (result.first) {
        QString extractedPath = QString::fromUtf8(result.second.c_str());
        if (!QDesktopServices::openUrl(QUrl::fromLocalFile(extractedPath))) {
             QMessageBox::warning(this, tr("错误"), 
                tr("无法打开文件: %1").arg(extractedPath));
        }
    } else {
        QMessageBox::warning(this, tr("错误"), 
            tr("无法解压文件: %1").arg(QString::fromUtf8(result.second.c_str())));
    }
}

}
