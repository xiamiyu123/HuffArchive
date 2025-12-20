#pragma once

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

namespace View {

class AboutDialog : public QDialog {
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = nullptr);
    ~AboutDialog();

private slots:
    void openRepository();

private:
    void setupUI();
    void setupConnections();
    
    // UI 组件
    QVBoxLayout* m_mainLayout;
    QLabel* m_titleLabel;
    QLabel* m_versionLabel;
    QLabel* m_descriptionLabel;
    QLabel* m_featuresLabel;
    QLabel* m_copyrightLabel;
    QPushButton* m_repositoryButton;
    QPushButton* m_closeButton;
};

}
