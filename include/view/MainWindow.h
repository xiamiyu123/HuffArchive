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
#include <QFileDialog>
#include <QStackedWidget>
#include <QToolButton>

namespace View {

class ArchiveView;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
    void showArchiveView(const QString& archivePath);

private:
    void setupUI();
    void setupMenuBar();
    void setupConnections();
    void showWelcomeScreen();
    void updateHistoryUI();
    
    // UI Components
    QStackedWidget* m_stackedWidget;
    QWidget* m_welcomeWidget;
    QWidget* m_centralWidget;
    QVBoxLayout* m_mainLayout;
    QHBoxLayout* m_buttonLayout;
    
    QToolButton* m_openButton;
    QPushButton* m_newButton;
    QMenu* m_historyMenu;
    
    ArchiveView* m_archiveView;
    
    // Menu components
    QMenu* m_fileMenu;
    QMenu* m_recentFilesMenu;
    QMenu* m_settingsMenu;
    QMenu* m_helpMenu;
    
    // Actions
    QAction* m_newAction;
    QAction* m_openAction;
    QAction* m_exitAction;
    QAction* m_associateAction;
    
private slots:
    void onOpenArchive();
    void onNewArchive();
    void onHistoryActionTriggered();
    void onAssociateFileExtension();
};

}
