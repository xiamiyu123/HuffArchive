#pragma once

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QGroupBox>
#include <QProgressBar>
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
    bool useTreeMap() const;

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    void setupUI();
    void setupHeader();
    void setupFileList();
    void setupBottomPanel();
    void setupConnections();
    void updateFileTable();
    
    // UI 组件
    QVBoxLayout* m_mainLayout;
    
    // 标题
    QLabel* m_titleLabel;
    QLabel* m_subtitleLabel;
    
    // 文件列表
    QTableWidget* m_fileTable;
    QPushButton* m_addFilesBtn;
    QPushButton* m_addFolderBtn;
    QPushButton* m_removeBtn;
    QPushButton* m_clearBtn;
    
    // 设置
    QLineEdit* m_destPathEdit;
    QPushButton* m_browseBtn;
    QComboBox* m_compressionLevelCombo;
    QCheckBox* m_useTreeMapCheck; // 新增：使用 TreeMap 选项
    QCheckBox* m_usePasswordCheck; // 新增：使用密码选项
    QLineEdit* m_passwordEdit;    // 新增：密码输入框
    QCheckBox* m_showPasswordCheck; // 新增：显示密码选项
    
    // 底部
    QPushButton* m_compressBtn;
    QPushButton* m_cancelBtn;
    
    // 进度
    QProgressBar* m_progressBar;
    QLabel* m_statusLabel;
    bool m_isCompressing;
    
    Structure::ArrayList<Structure::String> m_selectedFiles;
    
    // 工作线程
    class CompressionWorker* m_worker;

private slots:
    void onAddFiles();
    void onAddFolder();
    void onRemoveSelected();
    void onClearAll();
    void onBrowseDest();
    void onCompress();
    void onCancelClicked();
    void onCompressionProgress(int value, QString message);
    void onCompressionFinished(bool success, QString message);
};

}
