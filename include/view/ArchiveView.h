#pragma once

#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolBar>
#include <QPushButton>
#include <QLabel>
#include <QHeaderView>
#include "structure/String.h"

namespace View {

class ArchiveView : public QWidget {
    Q_OBJECT

public:
    explicit ArchiveView(const Structure::String& archivePath, QWidget *parent = nullptr);
    ~ArchiveView();

    void loadArchive();

private:
    void setupUI();
    void setupToolBar();
    void setupFileList();
    void setupConnections();
    
    Structure::String m_archivePath;
    
    // UI Components
    QVBoxLayout* m_mainLayout;
    QToolBar* m_toolBar;
    QTreeWidget* m_fileList;
    QLabel* m_statusLabel;
    
    // Toolbar actions
    QPushButton* m_openBtn;
    QPushButton* m_extractBtn;
    QPushButton* m_newFolderBtn;
    QPushButton* m_addBtn;
    QPushButton* m_deleteBtn;
    QPushButton* m_testBtn;
    QPushButton* m_propertiesBtn;
    QPushButton* m_helpBtn;

private slots:
    void onOpen();
    void onExtract();
    void onNewFolder();
    void onAdd();
    void onDelete();
    void onTest();
    void onProperties();
    void onHelp();
};

}
