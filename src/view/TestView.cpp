#include "view/TestView.h"
#include "structure/HuffmanTree.h"
#include "structure/HashMap.h"
#include "structure/MapFactory.h"
#include "view/TreeVisualizer.h"
#include "view/CodeTableView.h"
#include <QElapsedTimer>
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <random>
#include <vector>

namespace View {

TestView::TestView(QWidget *parent) : QWidget(parent), m_currentTree(nullptr) {
    setupUI();
}

TestView::~TestView() {
    if (m_currentTree) delete m_currentTree;
}

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
    m_testTypeCombo->addItem("Map 性能对比测试 (10^5 数据)", 5);
    m_testTypeCombo->setFixedWidth(200);
    m_testTypeCombo->setStyleSheet("QComboBox { padding: 5px; border: 1px solid #DADCE0; border-radius: 4px; }");
    ctrlLayout->addWidget(m_testTypeCombo);

    m_manualInput = new QLineEdit(this);
    m_manualInput->setPlaceholderText("在此输入测试文本...");
    m_manualInput->setFixedWidth(200);
    m_manualInput->setStyleSheet("QLineEdit { padding: 5px; border: 1px solid #DADCE0; border-radius: 4px; }");
    m_manualInput->hide();
    ctrlLayout->addWidget(m_manualInput);

    m_dataSizeInput = new QLineEdit(this);
    m_dataSizeInput->setPlaceholderText("数据量 (默认 100000)");
    m_dataSizeInput->setFixedWidth(150);
    m_dataSizeInput->setStyleSheet("QLineEdit { padding: 5px; border: 1px solid #DADCE0; border-radius: 4px; }");
    m_dataSizeInput->hide();
    ctrlLayout->addWidget(m_dataSizeInput);

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

    m_warningLabel = new QLabel("⚠️ 数据量过大可能导致测试时间较长，建议适当减小数据量。", this);
    m_warningLabel->setStyleSheet("color: #D93025; font-size: 12px; font-weight: 500;");
    m_warningLabel->hide();
    m_mainLayout->addWidget(m_warningLabel);

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
    m_saveBtn->setEnabled(false);
    m_resultArea->clear();
    m_progressBar->show();
    m_progressBar->setValue(0);
    m_resultArea->append("<span style='color: #1A73E8;'>正在准备测试数据，请稍候...</span>");
    
    if (m_currentTree) {
        delete m_currentTree;
        m_currentTree = nullptr;
    }
    m_treeVisualizer->setTree(nullptr);
    m_codeTableView->clear();
    m_lastTestData.clear();
    
    int testType = m_testTypeCombo->currentData().toInt();
    QString manualText = m_manualInput->text();

