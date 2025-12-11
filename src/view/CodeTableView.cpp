#include "view/CodeTableView.h"
#include <QStringList>
#include <cctype>

namespace View {

CodeTableView::CodeTableView(QWidget *parent) : QWidget(parent) {
    setupUi();
}

CodeTableView::~CodeTableView() {
    // QObject cleanup handles children
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
    m_table->setRowCount(0);
    
    // Collect all entries to sort them (optional but better for UI)
    // For now, direct insertion
    
    int row = 0;
    for (auto it = frequencies.begin(); it != frequencies.end(); ++it) {
        unsigned char ch = it->first;
        int freq = it->second;
        
        // Find code
        Structure::String codeStr;
        auto codeIt = codes.find(ch);
        if (codeIt != codes.end()) {
            codeStr = codeIt->second;
        }
        
        m_table->insertRow(row);
        
        // Character display
        QString charDisplay;
        if (ch == '\n') charDisplay = "LF (\\n)";
        else if (ch == '\r') charDisplay = "CR (\\r)";
        else if (ch == '\t') charDisplay = "Tab (\\t)";
        else if (ch == ' ') charDisplay = "Space";
        else if (std::isprint(ch)) charDisplay = QString(QChar(ch));
        else charDisplay = QString("0x%1").arg(ch, 2, 16, QChar('0')).toUpper();
        
        auto charItem = new QTableWidgetItem(charDisplay);
        charItem->setTextAlignment(Qt::AlignCenter);
        m_table->setItem(row, 0, charItem);

        auto freqItem = new QTableWidgetItem(QString::number(freq));
        freqItem->setTextAlignment(Qt::AlignCenter);
        m_table->setItem(row, 1, freqItem);

        auto codeItem = new QTableWidgetItem(QString::fromUtf8(codeStr.c_str()));
        codeItem->setTextAlignment(Qt::AlignCenter);
        m_table->setItem(row, 2, codeItem);
        
        row++;
    }
    
    // Sort by frequency (descending) by default if we wanted, but QTableWidget has sorting built-in if enabled.
    m_table->setSortingEnabled(true);
}

void CodeTableView::clear() {
    m_table->setRowCount(0);
}

}
