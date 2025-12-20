#include "view/TestView.h"
#include "structure/HuffmanTree.h"
#include "structure/HashMap.h"
#include <QElapsedTimer>
#include <QApplication>
#include <random>
#include <vector>

namespace View {

TestView::TestView(QWidget *parent) : QWidget(parent) {
    setupUI();
}

TestView::~TestView() {}

void TestView::setupUI() {
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(40, 40, 40, 40);
    m_mainLayout->setSpacing(20);

    m_titleLabel = new QLabel("性能测试与诊断", this);
    QFont titleFont = m_titleLabel->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    m_titleLabel->setFont(titleFont);
    m_mainLayout->addWidget(m_titleLabel);

    QLabel* descLabel = new QLabel("在此页面您可以测试哈夫曼算法在当前硬件上的性能表现。", this);
    descLabel->setStyleSheet("color: #5F6368;");
    m_mainLayout->addWidget(descLabel);

    QHBoxLayout* ctrlLayout = new QHBoxLayout();
    m_runBtn = new QPushButton("开始随机数据压缩测试 (5MB)", this);
    m_runBtn->setMinimumHeight(40);
    m_runBtn->setCursor(Qt::PointingHandCursor);
    m_runBtn->setStyleSheet(
        "QPushButton { "
        "   background: #1A73E8; "
        "   color: white; "
        "   border-radius: 6px; "
        "   padding: 0 20px; "
        "   font-weight: bold; "
        "   font-size: 14px;"
        "}"
        "QPushButton:hover { background: #1557B0; }"
        "QPushButton:pressed { background: #174EA6; }"
        "QPushButton:disabled { background: #E0E0E0; color: #9E9E9E; }"
    );
    
    m_backBtn = new QPushButton("返回主页", this);
    m_backBtn->setMinimumHeight(40);
    m_backBtn->setCursor(Qt::PointingHandCursor);
    m_backBtn->setStyleSheet(
        "QPushButton { "
        "   border: 1px solid #DADCE0; "
        "   border-radius: 6px; "
        "   padding: 0 20px; "
        "   background: #FFFFFF; "
        "   color: #3C4043; "
        "   font-weight: 500;"
        "}"
        "QPushButton:hover { background: #F8F9FA; }"
    );
    
    ctrlLayout->addWidget(m_runBtn);
    ctrlLayout->addWidget(m_backBtn);
    ctrlLayout->addStretch();
    m_mainLayout->addLayout(ctrlLayout);

    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->setFixedHeight(8);
    m_progressBar->setTextVisible(false);
    m_progressBar->setStyleSheet(
        "QProgressBar { border: none; background: #E0E0E0; border-radius: 4px; }"
        "QProgressBar::chunk { background: #1A73E8; border-radius: 4px; }"
    );
    m_progressBar->hide();
    m_mainLayout->addWidget(m_progressBar);

    m_resultArea = new QTextEdit(this);
    m_resultArea->setReadOnly(true);
    m_resultArea->setPlaceholderText("测试结果将显示在这里...");
    m_resultArea->setStyleSheet(
        "QTextEdit { "
        "   background: #F8F9FA; "
        "   border: 1px solid #E0E0E0; "
        "   border-radius: 8px; "
        "   padding: 15px; "
        "   font-family: 'Consolas', 'Courier New', monospace; "
        "   font-size: 13px; "
        "   line-height: 1.5;"
        "}"
    );
    m_mainLayout->addWidget(m_resultArea);

    connect(m_runBtn, &QPushButton::clicked, this, &TestView::onRunSpeedTest);
    connect(m_backBtn, &QPushButton::clicked, this, &TestView::backRequested);
}

void TestView::onRunSpeedTest() {
    m_runBtn->setEnabled(false);
    m_resultArea->clear();
    m_progressBar->show();
    m_progressBar->setValue(0);
    
    const int testSize = 5 * 1024 * 1024;
    m_resultArea->append(QString("<span style='color: #1A73E8;'>[1/4]</span> 正在生成 %1 字节的随机数据...").arg(testSize));
    QApplication::processEvents();

    std::vector<unsigned char> data(testSize);
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dis(0, 255);
    for (int i = 0; i < testSize; ++i) {
        data[i] = static_cast<unsigned char>(dis(gen));
    }
    
    m_progressBar->setValue(30);
    m_resultArea->append("<span style='color: #1A73E8;'>[2/4]</span> 数据生成完毕，开始频率统计...");
    QApplication::processEvents();

    QElapsedTimer timer;
    timer.start();

    // 1. 频率统计
    Structure::HashMap<unsigned char, int> freqMap;
    for (unsigned char c : data) {
        if (freqMap.contains(c)) freqMap[c]++;
        else freqMap.put(c, 1);
    }
    qint64 freqTime = timer.elapsed();
    m_progressBar->setValue(60);
    m_resultArea->append(QString("      - 频率统计耗时: %1 ms").arg(freqTime));
    QApplication::processEvents();

    // 2. 建树
    m_resultArea->append("<span style='color: #1A73E8;'>[3/4]</span> 正在构建哈夫曼树...");
    QElapsedTimer treeTimer;
    treeTimer.start();
    Structure::HuffmanTree tree;
    tree.build(freqMap);
    qint64 treeTime = treeTimer.elapsed();
    m_progressBar->setValue(80);
    m_resultArea->append(QString("      - 哈夫曼树构建耗时: %1 ms").arg(treeTime));
    QApplication::processEvents();

    // 3. 编码表
    m_resultArea->append("<span style='color: #1A73E8;'>[4/4]</span> 正在生成编码表...");
    QElapsedTimer codeTimer;
    codeTimer.start();
    tree.generateCodes();
    qint64 codeTime = codeTimer.elapsed();
    m_progressBar->setValue(100);
    m_resultArea->append(QString("      - 编码表生成耗时: %1 ms").arg(codeTime));
    
    qint64 totalElapsed = timer.elapsed();
    double speed = (testSize / 1024.0 / 1024.0) / (totalElapsed / 1000.0 + 0.001);

    m_resultArea->append("\n<b style='color: #202124;'>测试总结:</b>");
    m_resultArea->append(QString("--------------------------------"));
    m_resultArea->append(QString("总处理耗时: <b style='color: #1A73E8;'>%1 ms</b>").arg(totalElapsed));
    m_resultArea->append(QString("平均处理速度: <b style='color: #1A73E8;'>%1 MB/s</b>").arg(speed, 0, 'f', 2));
    m_resultArea->append(QString("--------------------------------\n"));
    
    m_runBtn->setEnabled(true);
}

}