    // 使用 QtConcurrent 异步运行测试
    QFuture<void> future = QtConcurrent::run([this, testType, manualText]() {
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
                QMetaObject::invokeMethod(m_resultArea, "append", Q_ARG(QString, "测试类型: <b>随机数据 (5MB)</b>"));
                break;
            case 1: // 全零
                testSize = 1 * 1024 * 1024;
                data.assign(testSize, 0);
                QMetaObject::invokeMethod(m_resultArea, "append", Q_ARG(QString, "测试类型: <b>全零数据 (1MB)</b>"));
                break;
            case 2: // 递增
                testSize = 1 * 1024 * 1024;
                data.resize(testSize);
                for (int i = 0; i < testSize; ++i) data[i] = static_cast<unsigned char>(i % 256);
                QMetaObject::invokeMethod(m_resultArea, "append", Q_ARG(QString, "测试类型: <b>递增序列 (1MB)</b>"));
                break;
            case 3: // 少量字符
                testSize = 100 * 1024;
                data.resize(testSize);
                {
                    const char* chars = "ABCDEFG";
                    for (int i = 0; i < testSize; ++i) data[i] = chars[i % 7];
                }
                QMetaObject::invokeMethod(m_resultArea, "append", Q_ARG(QString, "测试类型: <b>少量字符重复 (100KB)</b>"));
                break;
            case 4: // 人工输入
                {
                    if (manualText.isEmpty()) {
                        QMetaObject::invokeMethod(m_resultArea, "append", Q_ARG(QString, "<span style='color: red;'>错误: 请输入测试文本！</span>"));
                        QMetaObject::invokeMethod(m_runBtn, "setEnabled", Q_ARG(bool, true));
                        QMetaObject::invokeMethod(m_progressBar, "hide", Qt::QueuedConnection);
                        return;
                    }
                    QByteArray bytes = manualText.toUtf8();
                    testSize = bytes.size();
                    data.resize(testSize);
                    memcpy(data.data(), bytes.data(), testSize);
                    QMetaObject::invokeMethod(m_resultArea, "append", Q_ARG(QString, QString("测试类型: <b>人工输入数据 (%1 字节)</b>").arg(testSize)));
                }
                break;
            case 5: // Map 性能对比
                runMapPerformanceTest();
                return;
        }

        m_lastTestData = data;
        QMetaObject::invokeMethod(m_progressBar, "setValue", Q_ARG(int, 20));

        QElapsedTimer timer;
        timer.start();

        // 1. 频率统计
        m_lastFreqMap.clear();
        for (unsigned char c : data) {
            if (m_lastFreqMap.contains(c)) m_lastFreqMap[c]++;
            else m_lastFreqMap.put(c, 1);
        }
        qint64 freqTime = timer.elapsed();
        QMetaObject::invokeMethod(m_progressBar, "setValue", Q_ARG(int, 50));
        QMetaObject::invokeMethod(m_resultArea, "append", Q_ARG(QString, QString(" - 频率统计耗时: %1 ms").arg(freqTime)));

        // 2. 建树
        m_currentTree = new Structure::HuffmanTree();
        m_currentTree->build(m_lastFreqMap);
        qint64 treeTime = timer.elapsed() - freqTime;
        QMetaObject::invokeMethod(m_progressBar, "setValue", Q_ARG(int, 80));
        QMetaObject::invokeMethod(m_resultArea, "append", Q_ARG(QString, QString(" - 哈夫曼树构建耗时: %1 ms").arg(treeTime)));

        // 3. 编码表
        m_lastCodes = m_currentTree->generateCodes();
        qint64 codeTime = timer.elapsed() - freqTime - treeTime;
        QMetaObject::invokeMethod(m_progressBar, "setValue", Q_ARG(int, 100));
        QMetaObject::invokeMethod(m_resultArea, "append", Q_ARG(QString, QString(" - 编码表生成耗时: %1 ms").arg(codeTime)));
        
        qint64 totalElapsed = timer.elapsed();
        double speed = (testSize / 1024.0 / 1024.0) / (totalElapsed / 1000.0 + 0.001);

        QMetaObject::invokeMethod(m_resultArea, "append", Q_ARG(QString, QString("\n<b>总耗时: %1 ms</b>").arg(totalElapsed)));
        QMetaObject::invokeMethod(m_resultArea, "append", Q_ARG(QString, QString("<b>平均速度: %1 MB/s</b>").arg(speed, 0, 'f', 2)));
        
        // 更新可视化 (必须在主线程)
        QMetaObject::invokeMethod(this, [this]() {
            m_treeVisualizer->setTree(m_currentTree);
            m_codeTableView->updateTable(m_lastFreqMap, m_lastCodes);
            m_runBtn->setEnabled(true);
            m_saveBtn->setEnabled(true);
            m_progressBar->hide();
        }, Qt::QueuedConnection);
    });
}

