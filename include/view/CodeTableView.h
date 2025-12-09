#pragma once

#include <QWidget>

namespace View {

class CodeTableView : public QWidget {
    Q_OBJECT

public:
    explicit CodeTableView(QWidget *parent = nullptr);
    ~CodeTableView();

    // TODO: Add table view for codes
};

}
