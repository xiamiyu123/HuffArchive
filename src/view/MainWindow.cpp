#include "view/MainWindow.h"
#include <QIcon>
#include <QFont>
#include <QSize>

namespace View {

MainWindow::MainWindow(QWidget *parent) 
    : QMainWindow(parent),
      m_centralWidget(nullptr),
      m_mainLayout(nullptr),
      m_buttonLayout(nullptr),
      m_openButton(nullptr),
      m_newButton(nullptr),
      m_fileMenu(nullptr),
      m_editMenu(nullptr),
      m_searchMenu(nullptr),
      m_optionsMenu(nullptr),
      m_viewMenu(nullptr),
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
    
    // 创建中心部件
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
    
    // 创建主布局
    m_mainLayout = new QVBoxLayout(m_centralWidget);
    m_mainLayout->setContentsMargins(50, 50, 50, 50);
    m_mainLayout->setSpacing(20);
    
    // 添加顶部弹簧（垂直居中）
    m_mainLayout->addStretch(1);
    
    // 创建按钮布局
    m_buttonLayout = new QHBoxLayout();
    m_buttonLayout->setSpacing(30);
    
    // 创建"打开压缩文件"按钮
    m_openButton = new QPushButton(this);
    m_openButton->setText("打开压缩文件");
    m_openButton->setMinimumSize(200, 120);
    m_openButton->setMaximumSize(250, 150);
    m_openButton->setCursor(Qt::PointingHandCursor);
    
    // 设置按钮样式
    m_openButton->setStyleSheet(
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
    m_mainLayout->addLayout(m_buttonLayout);
    
    // 添加底部弹簧（垂直居中）
    m_mainLayout->addStretch(1);
    
    // 设置窗口背景色
    setStyleSheet("QMainWindow { background-color: #1890FF; }");
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
    m_fileMenu->addSeparator();
    m_exitAction = m_fileMenu->addAction("退出");
    
    // 编辑菜单
    m_editMenu = menuBar->addMenu("编辑(E)");
    m_editMenu->addAction("剪切");
    m_editMenu->addAction("复制");
    m_editMenu->addAction("粘贴");
    
    // 查找菜单
    m_searchMenu = menuBar->addMenu("查找(I)");
    m_searchMenu->addAction("查找文件");
    
    // 选项菜单
    m_optionsMenu = menuBar->addMenu("选项(O)");
    m_optionsMenu->addAction("设置");
    
    // 视图菜单
    m_viewMenu = menuBar->addMenu("视图(V)");
    m_viewMenu->addAction("工具栏");
    m_viewMenu->addAction("状态栏");
    
    // 帮助菜单
    m_helpMenu = menuBar->addMenu("帮助(H)");
    m_helpMenu->addAction("关于");
}

void MainWindow::setupConnections() {
    // 按钮连接
    connect(m_openButton, &QPushButton::clicked, this, &MainWindow::onOpenArchive);
    connect(m_newButton, &QPushButton::clicked, this, &MainWindow::onNewArchive);
    
    // 菜单动作连接
    connect(m_newAction, &QAction::triggered, this, &MainWindow::onNewArchive);
    connect(m_openAction, &QAction::triggered, this, &MainWindow::onOpenArchive);
    connect(m_exitAction, &QAction::triggered, this, &QMainWindow::close);
}

void MainWindow::onOpenArchive() {
    // TODO: 实现打开压缩文件功能
}

void MainWindow::onNewArchive() {
    // TODO: 实现新建压缩文件功能
}

}
