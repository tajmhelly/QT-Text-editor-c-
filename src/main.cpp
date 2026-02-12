#include <QApplication>
#include "mainwindow.h"

/**
 * @brief Main entry point for the Text Editor application
 *
 * Initializes the Qt application and creates the main window.
 * Handles command-line arguments for opening files.
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Set application metadata
    app.setApplicationName("Professional Text Editor");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("TextEditor");
    app.setOrganizationDomain("texteditor.local");

    // Create and show main window
    MainWindow window;
    window.show();

    // Open files from command line arguments
    for (int i = 1; i < argc; ++i)
    {
        QString fileName = QString::fromLocal8Bit(argv[i]);
        window.findChild<QTabWidget *>(); // TODO: Open file in editor
    }

    return app.exec();
}
