#include "searchreplace.h"
#include "editor.h"

#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QRegularExpression>
#include <QTextCursor>
#include <QTextDocument>
#include <QDebug>
#include <QCloseEvent>

SearchReplace::SearchReplace(QObject *parent)
    : QObject(parent), currentEditor(nullptr), searchOptions({}), totalMatches(0), currentMatchIndex(0)
{
}

SearchReplace::~SearchReplace() = default;

bool SearchReplace::find(Editor *editor, const QString &searchText, SearchOptions options)
{
    if (!editor || searchText.isEmpty())
    {
        emit noMatchFound();
        return false;
    }

    currentEditor = editor;
    searchOptions = options;
    lastSearchText = searchText;
    totalMatches = 0;

    // Get all matches first
    QVector<SearchResult> results = findAll(editor, searchText, options);
    if (results.isEmpty())
    {
        emit noMatchFound();
        return false;
    }

    totalMatches = results.size();
    if (!results.isEmpty())
    {
        currentResult = results.first();
        emit matchFound(currentResult);
        emit matchesUpdated(totalMatches);
        return true;
    }

    return false;
}

bool SearchReplace::findNext(Editor *editor)
{
    if (!editor || lastSearchText.isEmpty())
    {
        return false;
    }

    currentEditor = editor;
    QVector<SearchResult> results = findAll(editor, lastSearchText, searchOptions);

    if (results.isEmpty())
    {
        emit noMatchFound();
        return false;
    }

    // Find next match after current position
    int currentPos = editor->textCursor().position();
    for (const SearchResult &result : results)
    {
        if (result.startPosition > currentPos)
        {
            currentResult = result;
            emit matchFound(result);
            return true;
        }
    }

    // Wrap around to first match
    if (!results.isEmpty())
    {
        currentResult = results.first();
        emit matchFound(currentResult);
        return true;
    }

    return false;
}

bool SearchReplace::findPrevious(Editor *editor)
{
    if (!editor || lastSearchText.isEmpty())
    {
        return false;
    }

    currentEditor = editor;
    QVector<SearchResult> results = findAll(editor, lastSearchText, searchOptions);

    if (results.isEmpty())
    {
        emit noMatchFound();
        return false;
    }

    // Find previous match before current position
    int currentPos = editor->textCursor().position();
    for (int i = results.size() - 1; i >= 0; --i)
    {
        if (results[i].startPosition < currentPos)
        {
            currentResult = results[i];
            emit matchFound(currentResult);
            return true;
        }
    }

    // Wrap around to last match
    if (!results.isEmpty())
    {
        currentResult = results.last();
        emit matchFound(currentResult);
        return true;
    }

    return false;
}

bool SearchReplace::replace(Editor *editor, const QString &searchText, const QString &replaceText, SearchOptions options)
{
    if (!editor || searchText.isEmpty())
    {
        return false;
    }

    QTextCursor cursor = editor->textCursor();
    QString documentText = editor->toPlainText();

    QString searchPattern = searchText;
    if (!(options & UseRegex))
    {
        searchPattern = escapeRegexSpecialChars(searchText);
    }

    Qt::CaseSensitivity caseSensitivity = (options & CaseSensitive) ? Qt::CaseSensitive : Qt::CaseInsensitive;
    QRegularExpression regex(searchPattern);
    if (!caseSensitivity)
    {
        regex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    }

    if (regex.isValid())
    {
        QRegularExpressionMatch match = regex.match(documentText, cursor.position());
        if (match.hasMatch())
        {
            cursor.setPosition(match.capturedStart());
            cursor.setPosition(match.capturedEnd(), QTextCursor::KeepAnchor);
            cursor.insertText(replaceText);
            editor->setTextCursor(cursor);
            emit replacementMade(1);
            return true;
        }
    }

    return false;
}

int SearchReplace::replaceAll(Editor *editor, const QString &searchText, const QString &replaceText, SearchOptions options)
{
    if (!editor || searchText.isEmpty())
    {
        return 0;
    }

    QVector<SearchResult> results = findAll(editor, searchText, options);
    if (results.isEmpty())
    {
        return 0;
    }

    QString documentText = editor->toPlainText();
    QString result = documentText;

    for (int i = results.size() - 1; i >= 0; --i)
    {
        result.replace(results[i].startPosition, results[i].endPosition - results[i].startPosition, replaceText);
    }

    editor->setPlainText(result);
    emit replacementMade(results.size());
    return results.size();
}

