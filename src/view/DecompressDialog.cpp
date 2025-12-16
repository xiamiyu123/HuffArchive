#include "view/DecompressDialog.h"
#include "command/DecompressDirectoryCommand.h"
#include "command/SelectiveDecompressCommand.h"
#include <QFileDialog>
#include <QStandardPaths>
#include <QIcon>
#include <QFileInfo>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>

namespace View {

void DecompressionWorker::process() {
    bool success = false;
    Structure::String errorMessage;

    auto progressCallback = [this](float progress) {
        emit progressUpdated(static_cast<int>(progress * 100));
    };

    auto cancelCallback = [this]() -> bool {
        return m_isCancelled.load();
    };

    if (m_isSelective) {
        Command::SelectiveDecompressCommand cmd(m_archivePath, m_destPath, m_filesToExtract, m_password);
        cmd.setProgressCallback(progressCallback);
        cmd.setCheckCancelCallback(cancelCallback);
        success = cmd.execute();
        if (!success) errorMessage = cmd.getErrorMessage();
    } else {
        Command::DecompressDirectoryCommand cmd(m_archivePath, m_destPath, m_password);
        cmd.setProgressCallback(progressCallback);
        cmd.setCheckCancelCallback(cancelCallback);
        success = cmd.execute();
        if (!success) errorMessage = cmd.getErrorMessage();
    }

    if (m_isCancelled) {
        emit finished(false, "已停止解压");
    } else {
        emit finished(success, success ? "解压完成" : QString(errorMessage.c_str()));
    }
}

DecompressDialog::DecompressDialog(const QString& archiveName, QWidget *parent)
    : QDialog(parent), m_archiveName(archiveName), m_workerThread(nullptr), m_worker(nullptr), m_isDecompressing(false)
{
    setupUI();
    setupConnections();
}

DecompressDialog::~DecompressDialog() {
    if (m_workerThread) {
        m_workerThread->quit();
        m_workerThread->wait();
        delete m_workerThread;
        m_workerThread = nullptr;
    }
}

void DecompressDialog::setFilesToExtract(const Structure::ArrayList<Structure::String>& files) {
    m_filesToExtract = files;
}

void DecompressDialog::setupUI() {
    setWindowTitle("解压文件");
    setMinimumSize(800, 500);
    
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(20);
    m_mainLayout->setContentsMargins(24, 24, 24, 24);
    
    setupHeader();
    setupSettings();
    
    // Progress Bar (Initially Hidden)
    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->setTextVisible(true);
    m_progressBar->setFixedHeight(24);
    m_progressBar->hide();
    m_mainLayout->addWidget(m_progressBar);

    m_statusLabel = new QLabel(this);
    m_statusLabel->setStyleSheet("color: #666666;");
    m_statusLabel->hide();
    m_mainLayout->addWidget(m_statusLabel);
    
    setupBottomPanel();
    
    // Apply Styles (Consistent with NewArchiveDialog)
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
        "QCheckBox { spacing: 8px; color: #333333; }"
        "QCheckBox::indicator { width: 18px; height: 18px; border: 1px solid #E0E0E0; border-radius: 3px; background: #FAFAFA; }"
        "QCheckBox::indicator:checked { background: #0078D4; border-color: #0078D4; }"
        "QGroupBox { border: 1px solid #E0E0E0; border-radius: 4px; margin-top: 12px; padding-top: 24px; font-weight: bold; color: #666666; }"
        "QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; padding: 0 5px; left: 10px; }"
        "QProgressBar { border: 1px solid #E0E0E0; border-radius: 4px; text-align: center; background: #FAFAFA; }"
        "QProgressBar::chunk { background-color: #0078D4; border-radius: 3px; }"
    );
    
    // 自适应大小以显示所有内容
    adjustSize();
}

void DecompressDialog::setupHeader() {
    QVBoxLayout* headerLayout = new QVBoxLayout();
    headerLayout->setSpacing(4);
    
    m_titleLabel = new QLabel("解压归档文件", this);
    QFont titleFont = font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    m_titleLabel->setFont(titleFont);
    
    m_subtitleLabel = new QLabel(QString("正在解压: %1").arg(QFileInfo(m_archiveName).fileName()), this);
    m_subtitleLabel->setStyleSheet("color: #666666;");
    
    headerLayout->addWidget(m_titleLabel);
    headerLayout->addWidget(m_subtitleLabel);
    
    m_mainLayout->addLayout(headerLayout);
}

void DecompressDialog::setupSettings() {
    m_settingsGroup = new QGroupBox("解压选项", this);
    QVBoxLayout* settingsLayout = new QVBoxLayout(m_settingsGroup);
    settingsLayout->setSpacing(16);
    settingsLayout->setContentsMargins(16, 24, 16, 16);
    
    // Path Selection
    QVBoxLayout* pathGroup = new QVBoxLayout();
    pathGroup->setSpacing(8);
    QLabel* pathLabel = new QLabel("目标文件夹:", this);
    
    QHBoxLayout* pathInputLayout = new QHBoxLayout();
    m_destPathEdit = new QLineEdit(this);
    
    // Default to a folder with the archive name in the same directory or Documents
    QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/" + QFileInfo(m_archiveName).baseName();
    m_destPathEdit->setText(defaultPath);
    
    m_browseBtn = new QPushButton("浏览...", this);
    
    pathInputLayout->addWidget(m_destPathEdit);
    pathInputLayout->addWidget(m_browseBtn);
    
    pathGroup->addWidget(pathLabel);
    pathGroup->addLayout(pathInputLayout);
    
    settingsLayout->addLayout(pathGroup);
    
    // Checkboxes
    m_openFolderCheck = new QCheckBox("解压完成后打开文件夹", this);
    m_openFolderCheck->setChecked(true);
    
    m_overwriteCheck = new QCheckBox("覆盖已存在的文件", this);
    
    m_useTreeMapCheck = new QCheckBox("使用 TreeMap (红黑树) 重建哈夫曼树", this);
    m_useTreeMapCheck->setToolTip("勾选后将使用 TreeMap 替代 HashMap 进行频率统计与树构建");

    settingsLayout->addWidget(m_openFolderCheck);
    settingsLayout->addWidget(m_overwriteCheck);
    settingsLayout->addWidget(m_useTreeMapCheck);
    settingsLayout->addStretch();
    
    m_mainLayout->addWidget(m_settingsGroup, 1);
}

