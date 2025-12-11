#pragma once

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QGroupBox>
#include "structure/String.h"
#include "structure/ArrayList.h"

namespace View {

class NewArchiveDialog : public QDialog {
    Q_OBJECT

public:
    explicit NewArchiveDialog(QWidget *parent = nullptr);
    ~NewArchiveDialog();

    Structure::String getArchivePath() const;
    Structure::ArrayList<Structure::String> getFilesToCompress() const;

private:
    void setupUI();
    void setupFileListSection();
    void setupArchiveSettingsSection();
    void setupConnections();
    
    // UI Components
    QVBoxLayout* m_mainLayout;
    
    // 文件列表区域
    QGroupBox* m_fileListGroup;
    QListWidget* m_fileList;
    QPushButton* m_addButton;
    QPushButton* m_addFolderButton;
    QPushButton* m_deleteButton;
    
    // 压缩文件设置区域
    QGroupBox* m_settingsGroup;
    QLineEdit* m_archivePathEdit;
    QPushButton* m_browseButton;
    QComboBox* m_formatCombo;
    QCheckBox* m_moreOptionsCheckBox;
    
    // 底部按钮
    QPushButton* m_startButton;
    QPushButton* m_cancelButton;
    
    Structure::ArrayList<Structure::String> m_selectedFiles;

private slots:
    void onAddFiles();
    void onAddFolder();
    void onDeleteFiles();
    void onBrowse();
    void onStart();
    void onCancel();
};

}
