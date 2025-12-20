#include "view/TestView.h"
#include "structure/HuffmanTree.h"
#include "structure/HashMap.h"
#include "view/TreeVisualizer.h"
#include "view/CodeTableView.h"
#include <QElapsedTimer>
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <random>
#include <vector>

namespace View {

TestView::TestView(QWidget *parent) : QWidget(parent) {
    setupUI();
}

TestView::~TestView() {}

void TestView::setupUI() {
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(20, 20, 20, 20);
    m_mainLayout->setSpacing(15);

    // 标题栏
    QHBoxLayout* headerLayout = new QHBoxLayout();
    m_titleLabel = new QLabel("算法测试与可视化诊断", this);
    QFont titleFont = m_titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    m_titleLabel->setFont(titleFont);
    headerLayout->addWidget(m_titleLabel);
    headerLayout->addStretch();
    
    m_backBtn = new QPushButton("返回主页", this);
    m_backBtn->setFixedSize(100, 36);
    m_backBtn->setCursor(Qt::PointingHandCursor);
    m_backBtn->setStyleSheet(
        "QPushButton { border: 1px solid #DADCE0; border-radius: 4px; background: white; color: #3C4043; }"
        "QPushButton:hover { background: #F8F9FA; }"
    );
    headerLayout->addWidget(m_backBtn);
    m_mainLayout->addLayout(headerLayout);

    // 控制栏
    QHBoxLayout* ctrlLayout = new QHBoxLayout();
    
    QLabel* typeLabel = new QLabel("测试类型:", this);
    ctrlLayout->addWidget(typeLabel);

    m_testTypeCombo = new QComboBox(this);
    m_testTypeCombo->addItem("随机数据测试 (5MB)", 0);
    m_testTypeCombo->addItem("全零数据测试 (1MB)", 1);
    m_testTypeCombo->addItem("递增序列测试 (1MB)", 2);
    m_testTypeCombo->addItem("少量字符重复测试 (100KB)", 3);
    m_testTypeCombo->addItem("人工输入数据", 4);
    m_testTypeCombo->setFixedWidth(200);
    m_testTypeCombo->setStyleSheet("QComboBox { padding: 5px; border: 1px solid #DADCE0; border-radius: 4px; }");
    ctrlLayout->addWidget(m_testTypeCombo);

    m_manualInput = new QLineEdit(this);
    m_manualInput->setPlaceholderText("在此输入测试文本...");
    m_manualInput->setFixedWidth(200);
    m_manualInput->setStyleSheet("QLineEdit { padding: 5px; border: 1px solid #DADCE0; border-radius: 4px; }");
    m_manualInput->hide();
    ctrlLayout->addWidget(m_manualInput);

    m_runBtn = new QPushButton("开始测试", this);
    m_runBtn->setFixedSize(120, 36);
    m_runBtn->setCursor(Qt::PointingHandCursor);
    m_runBtn->setStyleSheet(
        "QPushButton { background: #1A73E8; color: white; border-radius: 4px; font-weight: bold; }"
        "QPushButton:hover { background: #1557B0; }"
        "QPushButton:disabled { background: #E0E0E0; color: #9E9E9E; }"
    );
    ctrlLayout->addWidget(m_runBtn);

    m_saveBtn = new QPushButton("保存结果", this);
    m_saveBtn->setFixedSize(100, 36);
    m_saveBtn->setCursor(Qt::PointingHandCursor);
    m_saveBtn->setStyleSheet(
        "QPushButton { border: 1px solid #DADCE0; border-radius: 4px; background: white; color: #3C4043; }"
        "QPushButton:hover { background: #F8F9FA; }"
    );
    ctrlLayout->addWidget(m_saveBtn);

    ctrlLayout->addStretch();
    m_mainLayout->addLayout(ctrlLayout);

    // 进度条
    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->setFixedHeight(4);
    m_progressBar->setTextVisible(false);
    m_progressBar->setStyleSheet(
        "QProgressBar { border: none; background: #E0E0E0; border-radius: 2px; }"
        "QProgressBar::chunk { background: #1A73E8; border-radius: 2px; }"
    );
    m_progressBar->hide();
    m_mainLayout->addWidget(m_progressBar);

    // 选项卡界面
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setStyleSheet(
        "QTabWidget::pane { border: 1px solid #DADCE0; border-radius: 4px; background: white; }"
        "QTabBar::tab { padding: 10px 20px; background: #F8F9FA; border: 1px solid #DADCE0; border-bottom: none; border-top-left-radius: 4px; border-top-right-radius: 4px; margin-right: 2px; }"
        "QTabBar::tab:selected { background: white; border-bottom: 2px solid #1A73E8; font-weight: bold; }"
    );

    // 1. 运行日志
    m_resultArea = new QTextEdit(this);
    m_resultArea->setReadOnly(true);
    m_resultArea->setPlaceholderText("测试结果将显示在这里...");
    m_resultArea->setStyleSheet("QTextEdit { border: none; font-family: 'Consolas', monospace; font-size: 12px; }");
    m_tabWidget->addTab(m_resultArea, "运行日志");

    // 2. 树可视化
    m_treeVisualizer = new TreeVisualizer(this);
    m_tabWidget->addTab(m_treeVisualizer, "哈夫曼树预览");

    // 3. 编码表
    m_codeTableView = new CodeTableView(this);
    m_tabWidget->addTab(m_codeTableView, "编码详情");

    m_mainLayout->addWidget(m_tabWidget);

    connect(m_runBtn, &QPushButton::clicked, this, &TestView::onRunTest);
    connect(m_saveBtn, &QPushButton::clicked, this, &TestView::onSaveResult);
    connect(m_backBtn, &QPushButton::clicked, this, &TestView::backRequested);
    connect(m_testTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TestView::onTestTypeChanged);
}

void TestView::onRunTest() {
    m_runBtn->setEnabled(false);
    m_resultArea->clear();
    m_progressBar->show();
    m_progressBar->setValue(0);
    m_treeVisualizer->setTree(nullptr);
    m_codeTableView->clear();
    
    int testType = m_testTypeCombo->currentData().toInt();
    int testSize = 0;
    std::vector<unsigned char> data;

    switch(testType) {
        case 0: // 随机
            testSize = 5 * 1024 * 1024;
            data.resize(testSize);
            {
                std::mt19937 gen(std::random_device{}());
                std::uniform_int_distribution<> dis(0, 255);
                for (int i = 0; i < testSize; ++i) data[i] = static_cast<unsigned char>(dis(gen));
            }
            m_resultArea->append("测试类型: <b>随机数据 (5MB)</b>");
            break;
        case 1: // 全零
            testSize = 1 * 1024 * 1024;
            data.assign(testSize, 0);
            m_resultArea->append("测试类型: <b>全零数据 (1MB)</b>");
            break;
        case 2: // 递增
            testSize = 1 * 1024 * 1024;
            data.resize(testSize);
            for (int i = 0; i < testSize; ++i) data[i] = static_cast<unsigned char>(i % 256);
            m_resultArea->append("测试类型: <b>递增序列 (1MB)</b>");
            break;
        case 3: // 少量字符
            testSize = 100 * 1024;
            data.resize(testSize);
            {
                const char* chars = "ABCDEFG";
                for (int i = 0; i < testSize; ++i) data[i] = chars[i % 7];
            }
            m_resultArea->append("测试类型: <b>少量字符重复 (100KB)</b>");
            break;
        case 4: // 人工输入
            {
                QString text = m_manualInput->text();
                if (text.isEmpty()) {
                    m_resultArea->append("<span style='color: red;'>错误: 请输入测试文本！</span>");
                    m_runBtn->setEnabled(true);
                    m_progressBar->hide();
                    return;
                }
                QByteArray bytes = text.toUtf8();
                testSize = bytes.size();
                data.resize(testSize);
                memcpy(data.data(), bytes.data(), testSize);
                m_resultArea->append(QString("测试类型: <b>人工输入数据 (%1 字节)</b>").arg(testSize));
            }
            break;
    }

    QApplication::processEvents();
    m_progressBar->setValue(20);

    QElapsedTimer timer;
    timer.start();

    // 1. 频率统计
    Structure::HashMap<unsigned char, int> freqMap;
    for (unsigned char c : data) {
        if (freqMap.contains(c)) freqMap[c]++;
        else freqMap.put(c, 1);
    }
    qint64 freqTime = timer.elapsed();
    m_progressBar->setValue(50);
    m_resultArea->append(QString(" - 频率统计耗时: %1 ms").arg(freqTime));
    QApplication::processEvents();

    // 2. 建树
    Structure::HuffmanTree* tree = new Structure::HuffmanTree();
    tree->build(freqMap);
    qint64 treeTime = timer.elapsed() - freqTime;
    m_progressBar->setValue(80);
    m_resultArea->append(QString(" - 哈夫曼树构建耗时: %1 ms").arg(treeTime));
    QApplication::processEvents();

    // 3. 编码表
    Structure::HashMap<unsigned char, Structure::String> codes = tree->generateCodes();
    qint64 codeTime = timer.elapsed() - freqTime - treeTime;
    m_progressBar->setValue(100);
    m_resultArea->append(QString(" - 编码表生成耗时: %1 ms").arg(codeTime));
    
    qint64 totalElapsed = timer.elapsed();
    double speed = (testSize / 1024.0 / 1024.0) / (totalElapsed / 1000.0 + 0.001);

    m_resultArea->append(QString("\n<b>总耗时: %1 ms</b>").arg(totalElapsed));
    m_resultArea->append(QString("<b>平均速度: %1 MB/s</b>").arg(speed, 0, 'f', 2));
    
    // 更新可视化
    m_treeVisualizer->setTree(tree);
    m_codeTableView->updateTable(freqMap, codes);
    
    m_runBtn->setEnabled(true);
    m_progressBar->hide();
}

void TestView::onSaveResult() {
    QString fileName = QFileDialog::getSaveFileName(this, "保存测试日志", "", "Text Files (*.txt);;All Files (*)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << m_resultArea->toPlainText();
        file.close();
    }
}

void TestView::onTestTypeChanged(int index) {
    int testType = m_testTypeCombo->itemData(index).toInt();
    if (testType == 4) {
        m_manualInput->show();
    } else {
        m_manualInput->hide();
    }
}

}

