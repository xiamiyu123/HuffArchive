#include "view/ArchivePropertiesDialog.h"
#include <QDialogButtonBox>
#include <QFileInfo>
#include <QDateTime>
#include <fstream>
#include <filesystem>

namespace View {

ArchivePropertiesDialog::ArchivePropertiesDialog(const Structure::String& archivePath, QWidget *parent)
    : QDialog(parent), m_archivePath(archivePath) {
    setWindowTitle("压缩包属性");
    setModal(true);
    resize(400, 300);
    setupUI();
    loadProperties();
}

ArchivePropertiesDialog::~ArchivePropertiesDialog() {
}

void ArchivePropertiesDialog::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // 属性组
    QGroupBox* propertiesGroup = new QGroupBox("基本信息", this);
    QFormLayout* formLayout = new QFormLayout(propertiesGroup);
    
    m_nameLabel = new QLabel(this);
    m_sizeLabel = new QLabel(this);
    m_fileCountLabel = new QLabel(this);
    m_compressionRatioLabel = new QLabel(this);
    m_createdTimeLabel = new QLabel(this);
    
    formLayout->addRow("名称:", m_nameLabel);
    formLayout->addRow("大小:", m_sizeLabel);
    formLayout->addRow("文件数量:", m_fileCountLabel);
    formLayout->addRow("压缩率:", m_compressionRatioLabel);
    formLayout->addRow("创建时间:", m_createdTimeLabel);
    
    mainLayout->addWidget(propertiesGroup);
    
    // 按钮
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    mainLayout->addWidget(buttonBox);
}

void ArchivePropertiesDialog::loadProperties() {
    // 获取文件基本信息
    QFileInfo fileInfo(QString::fromStdString(m_archivePath.c_str()));
    m_nameLabel->setText(fileInfo.fileName());
    m_sizeLabel->setText(QString::number(fileInfo.size() / 1024.0, 'f', 1) + " KB");
    m_createdTimeLabel->setText(fileInfo.birthTime().toString("yyyy-MM-dd hh:mm:ss"));
    
    // 读取压缩包内部信息
    std::ifstream inFile(std::filesystem::path(reinterpret_cast<const char8_t*>(m_archivePath.c_str())), std::ios::binary);
    if (!inFile) {
        m_fileCountLabel->setText("无法读取");
        m_compressionRatioLabel->setText("无法读取");
        return;
    }
    
    // 验证魔数
    char magic[5] = {0};
    inFile.read(magic, 4);
    if (std::string(magic) != "HUFF") {
        m_fileCountLabel->setText("无效格式");
        m_compressionRatioLabel->setText("无效格式");
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
    m_fileCountLabel->setText(QString::number(fileCount));
    
    // 计算压缩率：需要读取所有文件的原始大小和压缩大小
    long long totalOriginalSize = 0;
    long long totalCompressedSize = 0;
    
    for (int i = 0; i < fileCount; ++i) {
        // 读取路径长度并跳过
        int pathLen;
        inFile.read(reinterpret_cast<char*>(&pathLen), sizeof(int));
        inFile.seekg(pathLen, std::ios::cur);
        
        // 读取大小信息
        long long originalSize, compressedSize, offset;
        inFile.read(reinterpret_cast<char*>(&originalSize), sizeof(long long));
        inFile.read(reinterpret_cast<char*>(&compressedSize), sizeof(long long));
        inFile.read(reinterpret_cast<char*>(&offset), sizeof(long long));
        
        totalOriginalSize += originalSize;
        totalCompressedSize += compressedSize;
    }
    
    inFile.close();
    
    if (totalOriginalSize > 0) {
        double ratio = (1.0 - static_cast<double>(totalCompressedSize) / totalOriginalSize) * 100.0;
        m_compressionRatioLabel->setText(QString::number(ratio, 'f', 1) + "%");
    } else {
        m_compressionRatioLabel->setText("0%");
    }
}

} // namespace View