void DecompressDialog::setupBottomPanel() {
    QHBoxLayout* actionLayout = new QHBoxLayout();
    actionLayout->setContentsMargins(0, 10, 0, 0);
    
    m_cancelBtn = new QPushButton("取消", this);
    m_cancelBtn->setMinimumWidth(100);
    
    m_extractBtn = new QPushButton("立即解压", this);
    m_extractBtn->setObjectName("PrimaryButton");
    m_extractBtn->setMinimumWidth(120);
    m_extractBtn->setMinimumHeight(36);
    
    actionLayout->addStretch();
    actionLayout->addWidget(m_cancelBtn);
    actionLayout->addWidget(m_extractBtn);
    
    m_mainLayout->addLayout(actionLayout);
}

void DecompressDialog::setupConnections() {
    connect(m_browseBtn, &QPushButton::clicked, this, &DecompressDialog::onBrowseDest);
    connect(m_extractBtn, &QPushButton::clicked, this, &DecompressDialog::onExtract);
    connect(m_cancelBtn, &QPushButton::clicked, this, &DecompressDialog::onCancelClicked);
}

void DecompressDialog::onBrowseDest() {
    QString dir = QFileDialog::getExistingDirectory(this, "选择解压目标文件夹", m_destPathEdit->text());
    if (!dir.isEmpty()) {
        m_destPathEdit->setText(dir);
    }
}

void DecompressDialog::onExtract() {
    if (m_isDecompressing) return;

    // Validate
    QString destPath = m_destPathEdit->text();
    if (destPath.isEmpty()) {
        QMessageBox::warning(this, "警告", "请选择目标文件夹");
        return;
    }

    // UI State Change
    m_isDecompressing = true;
    m_settingsGroup->setEnabled(false);
    m_extractBtn->setEnabled(false);
    m_cancelBtn->setText("停止");
    m_progressBar->show();
    m_statusLabel->show();
    m_statusLabel->setText("正在准备解压...");

    // Start Thread
    m_workerThread = new QThread;
    m_worker = new DecompressionWorker(
        Structure::String(m_archiveName.toStdString().c_str()),
        Structure::String(destPath.toStdString().c_str()),
        m_password,
        m_filesToExtract
    );
    m_worker->setUseTreeMap(m_useTreeMapCheck->isChecked());
    m_worker->moveToThread(m_workerThread);

    connect(m_workerThread, &QThread::started, m_worker, &DecompressionWorker::process);
    connect(m_worker, &DecompressionWorker::progressUpdated, this, &DecompressDialog::updateProgress);
    connect(m_worker, &DecompressionWorker::finished, this, &DecompressDialog::onDecompressionFinished);
    connect(m_worker, &DecompressionWorker::finished, m_workerThread, &QThread::quit);
    connect(m_worker, &DecompressionWorker::finished, m_worker, &DecompressionWorker::deleteLater);
    // connect(m_workerThread, &QThread::finished, m_workerThread, &QThread::deleteLater); // Removed to prevent double deletion/crash

    m_workerThread->start();
}

void DecompressDialog::onCancelClicked() {
    if (m_isDecompressing) {
        if (m_worker) {
            m_worker->stop();
            m_statusLabel->setText("正在停止...");
            m_cancelBtn->setEnabled(false);
        }
    } else {
        reject();
    }
}

void DecompressDialog::updateProgress(int percentage) {
    m_progressBar->setValue(percentage);
    m_statusLabel->setText(QString("正在解压... %1%").arg(percentage));
}

void DecompressDialog::onDecompressionFinished(bool success, QString message) {
    m_isDecompressing = false;
    m_settingsGroup->setEnabled(true);
    m_extractBtn->setEnabled(true);
    m_cancelBtn->setText("关闭");
    m_cancelBtn->setEnabled(true);
    m_statusLabel->setText(message);

    if (success) {
        m_progressBar->setValue(100);
        QMessageBox::information(this, "完成", message);
        
        if (shouldOpenFolder()) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(m_destPathEdit->text()));
        }
        accept();
    } else {
        if (message == "已停止解压") {
             // Stay open or close? Usually stay open to let user try again or close.
             // Reset UI
             m_progressBar->hide();
             m_statusLabel->hide();
             m_cancelBtn->setText("取消");
        } else {
            QMessageBox::critical(this, "错误", message);
        }
    }
}

Structure::String DecompressDialog::getDestinationPath() const {
    return Structure::String(m_destPathEdit->text().toStdString().c_str());
}

bool DecompressDialog::shouldOpenFolder() const {
    return m_openFolderCheck->isChecked();
}

bool DecompressDialog::shouldOverwrite() const {
    return m_overwriteCheck->isChecked();
}

}
