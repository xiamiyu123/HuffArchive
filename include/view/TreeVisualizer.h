#pragma once

#include <QWidget>

namespace View {

class TreeVisualizer : public QWidget {
    Q_OBJECT

public:
    explicit TreeVisualizer(QWidget *parent = nullptr);
    ~TreeVisualizer();

    // TODO: Add drawing logic for Huffman Tree
};

}
