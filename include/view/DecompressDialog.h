#pragma once

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QGroupBox>
#include "structure/String.h"

namespace View {

class DecompressDialog : public QDialog {
    Q_OBJECT

public:
    explicit DecompressDialog(const QString& archiveName, QWidget *parent = nullptr);
    ~DecompressDialog();

    Structure::String getDestinationPath() const;
    bool shouldOpenFolder() const;
    bool shouldOverwrite() const;

private:
    void setupUI();
    void setupHeader();
    void setupSettings();
    void setupBottomPanel();
    void setupConnections();

    QString m_archiveName;

    // UI Components
    QVBoxLayout* m_mainLayout;
    
    // Header
    QLabel* m_titleLabel;
    QLabel* m_subtitleLabel;
    
    // Settings
    QLineEdit* m_destPathEdit;
    QPushButton* m_browseBtn;
    QCheckBox* m_openFolderCheck;
    QCheckBox* m_overwriteCheck;
    
    // Actions
    QPushButton* m_extractBtn;
    QPushButton* m_cancelBtn;

private slots:
    void onBrowseDest();
    void onExtract();
};

}
