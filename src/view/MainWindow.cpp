#include "view/MainWindow.h"
#include "view/ArchiveView.h"
#include "view/TestView.h"
#include "view/NewArchiveDialog.h"
#include "view/AboutDialog.h"
#include "command/CompressDirectoryCommand.h"
#include "model/HistoryManager.h"
#include "util/SystemUtils.h"
#include "structure/HuffmanTree.h"
#include <QIcon>
#include <QFont>
#include <QSize>
#include <QMessageBox>
#include <QFileInfo>
#include <QProgressDialog>
#include <QElapsedTimer>
#include <QApplication>
#include <random>
#include <algorithm>

namespace View {

MainWindow::MainWindow(QWidget *parent) 
    : QMainWindow(parent),
      m_stackedWidget(nullptr),
      m_welcomeWidget(nullptr),
      m_centralWidget(nullptr),
      m_mainLayout(nullptr),
      m_buttonLayout(nullptr),
      m_openButton(nullptr),
      m_newButton(nullptr),
      m_historyMenu(nullptr),
      m_archiveView(nullptr),
      m_testView(nullptr),
      m_fileMenu(nullptr),
      m_recentFilesMenu(nullptr),
      m_helpMenu(nullptr),
      m_newAction(nullptr),
      m_openAction(nullptr),
      m_exitAction(nullptr)
{
    setupUI();
    setupMenuBar();
    setupConnections();
}

MainWindow::~MainWindow() {
}

void MainWindow::setupUI() {
    // 设置窗口标题和大小
    setWindowTitle("哈夫曼压缩工具");
    setMinimumSize(800, 600);
    
    // 创建堆叠窗口部件（用于在欢迎界面和文件列表界面之间切换）
    m_stackedWidget = new QStackedWidget(this);
    setCentralWidget(m_stackedWidget);
    
    // 创建欢迎界面
    showWelcomeScreen();
}

void MainWindow::showWelcomeScreen() {
    // 如果欢迎界面已存在，直接显示
    if (m_welcomeWidget) {
        m_stackedWidget->setCurrentWidget(m_welcomeWidget);
        setWindowTitle("哈夫曼压缩工具");
        return;
    }
    
    // 创建欢迎界面部件
    m_welcomeWidget = new QWidget(this);
    
    // 创建主布局（用于整个欢迎界面）
    QVBoxLayout* welcomeLayout = new QVBoxLayout(m_welcomeWidget);
    welcomeLayout->setContentsMargins(50, 50, 50, 50);
    welcomeLayout->setSpacing(20);
    
    // 添加顶部弹簧（垂直居中）
    welcomeLayout->addStretch(1);
    
    // 创建按钮布局
    m_buttonLayout = new QHBoxLayout();
    m_buttonLayout->setSpacing(30);
    
    // 创建"打开压缩文件"按钮
    m_openButton = new QToolButton(this);
    m_openButton->setText("打开压缩文件");
    m_openButton->setMinimumSize(200, 120);
    m_openButton->setMaximumSize(250, 150);
    m_openButton->setCursor(Qt::PointingHandCursor);
    m_openButton->setPopupMode(QToolButton::MenuButtonPopup);
    m_openButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    
    // 设置按钮样式
    m_openButton->setStyleSheet(
        "QToolButton {"
        "   background-color: white;"
        "   border: 2px solid #E0E0E0;"
        "   border-radius: 8px;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "   padding: 20px;"
        "}"
        "QToolButton:hover {"
        "   background-color: #F5F5F5;"
        "   border-color: #1890FF;"
        "}"
        "QToolButton:pressed {"
        "   background-color: #E8E8E8;"
        "}"
        "QToolButton::menu-button {"
        "   border-left: 1px solid #E0E0E0;"
        "   border-top-right-radius: 8px;"
        "   border-bottom-right-radius: 8px;"
        "   width: 30px;"
        "}"
        "QToolButton::menu-button:hover {"
        "   background-color: #E3F2FD;"
        "}"
    );

    // 创建历史记录菜单
    m_historyMenu = new QMenu(this);
    m_openButton->setMenu(m_historyMenu);
    updateHistoryUI();
    
    // 创建"新建压缩文件"按钮
    m_newButton = new QPushButton(this);
    m_newButton->setText("新建压缩文件");
    m_newButton->setMinimumSize(200, 120);
    m_newButton->setMaximumSize(250, 150);
    m_newButton->setCursor(Qt::PointingHandCursor);
    
    // 设置按钮样式
    m_newButton->setStyleSheet(
        "QPushButton {"
        "   background-color: white;"
        "   border: 2px solid #E0E0E0;"
        "   border-radius: 8px;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "   padding: 20px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #F5F5F5;"
        "   border-color: #1890FF;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #E8E8E8;"
        "}"
    );
    
    // 添加按钮到布局
    m_buttonLayout->addStretch(1);
    m_buttonLayout->addWidget(m_openButton);
    m_buttonLayout->addWidget(m_newButton);
    m_buttonLayout->addStretch(1);
    
    // 添加按钮布局到主布局
    welcomeLayout->addLayout(m_buttonLayout);
    
    // 添加底部弹簧（垂直居中）
    welcomeLayout->addStretch(1);
    
    // 设置欢迎界面背景色
    m_welcomeWidget->setStyleSheet("QWidget { background-color: #1890FF; }");
    
    // 将欢迎界面添加到堆叠窗口
    m_stackedWidget->addWidget(m_welcomeWidget);
    m_stackedWidget->setCurrentWidget(m_welcomeWidget);
}

void MainWindow::showArchiveView(const QString& archivePath) {
    // 添加到历史记录
    Model::HistoryManager::instance().addHistory(Structure::String(archivePath.toUtf8().constData()));
    updateHistoryUI();

    // 如果已有 ArchiveView，先移除
    if (m_archiveView) {
        m_stackedWidget->removeWidget(m_archiveView);
        delete m_archiveView;
        m_archiveView = nullptr;
    }
    
    // 创建新的 ArchiveView
    m_archiveView = new ArchiveView(Structure::String(archivePath.toUtf8().constData()), this);
    m_stackedWidget->addWidget(m_archiveView);
    m_stackedWidget->setCurrentWidget(m_archiveView);
    
    // 连接返回信号
    connect(m_archiveView, &ArchiveView::backRequested, this, &MainWindow::showWelcomeScreen);
    
    // 更新窗口标题
    setWindowTitle(QString("哈夫曼压缩工具 - %1").arg(QFileInfo(archivePath).fileName()));
}

void MainWindow::showTestView() {
    if (!m_testView) {
        m_testView = new TestView(this);
        m_stackedWidget->addWidget(m_testView);
        connect(m_testView, &TestView::backRequested, this, &MainWindow::showWelcomeScreen);
    }
    m_stackedWidget->setCurrentWidget(m_testView);
    setWindowTitle("哈夫曼压缩工具 - 性能测试");
}

void MainWindow::setupMenuBar() {
    // 创建菜单栏
    QMenuBar* menuBar = this->menuBar();
    
    // 设置菜单栏样式
    menuBar->setStyleSheet(
        "QMenuBar {"
        "   background-color: white;"
        "   color: black;"
        "   border-bottom: 1px solid #E0E0E0;"
        "   padding: 2px;"
        "}"
        "QMenuBar::item {"
        "   background-color: transparent;"
        "   padding: 4px 12px;"
        "   margin: 2px;"
        "}"
        "QMenuBar::item:selected {"
        "   background-color: #E3F2FD;"
        "}"
        "QMenuBar::item:pressed {"
        "   background-color: #BBDEFB;"
        "}"
        "QMenu {"
        "   background-color: white;"
        "   border: 1px solid #E0E0E0;"
        "   padding: 4px;"
        "}"
        "QMenu::item {"
        "   padding: 6px 30px 6px 20px;"
        "   margin: 2px;"
        "}"
        "QMenu::item:selected {"
        "   background-color: #E3F2FD;"
        "}"
    );
    
    // 文件菜单
    m_fileMenu = menuBar->addMenu("文件(F)");
    m_newAction = m_fileMenu->addAction("新建压缩文件");
    m_openAction = m_fileMenu->addAction("打开压缩文件");
    
    m_recentFilesMenu = m_fileMenu->addMenu("最近打开的文件");
    updateHistoryUI(); // Update both menus

    m_fileMenu->addSeparator();
    m_exitAction = m_fileMenu->addAction("退出");
    
    // 设置菜单
    m_settingsMenu = menuBar->addMenu("设置(S)");
    m_associateAction = m_settingsMenu->addAction("关联 .huff 文件");

    // 测试菜单
    m_testMenu = menuBar->addMenu("测试(T)");
    m_speedTestAction = m_testMenu->addAction("随机数据压缩速度测试");
    m_mapPerformanceAction = m_testMenu->addAction("Map 性能对比测试");
    
    // 帮助菜单
    m_helpMenu = menuBar->addMenu("帮助(H)");
    m_aboutAction = m_helpMenu->addAction("关于");
}

void MainWindow::setupConnections() {
    // 按钮连接
    connect(m_openButton, &QPushButton::clicked, this, &MainWindow::onOpenArchive);
    connect(m_newButton, &QPushButton::clicked, this, &MainWindow::onNewArchive);
    
    // 菜单动作连接
    connect(m_newAction, &QAction::triggered, this, &MainWindow::onNewArchive);
    connect(m_openAction, &QAction::triggered, this, &MainWindow::onOpenArchive);
    connect(m_exitAction, &QAction::triggered, this, &QMainWindow::close);
    connect(m_associateAction, &QAction::triggered, this, &MainWindow::onAssociateFileExtension);
    connect(m_speedTestAction, &QAction::triggered, this, &MainWindow::onSpeedTestTriggered);
    connect(m_mapPerformanceAction, &QAction::triggered, this, &MainWindow::onMapPerformanceTriggered);
    connect(m_aboutAction, &QAction::triggered, this, &MainWindow::onAboutTriggered);
}

void MainWindow::onOpenArchive() {
    // 打开文件对话框
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "打开压缩文件",
        QString(),
        "HUFF 压缩文件 (*.huff);;所有文件 (*.*)"
    );
    
