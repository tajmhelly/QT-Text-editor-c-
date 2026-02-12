#include "mainwindow.h"
#include "editor.h"
#include "documentmanager.h"
#include "searchreplace.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QToolBar>
#include <QStatusBar>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QClipboard>
#include <QMimeData>
#include <QSettings>
#include <QDesktopServices>
#include <QUrl>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), tabWidget(nullptr), documentManager(std::make_unique<DocumentManager>(this)), searchReplace(std::make_unique<SearchReplace>(this)), currentFontSize(12), currentTheme("Light")
{
    setWindowTitle("Professional Text Editor");
    setWindowIcon(QIcon());
    setGeometry(100, 100, 1200, 800);

    // Create central widget
    tabWidget = new QTabWidget(this);
    tabWidget->setTabsClosable(true);
    tabWidget->setMovable(true);
    setCentralWidget(tabWidget);

    // Create UI components
    createMenuBar();
    createToolBars();
    createStatusBar();
    createConnections();

    // Read settings
    readSettings();

    // Enable drag and drop
    setAcceptDrops(true);
}

MainWindow::~MainWindow()
{
    writeSettings();
}

void MainWindow::createMenuBar()
{
    // File Menu
    fileMenu = menuBar()->addMenu(tr("&File"));

    newAction = fileMenu->addAction(tr("&New"));
    newAction->setShortcut(QKeySequence::New);
    connect(newAction, &QAction::triggered, this, &MainWindow::newFile);

    openAction = fileMenu->addAction(tr("&Open"));
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);

    fileMenu->addSeparator();

    recentFilesMenu = fileMenu->addMenu(tr("&Recent Files"));
    loadRecentFiles();

    fileMenu->addSeparator();

    saveAction = fileMenu->addAction(tr("&Save"));
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveFile);

    saveAsAction = fileMenu->addAction(tr("Save &As..."));
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    connect(saveAsAction, &QAction::triggered, this, &MainWindow::saveAsFile);

    saveAllAction = fileMenu->addAction(tr("Save A&ll"));
    saveAllAction->setShortcut(Qt::CTRL | Qt::SHIFT | Qt::Key_S);
    connect(saveAllAction, &QAction::triggered, this, &MainWindow::saveAllFiles);

    fileMenu->addSeparator();

    closeAction = fileMenu->addAction(tr("&Close"));
    closeAction->setShortcut(QKeySequence::Close);
    connect(closeAction, &QAction::triggered, this, &MainWindow::closeCurrentFile);

    fileMenu->addSeparator();

    exitAction = fileMenu->addAction(tr("E&xit"));
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &MainWindow::exitApplication);

    // Edit Menu
    editMenu = menuBar()->addMenu(tr("&Edit"));

    undoAction = editMenu->addAction(tr("&Undo"));
    undoAction->setShortcut(QKeySequence::Undo);
    connect(undoAction, &QAction::triggered, this, &MainWindow::undo);

    redoAction = editMenu->addAction(tr("&Redo"));
    redoAction->setShortcut(QKeySequence::Redo);
    connect(redoAction, &QAction::triggered, this, &MainWindow::redo);

    editMenu->addSeparator();

    cutAction = editMenu->addAction(tr("Cu&t"));
    cutAction->setShortcut(QKeySequence::Cut);
    connect(cutAction, &QAction::triggered, this, &MainWindow::cut);

    copyAction = editMenu->addAction(tr("&Copy"));
    copyAction->setShortcut(QKeySequence::Copy);
    connect(copyAction, &QAction::triggered, this, &MainWindow::copy);

    pasteAction = editMenu->addAction(tr("&Paste"));
    pasteAction->setShortcut(QKeySequence::Paste);
    connect(pasteAction, &QAction::triggered, this, &MainWindow::paste);

    editMenu->addSeparator();

    selectAllAction = editMenu->addAction(tr("Select &All"));
    selectAllAction->setShortcut(QKeySequence::SelectAll);
    connect(selectAllAction, &QAction::triggered, this, &MainWindow::selectAll);

    deleteAction = editMenu->addAction(tr("&Delete"));
    deleteAction->setShortcut(QKeySequence::Delete);
    connect(deleteAction, &QAction::triggered, this, &MainWindow::deleteSelection);

    // View Menu
    viewMenu = menuBar()->addMenu(tr("&View"));

    lineNumbersAction = viewMenu->addAction(tr("Show &Line Numbers"));
    lineNumbersAction->setCheckable(true);
    lineNumbersAction->setChecked(true);
    connect(lineNumbersAction, &QAction::triggered, this, &MainWindow::toggleLineNumbers);

    wordWrapAction = viewMenu->addAction(tr("&Word Wrap"));
    wordWrapAction->setCheckable(true);
    wordWrapAction->setChecked(false);
    connect(wordWrapAction, &QAction::triggered, this, &MainWindow::toggleWordWrap);

    viewMenu->addSeparator();

    increaseFontAction = viewMenu->addAction(tr("Increase Font &Size"));
    increaseFontAction->setShortcut(Qt::CTRL | Qt::Key_Plus);
    connect(increaseFontAction, &QAction::triggered, this, &MainWindow::increaseFontSize);

    decreaseFontAction = viewMenu->addAction(tr("Decrease Font Si&ze"));
    decreaseFontAction->setShortcut(Qt::CTRL | Qt::Key_Minus);
    connect(decreaseFontAction, &QAction::triggered, this, &MainWindow::decreaseFontSize);

    // Search Menu
    searchMenu = menuBar()->addMenu(tr("&Search"));

    findAction = searchMenu->addAction(tr("&Find"));
    findAction->setShortcut(QKeySequence::Find);
    connect(findAction, &QAction::triggered, this, &MainWindow::openFindDialog);

    replaceAction = searchMenu->addAction(tr("&Replace"));
    replaceAction->setShortcut(QKeySequence::Replace);
    connect(replaceAction, &QAction::triggered, this, &MainWindow::openReplaceDialog);

    searchMenu->addSeparator();

    findNextAction = searchMenu->addAction(tr("Find &Next"));
    findNextAction->setShortcut(QKeySequence::FindNext);
    connect(findNextAction, &QAction::triggered, this, &MainWindow::findNext);

    findPrevAction = searchMenu->addAction(tr("Find &Previous"));
    findPrevAction->setShortcut(QKeySequence::FindPrevious);
    connect(findPrevAction, &QAction::triggered, this, &MainWindow::findPrevious);

    // Help Menu
    helpMenu = menuBar()->addMenu(tr("&Help"));

    QAction *aboutAction = helpMenu->addAction(tr("&About"));
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAbout);

    QAction *aboutQtAction = helpMenu->addAction(tr("About &Qt"));
    connect(aboutQtAction, &QAction::triggered, this, &MainWindow::showAboutQt);
}

