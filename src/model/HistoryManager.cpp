#include "model/HistoryManager.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDateTime>
#include <QStandardPaths>

namespace Model {

HistoryManager& HistoryManager::instance() {
    static HistoryManager instance;
    return instance;
}

HistoryManager::HistoryManager() {
    // Ensure data directory exists
    QDir dir("data");
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    m_historyFilePath = "data/history.csv";
    loadHistory();
}

void HistoryManager::addHistory(const Structure::String& path) {
    // Remove if already exists to move it to the top
    for (int i = 0; i < m_historyList.size(); ++i) {
        if (m_historyList[i] == path) {
            m_historyList.remove(i);
            i--;
        }
    }
    
    m_historyList.insert(0, path);

    // Limit size
    while (m_historyList.size() > MAX_HISTORY_SIZE) {
        m_historyList.remove(m_historyList.size() - 1);
    }

    saveHistory();
}

Structure::ArrayList<Structure::String> HistoryManager::getHistory() const {
    return m_historyList;
}

void HistoryManager::loadHistory() {
    // Clear list (re-assign empty)
    m_historyList = Structure::ArrayList<Structure::String>();
    
    QFile file(QString::fromUtf8(m_historyFilePath.c_str()));
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (!line.isEmpty()) {
                m_historyList.add(Structure::String(line.toUtf8().constData()));
            }
        }
        file.close();
    }
}

void HistoryManager::saveHistory() const {
    QFile file(QString::fromUtf8(m_historyFilePath.c_str()));
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (int i = 0; i < m_historyList.size(); ++i) {
            out << QString::fromUtf8(m_historyList[i].c_str()) << "\n";
        }
        file.close();
    }
}

}
