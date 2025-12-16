#include <QtTest>
#include "model/DataModel.h"
#include "model/FileRecord.h"
#include "structure/String.h"

class TestModel : public QObject
{
    Q_OBJECT

private slots:
    void testFileRecord();
    void testDataModel();
};

void TestModel::testFileRecord()
{
    // Test Constructor & Path Parsing
    // Note: Path handling might depend on OS separators, but assuming standard forward slash or handled internally
    Structure::String path = "C:/test/file.txt";
    Model::FileRecord record(path);
    
    QCOMPARE(record.getFilePath().c_str(), "C:/test/file.txt");
    // Assuming getFileName extracts the name correctly
    QCOMPARE(record.getFileName().c_str(), "file.txt");
    QCOMPARE(record.getType(), Model::FileType::File);
    
    // Test Setters/Getters
    record.setOriginalSize(100);
    record.setCompressedSize(50);
    
    QCOMPARE(record.getOriginalSize(), 100);
    QCOMPARE(record.getCompressedSize(), 50);
    QCOMPARE(record.getCompressionRatio(), 0.5);
    
    record.setStatus(Model::FileStatus::Completed);
    QCOMPARE(record.getStatus(), Model::FileStatus::Completed);
}

void TestModel::testDataModel()
{
    Model::DataModel model;
    
    QCOMPARE(model.getFileCount(), 0);
    
    Model::FileRecord record1("file1.txt");
    Model::FileRecord record2("file2.txt");
    
    model.addFile(record1);
    model.addFile(record2);
    
    QCOMPARE(model.getFileCount(), 2);
    QCOMPARE(model.getFile(0).getFileName().c_str(), "file1.txt");
    
    model.removeFile(0);
    QCOMPARE(model.getFileCount(), 1);
    QCOMPARE(model.getFile(0).getFileName().c_str(), "file2.txt");
    
    model.clear();
    QCOMPARE(model.getFileCount(), 0);
}

QTEST_MAIN(TestModel)
#include "test_model.moc"
