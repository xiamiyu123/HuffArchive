#pragma once

#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QHeaderView>
#include <QProgressBar>
#include <QFutureWatcher>
#include "structure/String.h"

namespace View {

class ArchiveView : public QWidget {
    Q_OBJECT

public:
    explicit ArchiveView(const Structure::String& archivePath, QWidget *parent = nullptr);
    ~ArchiveView();

    void loadArchive();

signals:
    void backRequested();

private:
    void setupUI();
    void setupToolBar();
    void setupFileList();
    void setupConnections();
    
    Structure::String m_archivePath;
    std::string m_password;
    
    // UI 组件
    QVBoxLayout* m_mainLayout;
    QWidget* m_topPanel;      // 替换 QToolBar 以进行自定义样式设置
    QLineEdit* m_pathEdit;    // 地址栏
    QLineEdit* m_searchEdit;  // 搜索栏
    QTreeWidget* m_fileList;
    QLabel* m_statusLabel;
    QProgressBar* m_progressBar;
    
    // 工具栏操作
    QPushButton* m_extractBtn;
    QPushButton* m_extractSelectedBtn;
    QPushButton* m_addBtn;
    QPushButton* m_deleteBtn;
    QPushButton* m_infoBtn;   // 替换属性/测试/帮助，使 UI 更整洁

    // 异步处理
    QFutureWatcher<std::pair<bool, std::string>> m_openFileWatcher;
    Structure::String m_currentOpeningFile;
    Structure::String m_currentTempDir;

private slots:
    void onExtract();
    void onExtractSelected();
    void onAdd();
    void onDelete();
    void onInfo();
    void onSearchTextChanged(const QString& text);
    void onItemDoubleClicked(QTreeWidgetItem* item, int column);
    void onOpenFileFinished();

private:
    // 临时解压辅助函数
    void extractAndOpenFile(const Structure::String& relativePath);
};

}
