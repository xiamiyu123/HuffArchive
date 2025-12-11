//
// Created by xiami on 2025/12/9.
//
#include <QApplication>
#include "view/MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // 设置应用程序信息
    app.setApplicationName("哈夫曼压缩工具");
    app.setOrganizationName("DataStructure");
    app.setApplicationVersion("1.0");
    
    // 创建并显示主窗口
    View::MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}