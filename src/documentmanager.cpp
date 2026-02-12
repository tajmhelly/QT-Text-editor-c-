#include "documentmanager.h"
#include "editor.h"

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QStandardPaths>
#include <QDir>
#include <QSettings>
#include <QDebug>
#include <QDateTime>

DocumentManager::DocumentManager(QObject *parent)
    : QObject(parent), maxRecentFiles(10), autoSaveEnabled(false), autoSaveInterval(60000)
{
    configDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    backupDir = configDir + "/backups";

    // Create directories if they don't exist
    QDir().mkpath(configDir);
    QDir().mkpath(backupDir);

    loadSettings();
}

DocumentManager::~DocumentManager()
{
    saveSettings();
}

bool DocumentManager::openFile(const QString &fileName, Editor *editor)
{
    if (!editor)
        return false;

    QString content;
    if (!readFile(fileName, content))
    {
        return false;
    }

    editor->setPlainText(content);
    editor->setFileName(fileName);
    editor->document()->setModified(false);

    addRecentFile(fileName);
    emit fileOpened(fileName);

    return true;
}

bool DocumentManager::saveFile(Editor *editor)
{
    if (!editor)
        return false;

    QString fileName = editor->fileName();
    if (fileName.contains("Untitled"))
    {
        return false;
    }

    return writeFile(fileName, editor->toPlainText());
}

bool DocumentManager::saveFileAs(Editor *editor, const QString &newFileName)
{
    if (!editor)
        return false;

    if (writeFile(newFileName, editor->toPlainText()))
    {
        editor->setFileName(newFileName);
        addRecentFile(newFileName);
        emit fileSaved(newFileName);
        return true;
    }
    return false;
}

bool DocumentManager::closeFile(Editor *editor)
{
    if (!editor)
        return false;

    emit fileClosed(editor->fileName());
    return true;
}

bool DocumentManager::fileExists(const QString &fileName) const
{
    return QFile::exists(fileName);
}

QString DocumentManager::getFileInfo(const QString &fileName) const
{
    QFileInfo fileInfo(fileName);
    if (!fileInfo.exists())
    {
        return "File does not exist";
    }

    QString info = QString("Name: %1\nPath: %2\nSize: %3 bytes\nModified: %4")
                       .arg(fileInfo.fileName())
                       .arg(fileInfo.absolutePath())
                       .arg(fileInfo.size())
                       .arg(fileInfo.lastModified().toString("yyyy-MM-dd hh:mm:ss"));

    return info;
}

qint64 DocumentManager::getFileSize(const QString &fileName) const
{
    QFileInfo fileInfo(fileName);
    return fileInfo.size();
}

QString DocumentManager::getFileType(const QString &fileName) const
{
    QFileInfo fileInfo(fileName);
    return fileInfo.suffix();
}

void DocumentManager::addRecentFile(const QString &fileName)
{
    updateRecentFiles(fileName);
    emit recentFilesChanged();
    saveSettings();
}

void DocumentManager::removeRecentFile(const QString &fileName)
{
    recentFiles.removeAll(fileName);
    emit recentFilesChanged();
    saveSettings();
}

void DocumentManager::clearRecentFiles()
{
    recentFiles.clear();
    emit recentFilesChanged();
    saveSettings();
}

void DocumentManager::createBackup(const QString &fileName)
{
    if (!QFile::exists(fileName))
        return;

    QFileInfo fileInfo(fileName);
    QString backupFileName = backupDir + "/" + fileInfo.fileName() + ".bak";

    QFile file(fileName);
    QFile backup(backupFileName);

    if (file.open(QIODevice::ReadOnly))
    {
        if (backup.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            backup.write(file.readAll());
            backup.close();
        }
        file.close();
    }
}

QString DocumentManager::getBackupFile(const QString &fileName) const
{
    QFileInfo fileInfo(fileName);
    return backupDir + "/" + fileInfo.fileName() + ".bak";
}

bool DocumentManager::hasBackup(const QString &fileName) const
{
    return QFile::exists(getBackupFile(fileName));
}

void DocumentManager::saveSession(const QStringList &openFiles)
{
    QSettings settings("TextEditor", "TextEditor");
    settings.setValue("session/openFiles", openFiles);
}

QStringList DocumentManager::loadSession() const
{
    QSettings settings("TextEditor", "TextEditor");
    return settings.value("session/openFiles", QStringList()).toStringList();
}

void DocumentManager::clearSession()
{
    QSettings settings("TextEditor", "TextEditor");
    settings.remove("session/openFiles");
}

bool DocumentManager::readFile(const QString &fileName, QString &content) const
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return false;
    }

    QTextStream in(&file);
    content = in.readAll();
    file.close();

    return true;
}

bool DocumentManager::writeFile(const QString &fileName, const QString &content)
{
    // Create backup before writing
    if (QFile::exists(fileName))
    {
        QFile::copy(fileName, fileName + ".bak");
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return false;
    }

    QTextStream out(&file);
    out << content;
    file.close();

    emit fileSaved(fileName);
    return true;
}

void DocumentManager::updateRecentFiles(const QString &fileName)
{
    recentFiles.removeAll(fileName);
    recentFiles.prepend(fileName);

    while (recentFiles.size() > maxRecentFiles)
    {
        recentFiles.removeLast();
    }
}

QString DocumentManager::detectEncoding(const QString &fileName) const
{
    // TODO: Implement encoding detection
    return "UTF-8";
}

bool DocumentManager::isTextFile(const QString &fileName) const
{
    QFileInfo fileInfo(fileName);
    QString suffix = fileInfo.suffix().toLower();

    QStringList textExtensions = {
        "txt", "cpp", "h", "hpp", "c", "cc", "cxx",
        "py", "js", "ts", "json", "xml", "html", "css",
        "java", "cs", "rb", "go", "rs", "php", "swift",
        "sh", "bash", "sql", "md", "yml", "yaml", "toml",
        "ini", "conf", "config", "gradle", "maven", "cmake"};

    return textExtensions.contains(suffix);
}

void DocumentManager::loadSettings()
{
    QSettings settings("TextEditor", "TextEditor");
    recentFiles = settings.value("recentFiles", QStringList()).toStringList();
    maxRecentFiles = settings.value("maxRecentFiles", 10).toInt();
    autoSaveEnabled = settings.value("autoSaveEnabled", false).toBool();
    autoSaveInterval = settings.value("autoSaveInterval", 60000).toInt();
}

void DocumentManager::saveSettings()
{
    QSettings settings("TextEditor", "TextEditor");
    settings.setValue("recentFiles", recentFiles);
    settings.setValue("maxRecentFiles", maxRecentFiles);
    settings.setValue("autoSaveEnabled", autoSaveEnabled);
    settings.setValue("autoSaveInterval", autoSaveInterval);
}
