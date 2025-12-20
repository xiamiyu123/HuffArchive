#include "view/NewArchiveDialog.h"
#include "command/CompressMultipleSourcesCommand.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QHeaderView>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QFileInfo>
#include <QDateTime>
#include <QProgressDialog>
#include <QThread>

namespace View {

// Worker class for background compression
class CompressionWorker : public QObject {
    Q_OBJECT
public:
    CompressionWorker(const Structure::ArrayList<Structure::String>& files, const Structure::String& output, const Structure::String& password = "")
        : m_files(files), m_output(output), m_password(password), m_isCancelled(false) {}

    void cancel() { m_isCancelled = true; }

public slots:
    void process() {
        Command::CompressMultipleSourcesCommand cmd(m_files, m_output);
        if (!m_password.empty()) {
            cmd.setPassword(m_password);
        }
        
        cmd.setProgressCallback([this](float p, const std::string& msg) {
            emit progress(static_cast<int>(p * 100), QString::fromUtf8(msg.c_str()));
        });
        
        cmd.setCheckCancelCallback([this]() {
            return m_isCancelled.load();
        });
        
        bool success = cmd.execute();
        emit finished(success, QString::fromUtf8(cmd.getErrorMessage().c_str()));
    }

signals:
    void progress(int value, QString message);
    void finished(bool success, QString message);

private:
    Structure::ArrayList<Structure::String> m_files;
    Structure::String m_output;
    Structure::String m_password;
    std::atomic<bool> m_isCancelled;
};

NewArchiveDialog::NewArchiveDialog(QWidget *parent)
    : QDialog(parent), m_isCompressing(false), m_worker(nullptr)
{
    setupUI();
    setupConnections();
    setAcceptDrops(true);
}

NewArchiveDialog::~NewArchiveDialog() {
}

void NewArchiveDialog::setupUI() {
    setWindowTitle("新建压缩文件");
    setMinimumSize(800, 600);
    resize(800, 500);
    
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(20);
    m_mainLayout->setContentsMargins(24, 24, 24, 24);
    
    setupHeader();
    setupFileList();
    setupBottomPanel();
    
    // 设置默认样式
    setStyleSheet(
        "QDialog { background-color: #FFFFFF; }"
        "QLabel { color: #333333; }"
        "QLineEdit { "
        "   border: 1px solid #E0E0E0; "
        "   border-radius: 4px; "
        "   padding: 8px; "
        "   background: #FAFAFA; "
        "   selection-background-color: #0078D4; "
        "}"
        "QLineEdit:focus { border-color: #0078D4; background: #FFFFFF; }"
        "QTableWidget { "
        "   border: 1px solid #E0E0E0; "
        "   border-radius: 4px; "
        "   background: #FFFFFF; "
        "   gridline-color: #F0F0F0; "
        "}"
        "QHeaderView::section { "
        "   background-color: #FAFAFA; "
        "   border: none; "
        "   border-bottom: 1px solid #E0E0E0; "
        "   padding: 6px; "
        "   font-weight: bold; "
        "   color: #666666; "
        "}"
        "QPushButton { "
        "   border: 1px solid #E0E0E0; "
        "   border-radius: 4px; "
        "   padding: 6px 16px; "
        "   background: #FFFFFF; "
        "   color: #333333; "
        "}"
        "QPushButton:hover { background: #F5F5F5; border-color: #D0D0D0; }"
        "QPushButton:pressed { background: #E0E0E0; }"
        "QPushButton#PrimaryButton { "
        "   background: #0078D4; "
        "   color: white; "
        "   border: none; "
        "   font-weight: bold; "
        "}"
        "QPushButton#PrimaryButton:hover { background: #106EBE; }"
        "QPushButton#PrimaryButton:pressed { background: #005A9E; }"
        "QPushButton#DangerButton { color: #D13438; }"
        "QPushButton#DangerButton:hover { background: #FDF3F4; border-color: #D13438; }"
    );
}

void NewArchiveDialog::setupHeader() {
    QVBoxLayout* headerLayout = new QVBoxLayout();
    headerLayout->setSpacing(4);
    
    m_titleLabel = new QLabel("创建新的压缩归档", this);
    QFont titleFont = font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    m_titleLabel->setFont(titleFont);
    
    m_subtitleLabel = new QLabel("添加文件或文件夹，设置压缩选项，然后点击开始。", this);
    m_subtitleLabel->setStyleSheet("color: #666666;");
    
    headerLayout->addWidget(m_titleLabel);
    headerLayout->addWidget(m_subtitleLabel);
    
    m_mainLayout->addLayout(headerLayout);
}

void NewArchiveDialog::setupFileList() {
    QVBoxLayout* listLayout = new QVBoxLayout();
    listLayout->setSpacing(10);
    
    // 工具栏
    QHBoxLayout* toolsLayout = new QHBoxLayout();
    
    m_addFilesBtn = new QPushButton("添加文件", this);
    m_addFilesBtn->setIcon(QIcon::fromTheme("list-add"));
    
    m_addFolderBtn = new QPushButton("添加文件夹", this);
    m_addFolderBtn->setIcon(QIcon::fromTheme("folder-new"));
    
    m_removeBtn = new QPushButton("移除选中", this);
    m_removeBtn->setObjectName("DangerButton");
    
    m_clearBtn = new QPushButton("清空列表", this);
    
    toolsLayout->addWidget(m_addFilesBtn);
    toolsLayout->addWidget(m_addFolderBtn);
    toolsLayout->addStretch();
    toolsLayout->addWidget(m_removeBtn);
    toolsLayout->addWidget(m_clearBtn);
    
    // 表格
    m_fileTable = new QTableWidget(this);
    m_fileTable->setColumnCount(3);
    m_fileTable->setHorizontalHeaderLabels({"名称", "大小", "路径"});
    m_fileTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_fileTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_fileTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_fileTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_fileTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_fileTable->setShowGrid(false);
    m_fileTable->setAlternatingRowColors(true);
    m_fileTable->verticalHeader()->setVisible(false);
    
    // 拖拽提示
    QLabel* dragTip = new QLabel("支持拖拽文件到此处", this);
    dragTip->setAlignment(Qt::AlignCenter);
    dragTip->setStyleSheet("color: #999999; font-style: italic; margin-top: 4px;");
    
    listLayout->addLayout(toolsLayout);
    listLayout->addWidget(m_fileTable);
    listLayout->addWidget(dragTip);
    
    m_mainLayout->addLayout(listLayout, 1);
}

void NewArchiveDialog::setupBottomPanel() {
    QGroupBox* settingsGroup = new QGroupBox("输出设置", this);
    QVBoxLayout* settingsLayout = new QVBoxLayout(settingsGroup);
    settingsLayout->setSpacing(12);
    
    // 路径选择
    QHBoxLayout* pathLayout = new QHBoxLayout();
    m_destPathEdit = new QLineEdit(this);
    m_destPathEdit->setPlaceholderText("选择保存位置...");
    
    // 设置默认路径
    QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    m_destPathEdit->setText(defaultPath + "/Archive.huff");
    
    m_browseBtn = new QPushButton("浏览...", this);
    
    pathLayout->addWidget(m_destPathEdit);
    pathLayout->addWidget(m_browseBtn);
    
    settingsLayout->addLayout(pathLayout);

    // 压缩选项
    QHBoxLayout* optionsLayout = new QHBoxLayout();
    m_useTreeMapCheck = new QCheckBox("使用 TreeMap (红黑树) 构建哈夫曼树", this);
    m_useTreeMapCheck->setToolTip("勾选后将使用 TreeMap 替代 HashMap 进行频率统计");
    optionsLayout->addWidget(m_useTreeMapCheck);
    optionsLayout->addStretch();
    settingsLayout->addLayout(optionsLayout);

    // 密码选项
    QHBoxLayout* passwordLayout = new QHBoxLayout();
    m_usePasswordCheck = new QCheckBox("使用密码加密", this);
    
    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setPlaceholderText("请输入密码");
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setVisible(false); // 默认隐藏
    
    m_showPasswordCheck = new QCheckBox("显示密码", this);
    m_showPasswordCheck->setVisible(false); // 默认隐藏
    
    connect(m_usePasswordCheck, &QCheckBox::toggled, [this](bool checked) {
        m_passwordEdit->setVisible(checked);
        m_showPasswordCheck->setVisible(checked);
        if (checked) {
            m_passwordEdit->setFocus();
        } else {
            m_passwordEdit->clear();
        }
    });
    
    connect(m_showPasswordCheck, &QCheckBox::toggled, [this](bool checked) {
        m_passwordEdit->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
    });

    passwordLayout->addWidget(m_usePasswordCheck);
    passwordLayout->addWidget(m_passwordEdit);
    passwordLayout->addWidget(m_showPasswordCheck);
    settingsLayout->addLayout(passwordLayout);
    
    // 底部按钮
    QHBoxLayout* actionLayout = new QHBoxLayout();
    actionLayout->setContentsMargins(0, 10, 0, 0);
    
    m_cancelBtn = new QPushButton("取消", this);
    m_cancelBtn->setMinimumWidth(100);
    
    m_compressBtn = new QPushButton("开始压缩", this);
    m_compressBtn->setObjectName("PrimaryButton");
    m_compressBtn->setMinimumWidth(120);
    m_compressBtn->setMinimumHeight(36);
    
    actionLayout->addStretch();
    actionLayout->addWidget(m_cancelBtn);
    actionLayout->addWidget(m_compressBtn);
    
    m_mainLayout->addWidget(settingsGroup);

    // 进度条区域
    QVBoxLayout* progressLayout = new QVBoxLayout();
    progressLayout->setSpacing(4);
    
    m_statusLabel = new QLabel(this);
    m_statusLabel->setStyleSheet("color: #666666; font-size: 12px;");
    m_statusLabel->setVisible(false);
    
    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->setTextVisible(true);
    m_progressBar->setVisible(false);
    m_progressBar->setStyleSheet(
        "QProgressBar { border: 1px solid #E0E0E0; border-radius: 4px; text-align: center; background: #FFFFFF; }"
        "QProgressBar::chunk { background-color: #0078D4; border-radius: 3px; }"
    );
    
    progressLayout->addWidget(m_statusLabel);
    progressLayout->addWidget(m_progressBar);
    m_mainLayout->addLayout(progressLayout);
    
    m_mainLayout->addLayout(actionLayout);
}

void NewArchiveDialog::setupConnections() {
    connect(m_addFilesBtn, &QPushButton::clicked, this, &NewArchiveDialog::onAddFiles);
    connect(m_addFolderBtn, &QPushButton::clicked, this, &NewArchiveDialog::onAddFolder);
    connect(m_removeBtn, &QPushButton::clicked, this, &NewArchiveDialog::onRemoveSelected);
    connect(m_clearBtn, &QPushButton::clicked, this, &NewArchiveDialog::onClearAll);
    connect(m_browseBtn, &QPushButton::clicked, this, &NewArchiveDialog::onBrowseDest);
    connect(m_compressBtn, &QPushButton::clicked, this, &NewArchiveDialog::onCompress);
    connect(m_cancelBtn, &QPushButton::clicked, this, &NewArchiveDialog::onCancelClicked);
}

void NewArchiveDialog::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void NewArchiveDialog::dropEvent(QDropEvent *event) {
    const QMimeData* mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        QList<QUrl> urlList = mimeData->urls();
        for (const QUrl& url : urlList) {
            QString path = url.toLocalFile();
            if (!path.isEmpty()) {
                m_selectedFiles.add(Structure::String(path.toUtf8().constData()));
            }
        }
        updateFileTable();
    }
}