QVector<SearchResult> SearchReplace::findAll(Editor *editor, const QString &searchText, SearchOptions options)
{
    QVector<SearchResult> results;

    if (!editor || searchText.isEmpty())
    {
        return results;
    }

    QString documentText = editor->toPlainText();
    QString searchPattern = searchText;

    if (!(options & UseRegex))
    {
        searchPattern = escapeRegexSpecialChars(searchText);
    }

    if (options & WholeWord)
    {
        searchPattern = QString("\\b%1\\b").arg(searchPattern);
    }

    Qt::CaseSensitivity caseSensitivity = (options & CaseSensitive) ? Qt::CaseSensitive : Qt::CaseInsensitive;
    QRegularExpression regex(searchPattern);
    if (!caseSensitivity)
    {
        regex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    }

    if (!regex.isValid())
    {
        return results;
    }

    QRegularExpressionMatchIterator it = regex.globalMatch(documentText);
    while (it.hasNext())
    {
        QRegularExpressionMatch match = it.next();

        // Calculate line and column numbers
        int lineNumber = 0;
        int columnNumber = 0;

        for (int i = 0; i < match.capturedStart(); ++i)
        {
            if (documentText[i] == '\n')
            {
                lineNumber++;
                columnNumber = 0;
            }
            else
            {
                columnNumber++;
            }
        }

        SearchResult result;
        result.lineNumber = lineNumber;
        result.columnNumber = columnNumber;
        result.matchedText = match.captured();
        result.startPosition = match.capturedStart();
        result.endPosition = match.capturedEnd();

        results.append(result);
    }

    return results;
}

void SearchReplace::clearSearch()
{
    lastSearchText.clear();
    currentMatchIndex = 0;
    totalMatches = 0;
    clearHighlights(currentEditor);
}

void SearchReplace::highlightMatches(Editor *editor, const QString &searchText)
{
    if (!editor)
        return;

    QVector<SearchResult> results = findAll(editor, searchText, searchOptions);
    // TODO: Apply highlighting to matches
}

void SearchReplace::clearHighlights(Editor *editor)
{
    if (!editor)
        return;
    // TODO: Clear highlights
}

SearchResult SearchReplace::performSearch(Editor *editor, const QString &searchText, int startPosition)
{
    SearchResult result;

    if (!editor || searchText.isEmpty())
    {
        return result;
    }

    QString documentText = editor->toPlainText();
    QRegularExpression regex(escapeRegexSpecialChars(searchText));

    QRegularExpressionMatch match = regex.match(documentText, startPosition);
    if (match.hasMatch())
    {
        result.matchedText = match.captured();
        result.startPosition = match.capturedStart();
        result.endPosition = match.capturedEnd();
    }

    return result;
}

bool SearchReplace::validateRegex(const QString &pattern) const
{
    QRegularExpression regex(pattern);
    return regex.isValid();
}

QString SearchReplace::escapeRegexSpecialChars(const QString &text) const
{
    QString escaped = text;
    escaped.replace("\\", "\\\\");
    escaped.replace(".", "\\.");
    escaped.replace("^", "\\^");
    escaped.replace("$", "\\$");
    escaped.replace("*", "\\*");
    escaped.replace("+", "\\+");
    escaped.replace("?", "\\?");
    escaped.replace("{", "\\{");
    escaped.replace("}", "\\}");
    escaped.replace("[", "\\[");
    escaped.replace("]", "\\]");
    escaped.replace("(", "\\(");
    escaped.replace(")", "\\)");
    escaped.replace("|", "\\|");
    return escaped;
}

SearchReplaceDialog::SearchReplaceDialog(QWidget *parent)
    : QDialog(parent), searchManager(nullptr), currentEditor(nullptr)
{
    setWindowTitle("Find and Replace");
    setGeometry(100, 100, 500, 200);
    createUI();
    createConnections();
}

