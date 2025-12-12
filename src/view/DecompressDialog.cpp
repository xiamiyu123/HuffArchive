#include "view/DecompressDialog.h"
#include <QFileDialog>
#include <QStandardPaths>
#include <QIcon>
#include <QFileInfo>

namespace View {

DecompressDialog::DecompressDialog(const QString& archiveName, QWidget *parent)
    : QDialog(parent), m_archiveName(archiveName)
{
    setupUI();
    setupConnections();
}

DecompressDialog::~DecompressDialog() {
}

void DecompressDialog::setupUI() {
    setWindowTitle("解压文件");
    setMinimumSize(600, 350);
    resize(600, 350);
    
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(20);
    m_mainLayout->setContentsMargins(24, 24, 24, 24);
    
    setupHeader();
    setupSettings();
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
    );
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
    QGroupBox* settingsGroup = new QGroupBox("解压选项", this);
    QVBoxLayout* settingsLayout = new QVBoxLayout(settingsGroup);
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
    
    settingsLayout->addWidget(m_openFolderCheck);
    settingsLayout->addWidget(m_overwriteCheck);
    settingsLayout->addStretch();
    
    m_mainLayout->addWidget(settingsGroup, 1);
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
    connect(m_cancelBtn, &QPushButton::clicked, this, &DecompressDialog::reject);
}

void DecompressDialog::onBrowseDest() {
    QString dir = QFileDialog::getExistingDirectory(this, "选择解压目标文件夹", m_destPathEdit->text());
    if (!dir.isEmpty()) {
        m_destPathEdit->setText(dir);
    }
}

void DecompressDialog::onExtract() {
    // TODO: Implement extraction logic
    accept();
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