void NewArchiveDialog::updateFileTable() {
    m_fileTable->setRowCount(0);
    
    for (int i = 0; i < m_selectedFiles.size(); ++i) {
        QString path = QString::fromUtf8(m_selectedFiles[i].c_str());
        QFileInfo info(path);
        
        int row = m_fileTable->rowCount();
        m_fileTable->insertRow(row);
        
        // 名称
        QTableWidgetItem* nameItem = new QTableWidgetItem(info.fileName());
        if (info.isDir()) {
            nameItem->setIcon(QIcon::fromTheme("folder"));
        } else {
            nameItem->setIcon(QIcon::fromTheme("text-x-generic"));
        }
        m_fileTable->setItem(row, 0, nameItem);
        
        // 大小
        QString sizeStr = info.isDir() ? QString("-") : QString(QString::number(info.size() / 1024.0, 'f', 1) + " KB");
        m_fileTable->setItem(row, 1, new QTableWidgetItem(sizeStr));
        
        // 路径
        m_fileTable->setItem(row, 2, new QTableWidgetItem(info.absolutePath()));
    }
}

void NewArchiveDialog::onAddFiles() {
    QStringList files = QFileDialog::getOpenFileNames(this, "选择文件", QString(), "所有文件 (*.*)");
    if (!files.isEmpty()) {
        for (const QString& file : files) {
            m_selectedFiles.add(Structure::String(file.toUtf8().constData()));
        }
        updateFileTable();
    }
}

