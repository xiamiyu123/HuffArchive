#include "model/HistoryManager.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDateTime>
#include <QStandardPaths>
#include <QCoreApplication>

namespace Model {

HistoryManager& HistoryManager::instance() {
    static HistoryManager instance;
    return instance;
}

HistoryManager::HistoryManager() {
    // 确保数据目录相对于应用程序可执行文件存在
    QString dataPath = QCoreApplication::applicationDirPath() + "/data";
    QDir dir(dataPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    m_historyFilePath = (dataPath + "/history.csv").toUtf8().constData();
    loadHistory();
}

void HistoryManager::addHistory(const Structure::String& path) {
    // 如果已存在则移除，以便将其移动到顶部
    for (int i = 0; i < m_historyList.size(); ++i) {
        if (m_historyList[i] == path) {
            m_historyList.remove(i);
            i--;
        }
    }
    
    m_historyList.insert(0, path);

    // 限制大小
    while (m_historyList.size() > MAX_HISTORY_SIZE) {
        m_historyList.remove(m_historyList.size() - 1);
    }

    saveHistory();
}

Structure::ArrayList<Structure::String> HistoryManager::getHistory() const {
    return m_historyList;
}

void HistoryManager::loadHistory() {
    // 清空列表（重新分配为空）
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
