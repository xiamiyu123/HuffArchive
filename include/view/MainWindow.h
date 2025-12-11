#pragma once

#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QMenuBar>
#include <QMenu>
#include <QAction>

namespace View {

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void setupUI();
    void setupMenuBar();
    void setupConnections();
    
    // UI Components
    QWidget* m_centralWidget;
    QVBoxLayout* m_mainLayout;
    QHBoxLayout* m_buttonLayout;
    
    QPushButton* m_openButton;
    QPushButton* m_newButton;
    
    // Menu components
    QMenu* m_fileMenu;
    QMenu* m_editMenu;
    QMenu* m_searchMenu;
    QMenu* m_optionsMenu;
    QMenu* m_viewMenu;
    QMenu* m_helpMenu;
    
    // Actions
    QAction* m_newAction;
    QAction* m_openAction;
    QAction* m_exitAction;
    
private slots:
    void onOpenArchive();
    void onNewArchive();
};

}
