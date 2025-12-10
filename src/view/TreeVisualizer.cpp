#include "view/TreeVisualizer.h"
#include <QPainter>
#include <QPaintEvent>

namespace View {

TreeVisualizer::TreeVisualizer(QWidget *parent) 
    : QWidget(parent), m_tree(nullptr) {
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
}

TreeVisualizer::~TreeVisualizer() {
}

void TreeVisualizer::setTree(const Structure::HuffmanTree* tree) {
    m_tree = tree;
    update();
}

void TreeVisualizer::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    if (!m_tree) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int rootIndex = m_tree->getRootIndex();
    if (rootIndex == -1) return;

    // Start drawing from the top center
    drawNode(painter, rootIndex, width() / 2, 40, width() / 4);
}

void TreeVisualizer::drawNode(QPainter& painter, int nodeIndex, int x, int y, int hGap) {
    const auto& nodes = m_tree->getNodes();
    if (nodeIndex < 0 || nodeIndex >= nodes.size()) return;

    const auto& node = nodes[nodeIndex];
    int vGap = 60;
    int radius = 20;

    // Draw connections first so they are behind nodes
    if (node.lchild != -1) {
        painter.drawLine(x, y, x - hGap, y + vGap);
        drawNode(painter, node.lchild, x - hGap, y + vGap, hGap / 2);
    }
    if (node.rchild != -1) {
        painter.drawLine(x, y, x + hGap, y + vGap);
        drawNode(painter, node.rchild, x + hGap, y + vGap, hGap / 2);
    }

    // Draw node
    painter.setBrush(Qt::white);
    painter.setPen(Qt::black);
    painter.drawEllipse(QPoint(x, y), radius, radius);

    // Draw text (weight)
    QString text = QString::number(node.weight);
    
    // If it's a leaf node, show the character
    if (node.lchild == -1 && node.rchild == -1) {
        unsigned char c = node.data;
        if (c >= 32 && c <= 126) {
            text = QString("'%1'\n%2").arg(QChar(c)).arg(node.weight);
        } else {
            text = QString("0x%1\n%2").arg(QString::number(c, 16).toUpper()).arg(node.weight);
        }
    }

    painter.drawText(QRect(x - radius, y - radius, 2 * radius, 2 * radius), 
                     Qt::AlignCenter, text);
}

}