void NewArchiveDialog::onAddFolder() {
    QString folder = QFileDialog::getExistingDirectory(this, "选择文件夹");
    if (!folder.isEmpty()) {
        m_selectedFiles.add(Structure::String(folder.toUtf8().constData()));
        updateFileTable();
    }
}

void NewArchiveDialog::onRemoveSelected() {
    QList<QTableWidgetItem*> items = m_fileTable->selectedItems();
    if (items.isEmpty()) return;
    
    // 获取选中的行（去重）
    QSet<int> rows;
    for (auto item : items) rows.insert(item->row());
    
    // 从后往前删除
    QList<int> sortedRows = rows.values();
    std::sort(sortedRows.begin(), sortedRows.end(), std::greater<int>());
    
    for (int row : sortedRows) {
        if (row < m_selectedFiles.size()) {
            m_selectedFiles.remove(row);
        }
    }
    updateFileTable();
}

void NewArchiveDialog::onClearAll() {
    m_selectedFiles.clear();
    updateFileTable();
}

void NewArchiveDialog::onBrowseDest() {
    QString file = QFileDialog::getSaveFileName(this, "保存为", m_destPathEdit->text(), "HUFF 归档 (*.huff)");
    if (!file.isEmpty()) {
        if (!file.endsWith(".huff")) file += ".huff";
        m_destPathEdit->setText(file);
    }
}

