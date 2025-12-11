#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHeaderView>
#include "structure/HashMap.h"
#include "structure/String.h"

namespace View {

class CodeTableView : public QWidget {
    Q_OBJECT

public:
    explicit CodeTableView(QWidget *parent = nullptr);
    ~CodeTableView();

    void updateTable(const Structure::HashMap<unsigned char, int>& frequencies,
                     const Structure::HashMap<unsigned char, Structure::String>& codes);
    void clear();

private:
    QTableWidget* m_table;
    void setupUi();
};

}
