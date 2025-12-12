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
    
    // UI Components
    QVBoxLayout* m_mainLayout;
    
    // Header
    QLabel* m_titleLabel;
    QLabel* m_subtitleLabel;
    
    // File List
    QTableWidget* m_fileTable;
    QPushButton* m_addFilesBtn;
    QPushButton* m_addFolderBtn;
    QPushButton* m_removeBtn;
    QPushButton* m_clearBtn;
    
    // Settings
    QLineEdit* m_destPathEdit;
    QPushButton* m_browseBtn;
    QComboBox* m_compressionLevelCombo;
    
    // Bottom
    QPushButton* m_compressBtn;
    QPushButton* m_cancelBtn;
    
    // Progress
    QProgressBar* m_progressBar;
    QLabel* m_statusLabel;
    bool m_isCompressing;
    
    Structure::ArrayList<Structure::String> m_selectedFiles;
    
    // Worker
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
