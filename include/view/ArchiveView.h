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
    QWidget* m_topPanel;      // Replaces QToolBar for custom styling
    QLineEdit* m_pathEdit;    // Address bar
    QTreeWidget* m_fileList;
    QLabel* m_statusLabel;
    
    // Toolbar actions
    QPushButton* m_extractBtn;
    QPushButton* m_extractSelectedBtn;
    QPushButton* m_addBtn;
    QPushButton* m_deleteBtn;
    QPushButton* m_infoBtn;   // Replaces properties/test/help for cleaner UI

private slots:
    void onExtract();
    void onExtractSelected();
    void onAdd();
    void onDelete();
    void onInfo();
    void onItemDoubleClicked(QTreeWidgetItem* item, int column);

private:
    // Helper for temporary extraction
    void extractAndOpenFile(const Structure::String& relativePath);
};

}