void MainWindow::createToolBars()
{
    // File toolbar
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->setObjectName("FileToolBar");
    fileToolBar->addAction(newAction);
    fileToolBar->addAction(openAction);
    fileToolBar->addAction(saveAction);
    fileToolBar->addSeparator();

    // Edit toolbar
    editToolBar = addToolBar(tr("Edit"));
    editToolBar->setObjectName("EditToolBar");
    editToolBar->addAction(undoAction);
    editToolBar->addAction(redoAction);
    editToolBar->addSeparator();
    editToolBar->addAction(cutAction);
    editToolBar->addAction(copyAction);
    editToolBar->addAction(pasteAction);
    editToolBar->addSeparator();

    // Search toolbar
    searchToolBar = addToolBar(tr("Search"));
    searchToolBar->setObjectName("SearchToolBar");
    searchToolBar->addAction(findAction);
    searchToolBar->addAction(replaceAction);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::createConnections()
{
    connect(tabWidget, QOverload<int>::of(&QTabWidget::currentChanged),
            this, &MainWindow::onTabChanged);
    connect(tabWidget, &QTabWidget::tabCloseRequested,
            this, &MainWindow::onTabCloseRequested);
}

void MainWindow::newFile()
{
    Editor *editor = new Editor(this);
    editor->setFileName(QString("Untitled %1").arg(tabWidget->count() + 1));
    int index = tabWidget->addTab(editor, editor->fileName());
    tabWidget->setCurrentIndex(index);

    connect(editor->document(), &QTextDocument::modificationChanged,
            this, &MainWindow::onDocumentModified);

    editor->setFocus();
}

void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open File"), "",
                                                    tr("All Files (*);;Text Files (*.txt);;C++ Files (*.cpp *.h);;Python Files (*.py);;JSON Files (*.json)"));

    if (!fileName.isEmpty())
    {
        Editor *editor = nullptr;

        // Check if file is already open
        for (int i = 0; i < tabWidget->count(); ++i)
        {
            Editor *e = qobject_cast<Editor *>(tabWidget->widget(i));
            if (e && e->fileName() == fileName)
            {
                tabWidget->setCurrentIndex(i);
                return;
            }
        }

        // Create new editor
        editor = new Editor(this);
        if (documentManager->openFile(fileName, editor))
        {
            int index = tabWidget->addTab(editor, QFileInfo(fileName).fileName());
            tabWidget->setCurrentIndex(index);

            connect(editor->document(), &QTextDocument::modificationChanged,
                    this, &MainWindow::onDocumentModified);

            statusBar()->showMessage(tr("Opened: %1").arg(fileName), 5000);
        }
        else
        {
            delete editor;
            QMessageBox::warning(this, tr("Open File"),
                                 tr("Cannot open file:\n%1").arg(fileName));
        }
    }
}

