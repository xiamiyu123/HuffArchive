//
// 由 xiami 创建于 2025/12/9。
//
#include <QApplication>
#include <QStyleFactory>
#include <QPalette>
#include <QStandardPaths>
#include <filesystem>
#include "view/MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // 强制使用 Fusion 风格和浅色调色板，规避 Windows 深色模式导致的字体颜色问题
    app.setStyle(QStyleFactory::create("Fusion"));
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(240, 240, 240));
    palette.setColor(QPalette::WindowText, Qt::black);
    palette.setColor(QPalette::Base, Qt::white);
    palette.setColor(QPalette::AlternateBase, QColor(233, 231, 227));
    palette.setColor(QPalette::ToolTipBase, Qt::white);
    palette.setColor(QPalette::ToolTipText, Qt::black);
    palette.setColor(QPalette::Text, Qt::black);
    palette.setColor(QPalette::Button, QColor(240, 240, 240));
    palette.setColor(QPalette::ButtonText, Qt::black);
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Link, QColor(42, 130, 218));
    palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    palette.setColor(QPalette::HighlightedText, Qt::white);
    app.setPalette(palette);
    
    // 设置应用程序信息
    app.setApplicationName("哈夫曼压缩工具");
    app.setOrganizationName("DataStructure");
    app.setApplicationVersion("1.0");
    
    // 创建并显示主窗口
    View::MainWindow mainWindow;
    mainWindow.show();

    // 检查命令行参数，如果有文件路径则直接打开
    if (argc > 1) {
        QString filePath = QString::fromLocal8Bit(argv[1]);
        std::string u8Path = filePath.toUtf8().constData();
        if (std::filesystem::exists(std::filesystem::path(reinterpret_cast<const char8_t*>(u8Path.c_str())))) {
            mainWindow.showArchiveView(filePath);
        }
    }
    
    int ret = app.exec();

    // 清理临时目录
    std::error_code ec;
    QString tempPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/HuffmanTool/tmp";
    std::filesystem::path tempDir = std::filesystem::path(reinterpret_cast<const char8_t*>(tempPath.toUtf8().constData()));
    if (std::filesystem::exists(tempDir)) {
        std::filesystem::remove_all(tempDir, ec);
    }

    return ret;
}