#pragma once

#include <QMainWindow>

namespace View {

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // TODO: Setup UI
};

}
