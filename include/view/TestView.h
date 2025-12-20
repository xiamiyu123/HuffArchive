#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QProgressBar>
#include <QComboBox>
#include <QTabWidget>
#include <QLineEdit>
#include <QFileDialog>

namespace View {

class TreeVisualizer;
class CodeTableView;

class TestView : public QWidget {
    Q_OBJECT
public:
    explicit TestView(QWidget *parent = nullptr);
    ~TestView();

signals:
    void backRequested();

private slots:
    void onRunTest();
    void onSaveResult();
    void onTestTypeChanged(int index);

private:
    void setupUI();
    
    QVBoxLayout* m_mainLayout;
    QLabel* m_titleLabel;
    QComboBox* m_testTypeCombo;
    QLineEdit* m_manualInput;
    QPushButton* m_runBtn;
    QPushButton* m_saveBtn;
    QPushButton* m_backBtn;
    
    QTabWidget* m_tabWidget;
    QTextEdit* m_resultArea;
    TreeVisualizer* m_treeVisualizer;
    CodeTableView* m_codeTableView;
    
    QProgressBar* m_progressBar;
};

}
