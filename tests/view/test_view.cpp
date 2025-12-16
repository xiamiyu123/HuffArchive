#include <QtTest>
#include <QApplication>
#include "view/MainWindow.h"

class TestView : public QObject
{
    Q_OBJECT

private slots:
    void testMainWindow();
};

void TestView::testMainWindow()
{
    // Ensure we have a GUI application instance
    // QTEST_MAIN creates a QApplication for us
    
    View::MainWindow w;
    w.show();
    
    QVERIFY(w.isVisible());
    
    // Basic check to ensure it didn't crash immediately
    QVERIFY(w.width() > 0);
    QVERIFY(w.height() > 0);
    
    w.close();
}

QTEST_MAIN(TestView)
#include "test_view.moc"
