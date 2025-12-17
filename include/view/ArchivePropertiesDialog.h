#pragma once

#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include "structure/String.h"

namespace View {

class ArchivePropertiesDialog : public QDialog {
    Q_OBJECT

public:
    explicit ArchivePropertiesDialog(const Structure::String& archivePath, QWidget *parent = nullptr, const std::string& password = "");
    ~ArchivePropertiesDialog();

private:
    void setupUI();
    void loadProperties();

    Structure::String m_archivePath;
    std::string m_password;
    
    QLabel* m_nameLabel;
    QLabel* m_sizeLabel;
    QLabel* m_fileCountLabel;
    QLabel* m_compressionRatioLabel;
    QLabel* m_createdTimeLabel;
};

} // namespace View
