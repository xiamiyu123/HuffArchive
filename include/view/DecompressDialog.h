#pragma once

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QGroupBox>
#include <QProgressBar>
#include <QThread>
#include "structure/String.h"
#include "structure/ArrayList.h"

namespace View {

class DecompressionWorker : public QObject {
    Q_OBJECT
public:
    DecompressionWorker(const Structure::String& archivePath, 
                        const Structure::String& destPath,
                        const Structure::ArrayList<Structure::String>& filesToExtract = Structure::ArrayList<Structure::String>())
        : m_archivePath(archivePath), m_destPath(destPath), m_filesToExtract(filesToExtract) {
        m_isSelective = (m_filesToExtract.size() > 0);
    }

    void stop() { m_isCancelled = true; }
    void setUseTreeMap(bool use) { m_useTreeMap = use; }

public slots:
    void process();

signals:
    void progressUpdated(int percentage);
    void finished(bool success, QString message);

private:
    Structure::String m_archivePath;
    Structure::String m_destPath;
    Structure::ArrayList<Structure::String> m_filesToExtract;
    bool m_isSelective;
    bool m_useTreeMap = false;
    std::atomic<bool> m_isCancelled{false};
};

class DecompressDialog : public QDialog {
    Q_OBJECT

public:
    explicit DecompressDialog(const QString& archiveName, QWidget *parent = nullptr);
    ~DecompressDialog();

    Structure::String getDestinationPath() const;
    bool shouldOpenFolder() const;
    bool shouldOverwrite() const;
    
    void setFilesToExtract(const Structure::ArrayList<Structure::String>& files);

private slots:
    void onBrowseDest();
    void onExtract();
    void onCancelClicked();
    void onDecompressionFinished(bool success, QString message);
    void updateProgress(int percentage);

private:
    void setupUI();
    void setupHeader();
    void setupSettings();
    void setupBottomPanel();
    void setupConnections();

    QString m_archiveName;
    Structure::ArrayList<Structure::String> m_filesToExtract;

    // UI Components
    QVBoxLayout* m_mainLayout;
    
    // Header
    QLabel* m_titleLabel;
    QLabel* m_subtitleLabel;
    
    // Settings
    QGroupBox* m_settingsGroup;
    QLineEdit* m_destPathEdit;
    QPushButton* m_browseBtn;
    QCheckBox* m_openFolderCheck;
    QCheckBox* m_overwriteCheck;
    QCheckBox* m_useTreeMapCheck; // 新增：使用 TreeMap 选项
    
    // Progress
    QProgressBar* m_progressBar;
    QLabel* m_statusLabel;
    
    // Actions
    QPushButton* m_extractBtn;
    QPushButton* m_cancelBtn;

    // Threading
    QThread* m_workerThread;
    DecompressionWorker* m_worker;
    bool m_isDecompressing;
};

}