void MainWindow::openRecent()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        // TODO: Open recent file
    }
}

void MainWindow::saveFile()
{
    Editor *editor = currentEditor();
    if (editor)
    {
        if (editor->fileName().contains("Untitled"))
        {
            saveAsFile();
        }
        else if (documentManager->saveFile(editor))
        {
            statusBar()->showMessage(tr("File saved: %1").arg(editor->fileName()), 5000);
        }
    }
}

void MainWindow::saveAsFile()
{
    Editor *editor = currentEditor();
    if (!editor)
        return;

    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save File As"), editor->fileName(),
                                                    tr("All Files (*);;Text Files (*.txt);;C++ Files (*.cpp);;Python Files (*.py);;JSON Files (*.json)"));

    if (!fileName.isEmpty())
    {
        if (documentManager->saveFileAs(editor, fileName))
        {
            int index = tabWidget->currentIndex();
            tabWidget->setTabText(index, QFileInfo(fileName).fileName());
            statusBar()->showMessage(tr("File saved: %1").arg(fileName), 5000);
        }
    }
}

void MainWindow::saveAllFiles()
{
    for (int i = 0; i < tabWidget->count(); ++i)
    {
        Editor *editor = qobject_cast<Editor *>(tabWidget->widget(i));
        if (editor && editor->isModified())
        {
            if (!editor->fileName().contains("Untitled"))
            {
                documentManager->saveFile(editor);
            }
        }
    }
    statusBar()->showMessage(tr("All files saved"), 5000);
}

void MainWindow::closeCurrentFile()
{
    int index = tabWidget->currentIndex();
    if (index >= 0)
    {
        onTabCloseRequested(index);
    }
}

void MainWindow::closeAllFiles()
{
    while (tabWidget->count() > 0)
    {
        onTabCloseRequested(0);
    }
}

void MainWindow::exitApplication()
{
    if (maybeSaveAll())
    {
        qApp->quit();
    }
}

void MainWindow::undo()
{
    Editor *editor = currentEditor();
    if (editor)
        editor->undo();
}

void MainWindow::redo()
{
    Editor *editor = currentEditor();
    if (editor)
        editor->redo();
}

void MainWindow::cut()
{
    Editor *editor = currentEditor();
    if (editor)
        editor->cut();
}

void MainWindow::copy()
{
    Editor *editor = currentEditor();
    if (editor)
        editor->copy();
}

void MainWindow::paste()
{
    Editor *editor = currentEditor();
    if (editor)
        editor->paste();
}

void MainWindow::selectAll()
{
    Editor *editor = currentEditor();
    if (editor)
        editor->selectAll();
}

void MainWindow::deleteSelection()
{
    Editor *editor = currentEditor();
    if (editor)
    {
        QTextCursor cursor = editor->textCursor();
        cursor.removeSelectedText();
        editor->setTextCursor(cursor);
    }
}

void MainWindow::toggleLineNumbers()
{
    for (int i = 0; i < tabWidget->count(); ++i)
    {
        Editor *editor = qobject_cast<Editor *>(tabWidget->widget(i));
        if (editor)
        {
            editor->setShowLineNumbers(lineNumbersAction->isChecked());
        }
    }
}

void MainWindow::toggleWordWrap()
{
    for (int i = 0; i < tabWidget->count(); ++i)
    {
        Editor *editor = qobject_cast<Editor *>(tabWidget->widget(i));
        if (editor)
        {
            editor->setWordWrapMode(wordWrapAction->isChecked());
        }
    }
}

void MainWindow::increaseFontSize()
{
    currentFontSize++;
    for (int i = 0; i < tabWidget->count(); ++i)
    {
        Editor *editor = qobject_cast<Editor *>(tabWidget->widget(i));
        if (editor)
        {
            editor->setFontSize(currentFontSize);
        }
    }
}

void MainWindow::decreaseFontSize()
{
    if (currentFontSize > 6)
    {
        currentFontSize--;
        for (int i = 0; i < tabWidget->count(); ++i)
        {
            Editor *editor = qobject_cast<Editor *>(tabWidget->widget(i));
            if (editor)
            {
                editor->setFontSize(currentFontSize);
            }
        }
    }
}