void NewArchiveDialog::onCompress() {
    if (m_selectedFiles.empty()) {
        QMessageBox::warning(this, "提示", "请先添加要压缩的文件或文件夹！");
        return;
    }
    
    QString destPath = m_destPathEdit->text();
    if (destPath.isEmpty()) {
        QMessageBox::warning(this, "提示", "请设置输出文件路径！");
        return;
    }
    
    Structure::String outputPath(destPath.toUtf8().constData());
    Structure::String password;
    if (m_usePasswordCheck->isChecked()) {
        QString pwd = m_passwordEdit->text();
        if (pwd.isEmpty()) {
            QMessageBox::warning(this, "提示", "请输入密码！");
            return;
        }
        password = Structure::String(pwd.toUtf8().constData());
    }
    
    // 更新 UI 状态
    m_isCompressing = true;
    m_compressBtn->setEnabled(false);
    m_cancelBtn->setText("停止");
    m_cancelBtn->setEnabled(true);
    m_addFilesBtn->setEnabled(false);
    m_addFolderBtn->setEnabled(false);
    m_removeBtn->setEnabled(false);
    m_clearBtn->setEnabled(false);
    m_destPathEdit->setEnabled(false);
    m_browseBtn->setEnabled(false);
    m_usePasswordCheck->setEnabled(false);
    m_passwordEdit->setEnabled(false);
    m_showPasswordCheck->setEnabled(false);
    
    m_progressBar->setValue(0);
    m_progressBar->setVisible(true);
    m_statusLabel->setText("正在准备压缩...");
    m_statusLabel->setVisible(true);
    
    // 创建线程和 Worker
    QThread* thread = new QThread;
    m_worker = new CompressionWorker(m_selectedFiles, outputPath, password);
    m_worker->moveToThread(thread);
    
    // 连接信号槽
    connect(thread, &QThread::started, m_worker, &CompressionWorker::process);
    connect(m_worker, &CompressionWorker::progress, this, &NewArchiveDialog::onCompressionProgress);
    connect(m_worker, &CompressionWorker::finished, this, [this, thread](bool success, QString msg) {
        // 清理线程
        thread->quit();
        thread->wait();
        m_worker->deleteLater();
        m_worker = nullptr;
        thread->deleteLater();
        
        onCompressionFinished(success, msg);
    });
    
    // 开始执行
    thread->start();
}

void NewArchiveDialog::onCancelClicked() {
    if (m_isCompressing) {
        if (m_worker) {
            m_worker->cancel();
            m_statusLabel->setText("正在停止...");
            m_cancelBtn->setEnabled(false); // 防止重复点击
        }
    } else {
        reject();
    }
}

void NewArchiveDialog::onCompressionProgress(int value, QString message) {
    m_progressBar->setValue(value);
    m_statusLabel->setText(message);
}

void NewArchiveDialog::onCompressionFinished(bool success, QString msg) {
    m_isCompressing = false;
    
    // 恢复 UI 状态
    m_compressBtn->setEnabled(true);
    m_cancelBtn->setText("取消");
    m_cancelBtn->setEnabled(true);
    m_addFilesBtn->setEnabled(true);
    m_addFolderBtn->setEnabled(true);
    m_removeBtn->setEnabled(true);
    m_clearBtn->setEnabled(true);
    m_destPathEdit->setEnabled(true);
    m_browseBtn->setEnabled(true);
    m_usePasswordCheck->setEnabled(true);
    m_passwordEdit->setEnabled(true);
    m_showPasswordCheck->setEnabled(true);
    
    m_progressBar->setVisible(false);
    m_statusLabel->setVisible(false);
    
    if (success) {
        QMessageBox::information(this, "成功", "压缩已完成！");
        accept();
    } else {
        // 如果是用户取消，可能不需要弹窗报错，或者提示已取消
        if (msg.contains("cancelled")) {
             QMessageBox::information(this, "提示", "压缩已取消。");
        } else {
             QMessageBox::critical(this, "失败", QString("压缩失败：%1").arg(msg));
        }
    }
}

Structure::String NewArchiveDialog::getArchivePath() const {
    return Structure::String(m_destPathEdit->text().toUtf8().constData());
}

Structure::ArrayList<Structure::String> NewArchiveDialog::getFilesToCompress() const {
    return m_selectedFiles;
}

bool NewArchiveDialog::useTreeMap() const {
    return m_useTreeMapCheck->isChecked();
}

}
#include "NewArchiveDialog.moc"
