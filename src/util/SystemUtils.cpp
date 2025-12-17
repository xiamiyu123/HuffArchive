#include "util/SystemUtils.h"
#include <QSettings>
#include <QCoreApplication>
#include <QDir>
#include <QString>

namespace Util {

bool SystemUtils::registerFileAssociation() {
#ifdef _WIN32
    QString appPath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
    QString className = "HuffmanArchive";
    QString ext = ".huff";
    
    // HKEY_CURRENT_USER\Software\Classes
    QSettings classes(QSettings::UserScope, "Microsoft", "Windows\\CurrentVersion\\Explorer\\FileExts");
    // Note: QSettings with NativeFormat on Windows usually maps to HKCU\Software\Organization\App
    // To write to HKCU\Software\Classes directly, we use QSettings with specific path
    
    // Using QSettings to write to registry directly
    QSettings settings("HKEY_CURRENT_USER\\Software\\Classes", QSettings::NativeFormat);
    
    // 1. Associate .huff with HuffmanArchive class
    settings.setValue(ext + "/.", className);
    
    // 2. Define HuffmanArchive class
    settings.setValue(className + "/.", "Huffman Archive");
    
    // 3. Set Default Icon
    settings.setValue(className + "/DefaultIcon/.", QString("\"" + appPath + "\",0"));
    
    // 4. Set Open Command
    settings.setValue(className + "/shell/open/command/.", QString("\"" + appPath + "\" \"%1\""));
    
    return true;
#else
    // Linux/Mac implementation would go here (e.g., .desktop files)
    return false;
#endif
}

bool SystemUtils::isFileAssociationRegistered() {
#ifdef _WIN32
    QSettings settings("HKEY_CURRENT_USER\\Software\\Classes", QSettings::NativeFormat);
    QString cmd = settings.value("HuffmanArchive/shell/open/command/.").toString();
    QString appPath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
    return cmd.contains(appPath);
#else
    return false;
#endif
}

}