    if (!fileName.isEmpty()) {
        showArchiveView(fileName);
    }
}

void MainWindow::onNewArchive() {
    // 创建并显示新建压缩文件对话框
    NewArchiveDialog dialog(this);
    
    if (dialog.exec() == QDialog::Accepted) {
        // 压缩已经在对话框中完成
        // 获取生成的归档文件路径并打开
        Structure::String archivePath = dialog.getArchivePath();
        QString qArchivePath = QString::fromUtf8(archivePath.c_str());
        
        if (QFileInfo::exists(qArchivePath)) {
            showArchiveView(qArchivePath);
        }
    }
}

void MainWindow::onAssociateFileExtension() {
    if (Util::SystemUtils::isFileAssociationRegistered()) {
        QMessageBox::information(this, "提示", "文件关联已设置。\n在某些版本windows下无效\n可手动设置打开方式 -> 始终♥");
        return;
    }
    
    if (Util::SystemUtils::registerFileAssociation()) {
        QMessageBox::information(this, "成功", "成功关联 .huff 文件！\n您现在可以直接双击打开 .huff 文件。");
    } else {
        QMessageBox::warning(this, "失败", "无法设置文件关联。\n请尝试以管理员身份运行程序。");
    }
}

void MainWindow::updateHistoryUI() {
    auto history = Model::HistoryManager::instance().getHistory();
    
    auto updateMenu = [this, &history](QMenu* menu) {
        if (!menu) return;
        menu->clear();
        
        if (history.size() == 0) {
            QAction* action = menu->addAction("无历史记录");
            action->setEnabled(false);
        } else {
            for (int i = 0; i < history.size(); ++i) {
                QString path = QString::fromUtf8(history[i].c_str());
                QAction* action = menu->addAction(path);
                connect(action, &QAction::triggered, this, &MainWindow::onHistoryActionTriggered);
            }
        }
    };

    updateMenu(m_historyMenu);
    updateMenu(m_recentFilesMenu);
}

void MainWindow::onHistoryActionTriggered() {
    QAction* action = qobject_cast<QAction*>(sender());
    if (!action) return;
    
    QString path = action->text();
    if (!path.isEmpty() && QFileInfo::exists(path)) {
        showArchiveView(path);
    } else if (!path.isEmpty()) {
        QMessageBox::warning(this, "错误", "文件不存在或已被移动");
    }
}

void MainWindow::onSpeedTestTriggered() {
    showTestView();
    if (m_testView) m_testView->setTestType(0);
}

void MainWindow::onMapPerformanceTriggered() {
    showTestView();
    if (m_testView) m_testView->setTestType(5); // 5 will be Map Performance
}

void MainWindow::onAboutTriggered() {
    AboutDialog aboutDialog(this);
    aboutDialog.exec();
}

}
