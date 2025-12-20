#include "view/CodeTableView.h"
#include <QStringList>
#include <cctype>
#include <QDebug>
#include <exception>

namespace View {

CodeTableView::CodeTableView(QWidget *parent) : QWidget(parent) {
    setupUi();
}

CodeTableView::~CodeTableView() {
    // QObject 清理会自动处理子对象
}

void CodeTableView::setupUi() {
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_table = new QTableWidget(this);
    m_table->setColumnCount(3);
    m_table->setHorizontalHeaderLabels({"Character", "Frequency", "Huffman Code"});
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setAlternatingRowColors(true);
    
    layout->addWidget(m_table);
}


void CodeTableView::updateTable(const Structure::HashMap<unsigned char, int>& frequencies,
                                const Structure::HashMap<unsigned char, Structure::String>& codes) {
    // 1. 防御性检查：确保 UI 组件已初始化
    if (m_table == nullptr) {
        qWarning() << "[CodeTableView] Error: Table widget is not initialized.";
        return;
    }

    // 2. 日志记录：记录输入数据规模
    qDebug() << "[CodeTableView] Updating table with" << frequencies.size() << "entries.";

    // 3. 性能优化：暂时关闭更新以避免闪烁
    m_table->setUpdatesEnabled(false);
    
    // 4. 清理旧数据
    m_table->setRowCount(0);
    m_table->clearContents();
    m_table->setSortingEnabled(false); // 插入时关闭排序以提高性能

    // 5. 边界检查：如果数据为空，直接返回并恢复更新
    if (frequencies.empty()) {
        qDebug() << "[CodeTableView] Frequency map is empty, table cleared.";
        m_table->setUpdatesEnabled(true);
        return;
    }

    int currentRowIndex = 0;

    // 6. 遍历数据
    try {
        for (auto it = frequencies.begin(); it != frequencies.end(); ++it) {
            unsigned char character = it->first;
            int frequency = it->second;
            
            // 获取对应的编码，如果不存在则显示默认值
            Structure::String huffmanCode = "";
            auto codeIterator = codes.find(character);
            if (codeIterator != codes.end()) {
                huffmanCode = codeIterator->second;
            }

            // 插入新行
            m_table->insertRow(currentRowIndex);

            // 设置单元格数据
            QTableWidgetItem* charItem = new QTableWidgetItem(formatCharacterDisplay(character));
            QTableWidgetItem* freqItem = new QTableWidgetItem(QString::number(frequency));
            QTableWidgetItem* codeItem = new QTableWidgetItem(QString::fromUtf8(huffmanCode.c_str()));

            // 设置对齐方式
            charItem->setTextAlignment(Qt::AlignCenter);
            freqItem->setTextAlignment(Qt::AlignCenter);
            codeItem->setTextAlignment(Qt::AlignCenter);

            m_table->setItem(currentRowIndex, 0, charItem);
            m_table->setItem(currentRowIndex, 1, freqItem);
            m_table->setItem(currentRowIndex, 2, codeItem);

            currentRowIndex++;
        }
    } catch (const std::exception& e) {
        qCritical() << "[CodeTableView] Exception during table update:" << e.what();
    } catch (...) {
        qCritical() << "[CodeTableView] Unknown error during table update.";
    }

    // 7. 恢复 UI 更新并启用排序
    m_table->setSortingEnabled(true);
    m_table->setUpdatesEnabled(true);
    qDebug() << "[CodeTableView] Table update completed successfully.";
}

QString CodeTableView::formatCharacterDisplay(unsigned char ch) const {
    if (ch == '\n') return "LF (\\n)";
    if (ch == '\r') return "CR (\\r)";
    if (ch == '\t') return "Tab (\\t)";
    if (ch == ' ') return "Space";
    if (std::isprint(ch)) return QString(QChar(ch));
    return QString("0x%1").arg(ch, 2, 16, QChar('0')).toUpper();
}

void CodeTableView::clear() {
    m_table->setRowCount(0);
}

}
