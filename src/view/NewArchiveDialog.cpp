#include "view/NewArchiveDialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QHeaderView>

namespace View {

NewArchiveDialog::NewArchiveDialog(QWidget *parent)
    : QDialog(parent),
      m_mainLayout(nullptr),
      m_fileListGroup(nullptr),
      m_fileList(nullptr),
      m_addButton(nullptr),
      m_addFolderButton(nullptr),
      m_deleteButton(nullptr),
      m_settingsGroup(nullptr),
      m_archivePathEdit(nullptr),
      m_browseButton(nullptr),
      m_formatCombo(nullptr),
      m_moreOptionsCheckBox(nullptr),
      m_startButton(nullptr),
      m_cancelButton(nullptr)
{
    setupUI();
    setupConnections();
}

NewArchiveDialog::~NewArchiveDialog() {
}

void NewArchiveDialog::setupUI() {
    setWindowTitle("新建压缩文件");
    setMinimumSize(900, 600);
    
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(10);
    m_mainLayout->setContentsMargins(10, 10, 10, 10);
    
    // 设置文件列表区域
    setupFileListSection();
    
    // 设置压缩文件设置区域
    setupArchiveSettingsSection();
    
    // 创建底部按钮布局
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    m_startButton = new QPushButton("开始(S)", this);
    m_startButton->setMinimumWidth(100);
    m_startButton->setDefault(true);
    
    m_cancelButton = new QPushButton("取消", this);
    m_cancelButton->setMinimumWidth(100);
    
    buttonLayout->addWidget(m_startButton);
    buttonLayout->addWidget(m_cancelButton);
    
    m_mainLayout->addLayout(buttonLayout);
    
    // 设置对话框样式
    setStyleSheet(
        "QDialog {"
        "   background-color: white;"
        "}"
        "QGroupBox {"
        "   font-weight: bold;"
        "   border: 1px solid #D0D0D0;"
        "   border-radius: 4px;"
        "   margin-top: 8px;"
        "   padding-top: 8px;"
        "}"
        "QGroupBox::title {"
        "   subcontrol-origin: margin;"
        "   subcontrol-position: top left;"
        "   padding: 0 5px;"
        "   color: #333;"
        "}"
        "QPushButton {"
        "   background-color: #F0F0F0;"
        "   border: 1px solid #C0C0C0;"
        "   border-radius: 3px;"
        "   padding: 5px 15px;"
        "   min-height: 24px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #E5F3FF;"
        "   border-color: #0078D7;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #CCE8FF;"
        "}"
        "QPushButton:default {"
        "   border: 2px solid #0078D7;"
        "}"
        "QLineEdit {"
        "   border: 1px solid #C0C0C0;"
        "   border-radius: 3px;"
        "   padding: 4px;"
        "   background-color: white;"
        "}"
        "QLineEdit:focus {"
        "   border-color: #0078D7;"
        "}"
        "QListWidget {"
        "   border: 1px solid #C0C0C0;"
        "   background-color: white;"
        "}"
        "QComboBox {"
        "   border: 1px solid #C0C0C0;"
        "   border-radius: 3px;"
        "   padding: 4px;"
        "   background-color: white;"
        "}"
    );
}

void NewArchiveDialog::setupFileListSection() {
    m_fileListGroup = new QGroupBox("添加文件到压缩文件", this);
    QVBoxLayout* groupLayout = new QVBoxLayout(m_fileListGroup);
    
    // 创建文件列表
    m_fileList = new QListWidget(this);
    m_fileList->setMinimumHeight(250);
    m_fileList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    
    // 添加表头标签
    QHBoxLayout* headerLayout = new QHBoxLayout();
    QLabel* nameLabel = new QLabel("名称", this);
    QLabel* sizeLabel = new QLabel("大小", this);
    QLabel* pathLabel = new QLabel("文件夹路径", this);
    
    nameLabel->setMinimumWidth(200);
    sizeLabel->setMinimumWidth(100);
    pathLabel->setMinimumWidth(300);
    
    QFont headerFont;
    headerFont.setBold(true);
    nameLabel->setFont(headerFont);
    sizeLabel->setFont(headerFont);
    pathLabel->setFont(headerFont);
    
    headerLayout->addWidget(nameLabel);
    headerLayout->addWidget(sizeLabel);
    headerLayout->addWidget(pathLabel);
    headerLayout->addStretch();
    
    groupLayout->addLayout(headerLayout);
    groupLayout->addWidget(m_fileList);
    
    // 创建按钮布局
    QHBoxLayout* fileButtonLayout = new QHBoxLayout();
    
    m_addButton = new QPushButton("添加文件(A)", this);
    m_addFolderButton = new QPushButton("添加文件夹(F)", this);
    m_deleteButton = new QPushButton("删除(D)", this);
    
    fileButtonLayout->addWidget(m_addButton);
    fileButtonLayout->addWidget(m_addFolderButton);
    fileButtonLayout->addWidget(m_deleteButton);
    fileButtonLayout->addStretch();
    
    groupLayout->addLayout(fileButtonLayout);
    
    m_mainLayout->addWidget(m_fileListGroup);
}

void NewArchiveDialog::setupArchiveSettingsSection() {
    m_settingsGroup = new QGroupBox("压缩文件设置", this);
    QVBoxLayout* groupLayout = new QVBoxLayout(m_settingsGroup);
    
    // 文件名输入
    QHBoxLayout* fileNameLayout = new QHBoxLayout();
    QLabel* fileNameLabel = new QLabel("文件名", this);
    fileNameLabel->setMinimumWidth(80);
    
    m_archivePathEdit = new QLineEdit(this);
    m_archivePathEdit->setPlaceholderText("C:\\Users\\xiami\\Documents\\新建压缩文件.huff");
    
    // 设置默认路径
    QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    m_archivePathEdit->setText(defaultPath + "/新建压缩文件.huff");
    
    m_browseButton = new QPushButton("浏览(B)...", this);
    
    fileNameLayout->addWidget(fileNameLabel);
    fileNameLayout->addWidget(m_archivePathEdit);
    fileNameLayout->addWidget(m_browseButton);
    
    groupLayout->addLayout(fileNameLayout);
    
    // 保存类型选择
    QHBoxLayout* formatLayout = new QHBoxLayout();
    QLabel* formatLabel = new QLabel("保存类型", this);
    formatLabel->setMinimumWidth(80);
    
    m_formatCombo = new QComboBox(this);
    m_formatCombo->addItem("HUFF");
    m_formatCombo->setEnabled(false); // 只有一种格式，禁用选择
    
    formatLayout->addWidget(formatLabel);
    formatLayout->addWidget(m_formatCombo);
    formatLayout->addStretch();
    
    groupLayout->addLayout(formatLayout);
    
    // 更多选项复选框
    m_moreOptionsCheckBox = new QCheckBox("更多选项...", this);
    groupLayout->addWidget(m_moreOptionsCheckBox);
    
    m_mainLayout->addWidget(m_settingsGroup);
}

void NewArchiveDialog::setupConnections() {
    connect(m_addButton, &QPushButton::clicked, this, &NewArchiveDialog::onAddFiles);
    connect(m_addFolderButton, &QPushButton::clicked, this, &NewArchiveDialog::onAddFolder);
    connect(m_deleteButton, &QPushButton::clicked, this, &NewArchiveDialog::onDeleteFiles);
    connect(m_browseButton, &QPushButton::clicked, this, &NewArchiveDialog::onBrowse);
    connect(m_startButton, &QPushButton::clicked, this, &NewArchiveDialog::onStart);
    connect(m_cancelButton, &QPushButton::clicked, this, &NewArchiveDialog::onCancel);
}

Structure::String NewArchiveDialog::getArchivePath() const {
    return Structure::String(m_archivePathEdit->text().toStdString().c_str());
}

Structure::ArrayList<Structure::String> NewArchiveDialog::getFilesToCompress() const {
    return m_selectedFiles;
}
void NewArchiveDialog::onAddFiles() {
    // 打开文件选择对话框（支持多选）
    QStringList fileNames = QFileDialog::getOpenFileNames(
        this,
        "选择要添加的文件",
        QString(),
        "所有文件 (*.*)"
    );
    
    if (!fileNames.isEmpty()) {
        for (const QString& fileName : fileNames) {
            // 添加到列表
            QFileInfo fileInfo(fileName);
            
            // 创建列表项
            QString itemText = QString("%1\t%2\t%3")
                .arg(fileInfo.fileName(), -30)
                .arg(QString::number(fileInfo.size()), -15)
                .arg(fileInfo.absolutePath());
            
            m_fileList->addItem(itemText);
            
            // 添加到内部列表
            m_selectedFiles.add(Structure::String(fileName.toStdString().c_str()));
        }
    }
}

void NewArchiveDialog::onAddFolder() {
    // 打开文件夹选择对话框
    QString folderPath = QFileDialog::getExistingDirectory(
        this,
        "选择要添加的文件夹",
        QString(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );
    
    if (!folderPath.isEmpty()) {
        // 获取文件夹信息
        QFileInfo folderInfo(folderPath);
        
        // 创建列表项（文件夹显示为特殊项）
        QString itemText = QString("[文件夹] %1\t\t%2")
            .arg(folderInfo.fileName(), -30)
            .arg(folderInfo.absolutePath());
        
        m_fileList->addItem(itemText);
        
        // 添加到内部列表
        m_selectedFiles.add(Structure::String(folderPath.toStdString().c_str()));
    }
}

void NewArchiveDialog::onDeleteFiles() {
    // 获取选中的项
    QList<QListWidgetItem*> selectedItems = m_fileList->selectedItems();
    
    if (selectedItems.isEmpty()) {
        QMessageBox::information(this, "提示", "请先选择要删除的文件");
        return;
    }
    
    // 从后往前删除，避免索引问题
    for (int i = selectedItems.size() - 1; i >= 0; --i) {
        int row = m_fileList->row(selectedItems[i]);
        delete m_fileList->takeItem(row);
        
        // 同时从内部列表删除
        if (row < m_selectedFiles.size()) {
            m_selectedFiles.remove(row);
        }
    }
}

void NewArchiveDialog::onBrowse() {
    // 打开保存文件对话框
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "选择压缩文件保存位置",
        m_archivePathEdit->text(),
        "HUFF 压缩文件 (*.huff);;所有文件 (*.*)"
    );
    
    if (!fileName.isEmpty()) {
        // 确保文件扩展名为 .huff
        if (!fileName.endsWith(".huff", Qt::CaseInsensitive)) {
            fileName += ".huff";
        }
        m_archivePathEdit->setText(fileName);
    }
}
void NewArchiveDialog::onStart() {
    // 验证输入
    if (m_selectedFiles.empty()) {
        QMessageBox::warning(this, "错误", "请至少添加一个文件");
        return;
    }
    
    if (m_archivePathEdit->text().isEmpty()) {
        QMessageBox::warning(this, "错误", "请指定压缩文件保存路径");
        return;
    }
    
    // 接受对话框
    accept();
}

void NewArchiveDialog::onCancel() {
    reject();
}

}
