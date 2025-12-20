#include "view/AboutDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFont>

namespace View {

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("关于"));
    setModal(true);
    setMinimumWidth(500);
    setMinimumHeight(350);
    
    setupUI();
    setupConnections();
}

AboutDialog::~AboutDialog()
{
}

void AboutDialog::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(15);
    m_mainLayout->setContentsMargins(20, 20, 20, 20);
    
    // 标题
    m_titleLabel = new QLabel(tr("HuffArchive"), this);
    QFont titleFont = m_titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    m_titleLabel->setFont(titleFont);
    m_mainLayout->addWidget(m_titleLabel);
    
    // 版本号
    m_versionLabel = new QLabel(tr("版本 2.0.0"), this);
    QFont versionFont = m_versionLabel->font();
    versionFont.setPointSize(11);
    m_versionLabel->setFont(versionFont);
    m_mainLayout->addWidget(m_versionLabel);
    
    // 分割线
    QFrame* line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    m_mainLayout->addWidget(line);
    
    // 描述
    m_descriptionLabel = new QLabel(
        tr("基于 Huffman 编码算法的文件压缩工具。\n"
           "对bandzip的拙劣模仿。"),
        this
    );
    m_descriptionLabel->setWordWrap(true);
    m_mainLayout->addWidget(m_descriptionLabel);
    
    // 主要功能
    m_featuresLabel = new QLabel(
        tr("<b>主要功能:</b><br/>"
           "• 使用 Huffman 编码压缩文件<br/>"
           "• 解压 .huff 格式的文件<br/>"
           "• 查看压缩文件的统计信息<br/>"
           "• 支持密码保护<br/>"
           "• 自我感觉用户友好的图形界面<br/>"
           "• Ciallo~ (∠・ω< )⌒★"),
        this
    );
    m_featuresLabel->setWordWrap(true);
    m_mainLayout->addWidget(m_featuresLabel);
    
    // 版权信息
    m_copyrightLabel = new QLabel(
        tr("🄯 2025 HuffArchive Project\n"
           "采用 GPLv3 协议发布\n"),
        this
    );
    QFont copyrightFont = m_copyrightLabel->font();
    copyrightFont.setPointSize(9);
    m_copyrightLabel->setFont(copyrightFont);
    m_copyrightLabel->setAlignment(Qt::AlignCenter);
    m_mainLayout->addWidget(m_copyrightLabel);
    
    // 添加弹性空间
    m_mainLayout->addStretch();
    
    // 关闭按钮
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    m_closeButton = new QPushButton(tr("关闭"), this);
    m_closeButton->setMinimumWidth(100);
    buttonLayout->addWidget(m_closeButton);
    buttonLayout->addStretch();
    m_mainLayout->addLayout(buttonLayout);
    
    setLayout(m_mainLayout);
}

void AboutDialog::setupConnections()
{
    connect(m_closeButton, &QPushButton::clicked, this, &QDialog::close);
}

}
