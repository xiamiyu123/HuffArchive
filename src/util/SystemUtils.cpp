#include "util/SystemUtils.h"
#include <QSettings>
#include <QCoreApplication>
#include <QDir>
#include <QString>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#endif

namespace Util {

#ifdef _WIN32
bool setRegistryValue(HKEY hKeyRoot, const QString& subKey, const QString& valueName, const QString& data) {
    HKEY hKey;
    std::wstring wSubKey = subKey.toStdWString();
    LONG result = RegCreateKeyExW(hKeyRoot, wSubKey.c_str(), 0, NULL, 
                                  REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);
    if (result != ERROR_SUCCESS) {
        return false;
    }

    std::wstring wValueName = valueName.toStdWString();
    std::wstring wData = data.toStdWString();
    result = RegSetValueExW(hKey, valueName.isEmpty() ? NULL : wValueName.c_str(), 
                            0, REG_SZ, (const BYTE*)wData.c_str(), 
                            (wData.length() + 1) * sizeof(wchar_t));
    
    RegCloseKey(hKey);
    return result == ERROR_SUCCESS;
}
#endif

bool SystemUtils::registerFileAssociation() {
#ifdef _WIN32
    QString appPath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
    QString className = "HuffmanArchive";
    QString ext = ".huff";
    QString description = "Huffman Archive";
    
    // 1. HKCU\Software\Classes\.huff -> Default = HuffmanArchive
    if (!setRegistryValue(HKEY_CURRENT_USER, "Software\\Classes\\" + ext, "", className)) return false;
    
    // 2. HKCU\Software\Classes\HuffmanArchive -> Default = Huffman Archive
    if (!setRegistryValue(HKEY_CURRENT_USER, "Software\\Classes\\" + className, "", description)) return false;
    
    // 3. HKCU\Software\Classes\HuffmanArchive\DefaultIcon -> Default = "appPath",0
    QString iconPath = "\"" + appPath + "\",0";
    if (!setRegistryValue(HKEY_CURRENT_USER, "Software\\Classes\\" + className + "\\DefaultIcon", "", iconPath)) return false;
    
    // 4. HKCU\Software\Classes\HuffmanArchive\shell\open\command -> Default = "appPath" "%1"
    QString command = "\"" + appPath + "\" \"%1\"";
    if (!setRegistryValue(HKEY_CURRENT_USER, "Software\\Classes\\" + className + "\\shell\\open\\command", "", command)) return false;
    
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);

    return true;
#else
    // Linux/Mac implementation would go here (e.g., .desktop files)
    return false;
#endif
}

bool SystemUtils::isFileAssociationRegistered() {
#ifdef _WIN32
    QString className = "HuffmanArchive";
    QString subKey = "Software\\Classes\\" + className + "\\shell\\open\\command";
    
    HKEY hKey;
    LONG result = RegOpenKeyExW(HKEY_CURRENT_USER, subKey.toStdWString().c_str(), 0, KEY_READ, &hKey);
    if (result != ERROR_SUCCESS) return false;
    
    wchar_t buffer[MAX_PATH * 2];
    DWORD bufferSize = sizeof(buffer);
    result = RegQueryValueExW(hKey, NULL, NULL, NULL, (LPBYTE)buffer, &bufferSize);
    RegCloseKey(hKey);
    
    if (result != ERROR_SUCCESS) return false;
    
    QString command = QString::fromWCharArray(buffer);
    QString appPath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
    
    return command.contains(appPath);
#else
    return false;
#endif
}

}