void TestView::onSaveResult() {
    if (m_resultArea->toPlainText().isEmpty()) return;

    QString dirPath = QFileDialog::getExistingDirectory(this, "选择保存目录", "");
    if (dirPath.isEmpty()) return;

    QDir dir(dirPath);
    QString folderName = QString("TestResult_%1").arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));
    if (!dir.mkdir(folderName)) return;
    dir.cd(folderName);

    // 1. 保存原始数据 (如果有)
    if (!m_lastTestData.empty()) {
        QFile rawFile(dir.filePath("original.dat"));
        if (rawFile.open(QIODevice::WriteOnly)) {
            rawFile.write(reinterpret_cast<const char*>(m_lastTestData.data()), m_lastTestData.size());
            rawFile.close();
        }
    }

    // 2. 保存编码表 (如果有)
    if (!m_lastCodes.isEmpty()) {
        QFile codeFile(dir.filePath("codes.txt"));
        if (codeFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&codeFile);
            for (auto it = m_lastCodes.begin(); it != m_lastCodes.end(); ++it) {
                unsigned char ch = it->first;
                Structure::String code = it->second;
                out << (int)ch << ":" << code.c_str() << "\n";
            }
            codeFile.close();
        }
    }

    // 3. 保存压缩后的 01 字符串 (如果有)
    if (m_currentTree && !m_lastTestData.empty()) {
        QFile compFile(dir.filePath("compressed_bits.txt"));
        if (compFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&compFile);
            Structure::String encoded = m_currentTree->encode(m_lastTestData.data(), m_lastTestData.size());
            out << encoded.c_str();
            compFile.close();
        }
    }

    // 4. 保存测试报告
    QFile reportFile(dir.filePath("report.txt"));
    if (reportFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&reportFile);
        out << m_resultArea->toPlainText();
        reportFile.close();
    }

    m_resultArea->append(QString("\n<span style='color: green;'>结果已保存至: %1</span>").arg(dir.absolutePath()));
}

void TestView::onTestTypeChanged(int index) {
    int testType = m_testTypeCombo->itemData(index).toInt();
    m_manualInput->setVisible(testType == 4);
    m_dataSizeInput->setVisible(testType == 5);
    m_warningLabel->setVisible(testType == 5 || testType == 0); // Map测试或大随机数据测试显示警告

    // 如果是 Map 性能对比测试，隐藏可视化选项卡
    bool isMapTest = (testType == 5);
    m_tabWidget->setTabVisible(1, !isMapTest); // 哈夫曼树预览
    m_tabWidget->setTabVisible(2, !isMapTest); // 编码详情
    
    if (isMapTest && m_tabWidget->currentIndex() != 0) {
        m_tabWidget->setCurrentIndex(0);
    }
}

void TestView::setTestType(int type) {
    for (int i = 0; i < m_testTypeCombo->count(); ++i) {
        if (m_testTypeCombo->itemData(i).toInt() == type) {
            m_testTypeCombo->setCurrentIndex(i);
            break;
        }
    }
}

