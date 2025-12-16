#pragma once

#include "structure/String.h"
#include "structure/ArrayList.h"

namespace Model {

class HistoryManager {
public:
    static HistoryManager& instance();

    void addHistory(const Structure::String& path);
    Structure::ArrayList<Structure::String> getHistory() const;
    void loadHistory();
    void saveHistory() const;

private:
    HistoryManager();
    ~HistoryManager() = default;
    HistoryManager(const HistoryManager&) = delete;
    HistoryManager& operator=(const HistoryManager&) = delete;

    Structure::String m_historyFilePath;
    Structure::ArrayList<Structure::String> m_historyList;
    const int MAX_HISTORY_SIZE = 10;
};

}