void MainWindow::resetFontSize()
{
    currentFontSize = 12;
    for (int i = 0; i < tabWidget->count(); ++i)
    {
        Editor *editor = qobject_cast<Editor *>(tabWidget->widget(i));
        if (editor)
        {
            editor->setFontSize(currentFontSize);
        }
    }
}

void MainWindow::changeTheme()
{
    // TODO: Implement theme switching
}

void MainWindow::openFindDialog()
{
    // TODO: Show find dialog
    statusBar()->showMessage("Find dialog opened");
}

void MainWindow::openReplaceDialog()
{
    // TODO: Show replace dialog
    statusBar()->showMessage("Replace dialog opened");
}

void MainWindow::findNext()
{
    // TODO: Find next occurrence
}

void MainWindow::findPrevious()
{
    // TODO: Find previous occurrence
}

void MainWindow::showAbout()
{
    QMessageBox::about(this, tr("About Professional Text Editor"),
                       tr("Professional Text Editor v1.0\n\n"
                          "A feature-rich text editor built with Qt and C++\n\n"
                          "© 2026 All Rights Reserved"));
}

void MainWindow::showAboutQt()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::onTabChanged(int index)
{
    if (index >= 0)
    {
        Editor *editor = qobject_cast<Editor *>(tabWidget->widget(index));
        if (editor)
        {
            statusBar()->showMessage(
                QString("Line %1, Column %2")
                    .arg(editor->currentLineNumber() + 1)
                    .arg(editor->currentColumnNumber() + 1));
            searchReplace->setCurrentEditor(editor);
        }
    }
}

void MainWindow::onTabCloseRequested(int index)
{
    Editor *editor = qobject_cast<Editor *>(tabWidget->widget(index));
    if (editor)
    {
        if (editor->isModified())
        {
            QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                                      tr("Unsaved Changes"),
                                                                      tr("File '%1' has unsaved changes. Save before closing?")
                                                                          .arg(editor->fileName()),
                                                                      QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

            if (reply == QMessageBox::Cancel)
            {
                return;
            }
            else if (reply == QMessageBox::Save)
            {
                saveFile();
            }
        }
        tabWidget->removeTab(index);
        delete editor;
    }
}

void MainWindow::onDocumentModified()
{
    Editor *editor = currentEditor();
    if (editor && tabWidget->currentIndex() >= 0)
    {
        QString tabText = tabWidget->tabText(tabWidget->currentIndex());
        if (!tabText.endsWith("*"))
        {
            tabWidget->setTabText(tabWidget->currentIndex(), tabText + "*");
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSaveAll())
    {
        writeSettings();
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls())
    {
        for (const QUrl &url : mimeData->urls())
        {
            QString filePath = url.toLocalFile();
            if (QFile::exists(filePath))
            {
                // Open file
                Editor *editor = new Editor(this);
                if (documentManager->openFile(filePath, editor))
                {
                    int index = tabWidget->addTab(editor, QFileInfo(filePath).fileName());
                    tabWidget->setCurrentIndex(index);
                }
            }
        }
        event->acceptProposedAction();
    }
}

void MainWindow::readSettings()
{
    QSettings settings("TextEditor", "TextEditor");
    restoreGeometry(settings.value("geometry", saveGeometry()).toByteArray());
    restoreState(settings.value("windowState", saveState()).toByteArray());

    currentFontSize = settings.value("fontSize", 12).toInt();
    currentTheme = settings.value("theme", "Light").toString();
}

void MainWindow::writeSettings()
{
    QSettings settings("TextEditor", "TextEditor");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.setValue("fontSize", currentFontSize);
    settings.setValue("theme", currentTheme);
}

void MainWindow::loadRecentFiles()
{
    recentFilesMenu->clear();
    recentFiles = documentManager->getRecentFiles();

    for (const QString &file : recentFiles)
    {
        QAction *action = recentFilesMenu->addAction(file);
        connect(action, &QAction::triggered, this, &MainWindow::openRecent);
    }
}

bool MainWindow::maybeSave()
{
    Editor *editor = currentEditor();
    if (!editor || !editor->isModified())
    {
        return true;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                              tr("Unsaved Changes"),
                                                              tr("File '%1' has unsaved changes. Save?")
                                                                  .arg(editor->fileName()),
                                                              QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if (reply == QMessageBox::Save)
    {
        saveFile();
        return !editor->isModified();
    }
    return reply == QMessageBox::Discard;
}

bool MainWindow::maybeSaveAll()
{
    for (int i = 0; i < tabWidget->count(); ++i)
    {
        tabWidget->setCurrentIndex(i);
        if (!maybeSave())
        {
            return false;
        }
    }
    return true;
}

Editor *MainWindow::currentEditor() const
{
    return qobject_cast<Editor *>(tabWidget->currentWidget());
}
