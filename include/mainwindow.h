#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <memory>

class Editor;
class DocumentManager;
class SearchReplace;
class QTabWidget;
class QToolBar;
class QStatusBar;
class QAction;
class QMenu;

/**
 * @brief Main application window for the text editor
 *
 * Handles the main UI, menu bar, toolbars, and document management.
 * Provides the interface for all editor operations.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    // File operations
    void newFile();
    void openFile();
    void openRecent();
    void saveFile();
    void saveAsFile();
    void saveAllFiles();
    void closeCurrentFile();
    void closeAllFiles();
    void exitApplication();

    // Edit operations
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void selectAll();
    void deleteSelection();

    // View operations
    void toggleLineNumbers();
    void toggleWordWrap();
    void increaseFontSize();
    void decreaseFontSize();
    void resetFontSize();
    void changeTheme();

    // Search operations
    void openFindDialog();
    void openReplaceDialog();
    void findNext();
    void findPrevious();

    // Help operations
    void showAbout();
    void showAboutQt();

    // Tab management
    void onTabChanged(int index);
    void onTabCloseRequested(int index);
    void onDocumentModified();

private:
    void createMenuBar();
    void createToolBars();
    void createStatusBar();
    void createConnections();
    void readSettings();
    void writeSettings();
    void loadRecentFiles();
    bool maybeSave();
    bool maybeSaveAll();
    Editor *currentEditor() const;

    // UI Components
    QTabWidget *tabWidget;
    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QToolBar *searchToolBar;

    // Menus
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *viewMenu;
    QMenu *searchMenu;
    QMenu *recentFilesMenu;
    QMenu *helpMenu;

    // Actions
    QAction *newAction;
    QAction *openAction;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *saveAllAction;
    QAction *closeAction;
    QAction *exitAction;

    QAction *undoAction;
    QAction *redoAction;
    QAction *cutAction;
    QAction *copyAction;
    QAction *pasteAction;
    QAction *selectAllAction;
    QAction *deleteAction;

    QAction *findAction;
    QAction *replaceAction;
    QAction *findNextAction;
    QAction *findPrevAction;

    QAction *lineNumbersAction;
    QAction *wordWrapAction;
    QAction *increaseFontAction;
    QAction *decreaseFontAction;

    // Managers
    std::unique_ptr<DocumentManager> documentManager;
    std::unique_ptr<SearchReplace> searchReplace;

    // Settings
    QStringList recentFiles;
    int currentFontSize;
    QString currentTheme;
};

#endif // MAINWINDOW_H
