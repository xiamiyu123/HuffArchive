#include "view/MainWindow.h"
#include "view/ArchiveView.h"
#include "view/NewArchiveDialog.h"
#include "command/CompressDirectoryCommand.h"
#include <QIcon>
#include <QFont>
#include <QSize>
#include <QMessageBox>
#include <QFileInfo>
#include <QProgressDialog>

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
      m_archiveView(nullptr),
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
    // 如果已有 ArchiveView，先移除
    if (m_archiveView) {
        m_stackedWidget->removeWidget(m_archiveView);
        delete m_archiveView;
        m_archiveView = nullptr;
    }
    
    // 创建新的 ArchiveView
    m_archiveView = new ArchiveView(Structure::String(archivePath.toStdString().c_str()), this);
    m_stackedWidget->addWidget(m_archiveView);
    m_stackedWidget->setCurrentWidget(m_archiveView);
    
    // 更新窗口标题
    setWindowTitle(QString("哈夫曼压缩工具 - %1").arg(QFileInfo(archivePath).fileName()));
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
        Structure::String archivePath = dialog.getArchivePath();
        Structure::ArrayList<Structure::String> filesToCompress = dialog.getFilesToCompress();
        
        if (filesToCompress.empty()) {
            QMessageBox::warning(this, "错误", "没有选择要压缩的文件");
            return;
        }
        
        // 创建进度对话框
        QProgressDialog progressDialog("正在压缩文件...", "取消", 0, 0, this);
        progressDialog.setWindowModality(Qt::WindowModal);
        progressDialog.setMinimumDuration(0);
        progressDialog.show();
        
        // TODO: 实际的压缩逻辑
        // 这里需要实现将多个文件压缩到一个归档的功能
        // 当前的 CompressDirectoryCommand 只支持目录压缩
        
        progressDialog.close();
        
        QMessageBox::information(this, "提示", 
            QString("压缩功能待完善\n已选择 %1 个文件\n目标: %2")
                .arg(filesToCompress.size())
                .arg(QString::fromStdString(archivePath.c_str())));
    }
}

}
