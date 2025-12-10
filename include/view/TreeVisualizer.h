#pragma once

#include <QWidget>
#include "structure/HuffmanTree.h"

namespace View {

class TreeVisualizer : public QWidget {
    Q_OBJECT

public:
    explicit TreeVisualizer(QWidget *parent = nullptr);
    ~TreeVisualizer();

    void setTree(const Structure::HuffmanTree* tree);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    const Structure::HuffmanTree* m_tree;
    
    void drawNode(QPainter& painter, int nodeIndex, int x, int y, int hGap);
};

}
