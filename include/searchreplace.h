#ifndef SEARCHREPLACE_H
#define SEARCHREPLACE_H

#include <QString>
#include <QTextDocument>
#include <QVector>
#include <QDialog>
#include <memory>

class Editor;
class QLineEdit;
class QCheckBox;
class QPushButton;
class QComboBox;

/**
 * @brief Search and replace result item
 */
struct SearchResult
{
    int lineNumber;
    int columnNumber;
    QString matchedText;
    int startPosition;
    int endPosition;
};

/**
 * @brief Manages search and replace operations
 *
 * Provides efficient text search with various options including
 * regular expressions, case sensitivity, and whole word matching.
 */
class SearchReplace : public QObject
{
    Q_OBJECT

public:
    explicit SearchReplace(QObject *parent = nullptr);
    ~SearchReplace();

    // Search options
    enum SearchOption
    {
        CaseSensitive = 0x1,
        WholeWord = 0x2,
        UseRegex = 0x4,
        SearchSelection = 0x8
    };
    Q_DECLARE_FLAGS(SearchOptions, SearchOption)

    // Search operations
    bool find(Editor *editor, const QString &searchText, SearchOptions options = {});
    bool findNext(Editor *editor);
    bool findPrevious(Editor *editor);
    bool replace(Editor *editor, const QString &searchText, const QString &replaceText, SearchOptions options = {});
    int replaceAll(Editor *editor, const QString &searchText, const QString &replaceText, SearchOptions options = {});

    // Results
    QVector<SearchResult> findAll(Editor *editor, const QString &searchText, SearchOptions options = {});
    SearchResult getCurrentResult() const { return currentResult; }
    int getTotalMatches() const { return totalMatches; }

    // State
    void setCurrentEditor(Editor *editor) { currentEditor = editor; }
    void setSearchOptions(SearchOptions options) { searchOptions = options; }
    SearchOptions getSearchOptions() const { return searchOptions; }
    void clearSearch();

    // Highlighting
    void highlightMatches(Editor *editor, const QString &searchText);
    void clearHighlights(Editor *editor);

signals:
    void matchFound(const SearchResult &result);
    void noMatchFound();
    void matchesUpdated(int totalMatches);
    void replacementMade(int replacements);

private:
    SearchResult performSearch(Editor *editor, const QString &searchText, int startPosition);
    bool validateRegex(const QString &pattern) const;
    QString escapeRegexSpecialChars(const QString &text) const;

    Editor *currentEditor;
    SearchOptions searchOptions;
    SearchResult currentResult;
    int totalMatches;
    int currentMatchIndex;
    QString lastSearchText;

    friend class SearchReplaceDialog;
};

/**
 * @brief Find and Replace dialog
 */
class SearchReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SearchReplaceDialog(QWidget *parent = nullptr);
    ~SearchReplaceDialog();

    void setSearchManager(SearchReplace *manager) { searchManager = manager; }
    void setEditor(Editor *editor) { currentEditor = editor; }

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onFindButtonClicked();
    void onReplaceButtonClicked();
    void onReplaceAllButtonClicked();
    void onCaseSensitiveToggled(bool checked);
    void onWholeWordToggled(bool checked);
    void onRegexToggled(bool checked);

private:
    void createUI();
    void createConnections();

    // UI Components
    QLineEdit *findLineEdit;
    QLineEdit *replaceLineEdit;
    QCheckBox *caseSensitiveCheckBox;
    QCheckBox *wholeWordCheckBox;
    QCheckBox *regexCheckBox;
    QPushButton *findButton;
    QPushButton *replaceButton;
    QPushButton *replaceAllButton;
    QPushButton *closeButton;

    // Managers
    SearchReplace *searchManager;
    Editor *currentEditor;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(SearchReplace::SearchOptions)

#endif // SEARCHREPLACE_H
