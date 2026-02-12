/tmp/TextEditor/run.sh#ifndef DOCUMENTMANAGER_H
#define DOCUMENTMANAGER_H

#include <QString>
#include <QVector>
#include <QJsonDocument>
#include <memory>

class Editor;

/**
 * @brief Manages document loading, saving, and file operations
 *
 * Handles all file I/O, recent files tracking, and document metadata.
 */
class DocumentManager : public QObject
{
    Q_OBJECT

public:
    explicit DocumentManager(QObject *parent = nullptr);
    ~DocumentManager();

    // File operations
    bool openFile(const QString &fileName, Editor *editor);
    bool saveFile(Editor *editor);
    bool saveFileAs(Editor *editor, const QString &newFileName);
    bool closeFile(Editor *editor);

    // File info
    bool fileExists(const QString &fileName) const;
    QString getFileInfo(const QString &fileName) const;
    qint64 getFileSize(const QString &fileName) const;
    QString getFileType(const QString &fileName) const;

    // Recent files
    void addRecentFile(const QString &fileName);
    void removeRecentFile(const QString &fileName);
    QStringList getRecentFiles() const { return recentFiles; }
    void clearRecentFiles();

    // Backup and recovery
    void createBackup(const QString &fileName);
    QString getBackupFile(const QString &fileName) const;
    bool hasBackup(const QString &fileName) const;

    // Session management
    void saveSession(const QStringList &openFiles);
    QStringList loadSession() const;
    void clearSession();

    // Settings
    void setMaxRecentFiles(int max) { maxRecentFiles = max; }
    void setAutoSaveEnabled(bool enabled) { autoSaveEnabled = enabled; }
    void setAutoSaveInterval(int ms) { autoSaveInterval = ms; }

signals:
    void fileOpened(const QString &fileName);
    void fileSaved(const QString &fileName);
    void fileModified(const QString &fileName);
    void fileClosed(const QString &fileName);
    void recentFilesChanged();

private:
    bool readFile(const QString &fileName, QString &content) const;
    bool writeFile(const QString &fileName, const QString &content);
    void updateRecentFiles(const QString &fileName);
    void loadSettings();
    void saveSettings();

    // File encoding helpers
    QString detectEncoding(const QString &fileName) const;
    bool isTextFile(const QString &fileName) const;

    // Settings and state
    QStringList recentFiles;
    int maxRecentFiles;
    bool autoSaveEnabled;
    int autoSaveInterval;
    QString configDir;
    QString backupDir;
};

#endif // DOCUMENTMANAGER_H