SearchReplaceDialog::~SearchReplaceDialog() = default;

void SearchReplaceDialog::createUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Find row
    QHBoxLayout *findLayout = new QHBoxLayout();
    findLayout->addWidget(new QLabel("Find:"));
    findLineEdit = new QLineEdit();
    findLayout->addWidget(findLineEdit);
    mainLayout->addLayout(findLayout);

    // Replace row
    QHBoxLayout *replaceLayout = new QHBoxLayout();
    replaceLayout->addWidget(new QLabel("Replace:"));
    replaceLineEdit = new QLineEdit();
    replaceLayout->addWidget(replaceLineEdit);
    mainLayout->addLayout(replaceLayout);

    // Options row
    QHBoxLayout *optionsLayout = new QHBoxLayout();
    caseSensitiveCheckBox = new QCheckBox("Case Sensitive");
    wholeWordCheckBox = new QCheckBox("Whole Word");
    regexCheckBox = new QCheckBox("Regular Expression");
    optionsLayout->addWidget(caseSensitiveCheckBox);
    optionsLayout->addWidget(wholeWordCheckBox);
    optionsLayout->addWidget(regexCheckBox);
    mainLayout->addLayout(optionsLayout);

    // Buttons row
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    findButton = new QPushButton("Find");
    replaceButton = new QPushButton("Replace");
    replaceAllButton = new QPushButton("Replace All");
    closeButton = new QPushButton("Close");
    buttonsLayout->addWidget(findButton);
    buttonsLayout->addWidget(replaceButton);
    buttonsLayout->addWidget(replaceAllButton);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(closeButton);
    mainLayout->addLayout(buttonsLayout);
}

void SearchReplaceDialog::createConnections()
{
    connect(findButton, &QPushButton::clicked, this, &SearchReplaceDialog::onFindButtonClicked);
    connect(replaceButton, &QPushButton::clicked, this, &SearchReplaceDialog::onReplaceButtonClicked);
    connect(replaceAllButton, &QPushButton::clicked, this, &SearchReplaceDialog::onReplaceAllButtonClicked);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::close);
}

void SearchReplaceDialog::onFindButtonClicked()
{
    if (!searchManager || !currentEditor)
        return;

    SearchReplace::SearchOptions options;
    if (caseSensitiveCheckBox->isChecked())
        options |= SearchReplace::CaseSensitive;
    if (wholeWordCheckBox->isChecked())
        options |= SearchReplace::WholeWord;
    if (regexCheckBox->isChecked())
        options |= SearchReplace::UseRegex;

    searchManager->find(currentEditor, findLineEdit->text(), options);
}

void SearchReplaceDialog::onReplaceButtonClicked()
{
    if (!searchManager || !currentEditor)
        return;

    SearchReplace::SearchOptions options;
    if (caseSensitiveCheckBox->isChecked())
        options |= SearchReplace::CaseSensitive;
    if (wholeWordCheckBox->isChecked())
        options |= SearchReplace::WholeWord;
    if (regexCheckBox->isChecked())
        options |= SearchReplace::UseRegex;

    searchManager->replace(currentEditor, findLineEdit->text(), replaceLineEdit->text(), options);
}

void SearchReplaceDialog::onReplaceAllButtonClicked()
{
    if (!searchManager || !currentEditor)
        return;

    SearchReplace::SearchOptions options;
    if (caseSensitiveCheckBox->isChecked())
        options |= SearchReplace::CaseSensitive;
    if (wholeWordCheckBox->isChecked())
        options |= SearchReplace::WholeWord;
    if (regexCheckBox->isChecked())
        options |= SearchReplace::UseRegex;

    searchManager->replaceAll(currentEditor, findLineEdit->text(), replaceLineEdit->text(), options);
}

void SearchReplaceDialog::onCaseSensitiveToggled(bool checked)
{
    // TODO: Update search behavior
}

void SearchReplaceDialog::onWholeWordToggled(bool checked)
{
    // TODO: Update search behavior
}

void SearchReplaceDialog::onRegexToggled(bool checked)
{
    // TODO: Update search behavior
}

void SearchReplaceDialog::closeEvent(QCloseEvent *event)
{
    event->accept();
}
