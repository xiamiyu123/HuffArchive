#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QProgressBar>

namespace View {

class TestView : public QWidget {
    Q_OBJECT
public:
    explicit TestView(QWidget *parent = nullptr);
    ~TestView();

signals:
    void backRequested();

private slots:
    void onRunSpeedTest();

private:
    void setupUI();
    
    QVBoxLayout* m_mainLayout;
    QLabel* m_titleLabel;
    QPushButton* m_runBtn;
    QPushButton* m_backBtn;
    QTextEdit* m_resultArea;
    QProgressBar* m_progressBar;
};

}
