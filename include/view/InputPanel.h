#pragma once

#include <QWidget>

namespace View {

class InputPanel : public QWidget {
    Q_OBJECT

public:
    explicit InputPanel(QWidget *parent = nullptr);
    ~InputPanel();

    // TODO: Add input controls
};

}