void TestView::runMapPerformanceTest() {
    int DATA_SIZE = m_dataSizeInput->text().toInt();
    if (DATA_SIZE <= 0) DATA_SIZE = 100000; // 默认值

    QFuture<void> future = QtConcurrent::run([this, DATA_SIZE]() {
        QMetaObject::invokeMethod(m_resultArea, "append", Q_ARG(QString, QString("<b>Map 基础性能对比测试 (数据量: %1)</b>").arg(DATA_SIZE)));
        QMetaObject::invokeMethod(m_resultArea, "append", Q_ARG(QString, "--------------------------------------------------"));

        // 准备随机数据
        std::vector<int> keys(DATA_SIZE);
        std::vector<int> values(DATA_SIZE);
        std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<int> dist(1, 1000000);

        for (int i = 0; i < DATA_SIZE; ++i) {
            keys[i] = dist(rng);
            values[i] = dist(rng);
        }
        QMetaObject::invokeMethod(m_progressBar, "setValue", Q_ARG(int, 20));

        struct MapTypeInfo {
            QString name;
            Structure::MapType type;
        };

        std::vector<MapTypeInfo> mapTypes = {
            {"HashMap", Structure::MapType::HASH_MAP},
            {"TreeMap (LLRB)", Structure::MapType::TREE_MAP},
            {"TreeMap (STL)", Structure::MapType::TREE_MAP_STL}
        };

        int progressStep = 80 / mapTypes.size();
        int currentProgress = 20;

        for (const auto& info : mapTypes) {
            QMetaObject::invokeMethod(m_resultArea, "append", Q_ARG(QString, QString("正在测试: <span style='color: #1A73E8;'>%1</span>...").arg(info.name)));

            Structure::MapHuff<int, int>* map = Structure::MapFactory<int, int>::createMap(info.type);
            
            QElapsedTimer timer;
            
            // 1. 插入测试
            timer.start();
            for (int i = 0; i < DATA_SIZE; ++i) {
                map->put(keys[i], values[i]);
            }
            qint64 insertTime = timer.elapsed();

            // 2. 查找测试
            timer.start();
            for (int i = 0; i < DATA_SIZE; ++i) {
                map->contains(keys[i]);
            }
            qint64 containsTime = timer.elapsed();

            // 3. 获取测试
            timer.start();
            for (int i = 0; i < DATA_SIZE; ++i) {
                map->get(keys[i]);
            }
            qint64 getTime = timer.elapsed();

            QMetaObject::invokeMethod(m_resultArea, "append", Q_ARG(QString, QString("  - 插入耗时: %1 ms").arg(insertTime)));
            QMetaObject::invokeMethod(m_resultArea, "append", Q_ARG(QString, QString("  - 查找耗时: %1 ms").arg(containsTime)));
            QMetaObject::invokeMethod(m_resultArea, "append", Q_ARG(QString, QString("  - 获取耗时: %1 ms").arg(getTime)));

            // --- 哈夫曼算法集成测试 ---
            QMetaObject::invokeMethod(m_resultArea, "append", Q_ARG(QString, "  [哈夫曼算法集成测试 - 1MB 数据]"));
            
            std::vector<unsigned char> huffData(1024 * 1024);
            for (auto& b : huffData) b = static_cast<unsigned char>(dist(rng) % 256);

            Structure::MapHuff<unsigned char, int>* freqMap = Structure::MapFactory<unsigned char, int>::createMap(info.type);
            timer.start();
            for (unsigned char c : huffData) {
                int* count = freqMap->get(c);
                if (count) (*count)++;
                else freqMap->put(c, 1);
            }
            qint64 huffFreqTime = timer.elapsed();

            Structure::HuffmanTree huffTree;
            timer.start();
            huffTree.build(*freqMap);
            qint64 huffBuildTime = timer.elapsed();

            Structure::String encoded = huffTree.encode(huffData.data(), 10240);
            int bitPos = 0;
            auto readBit = [&]() -> int {
                if (bitPos >= (int)encoded.length()) return -1;
                return encoded[bitPos++] == '1' ? 1 : 0;
            };
            auto writeByte = [&](unsigned char) {};
            
            timer.start();
            huffTree.decode(readBit, writeByte, 10240);
            qint64 huffDecodeTime = timer.elapsed();

            QMetaObject::invokeMethod(m_resultArea, "append", Q_ARG(QString, QString("    * 频率统计 (Map-%1): %2 ms").arg(info.name).arg(huffFreqTime)));
            QMetaObject::invokeMethod(m_resultArea, "append", Q_ARG(QString, QString("    * 哈夫曼建树: %1 ms").arg(huffBuildTime)));
            QMetaObject::invokeMethod(m_resultArea, "append", Q_ARG(QString, QString("    * 流式解码 (10KB): %1 ms").arg(huffDecodeTime)));
            QMetaObject::invokeMethod(m_resultArea, "append", Q_ARG(QString, ""));

            delete freqMap;
            delete map;
            currentProgress += progressStep;
            QMetaObject::invokeMethod(m_progressBar, "setValue", Q_ARG(int, currentProgress));
        }

        QMetaObject::invokeMethod(m_resultArea, "append", Q_ARG(QString, "--------------------------------------------------"));
        QMetaObject::invokeMethod(m_resultArea, "append", Q_ARG(QString, "<b style='color: green;'>测试完成！</b>"));
        
        QMetaObject::invokeMethod(this, [this]() {
            m_runBtn->setEnabled(true);
            m_saveBtn->setEnabled(true);
            m_progressBar->hide();
        }, Qt::QueuedConnection);
    });
}

}

