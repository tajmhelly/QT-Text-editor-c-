#ifndef SYNTAXHIGHLIGHTER_H
#define SYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextDocument>
#include <QHash>
#include <QTextCharFormat>
#include <QVector>
#include <QRegularExpression>

class Editor;

/**
 * @brief Syntax highlighting rule definition
 */
struct HighlightingRule
{
    QRegularExpression pattern;
    QTextCharFormat format;
};

/**
 * @brief Provides syntax highlighting for various programming languages
 *
 * Supports C++, Python, JavaScript, JSON, XML, and more.
 * Extensible architecture for adding new languages.
 */
class SyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    enum Language
    {
        PlainText,
        CPlusPlus,
        Python,
        JavaScript,
        JSON,
        XML,
        HTML,
        CSS,
        SQL
    };

    explicit SyntaxHighlighter(QTextDocument *parent = nullptr);
    ~SyntaxHighlighter();

    // Language selection
    void setLanguage(Language lang);
    void detectLanguageFromExtension(const QString &extension);
    Language getLanguage() const { return currentLanguage; }

    // Theme management
    void setTheme(const QString &themeName);
    QString currentThemeName() const { return theme; }

    // Custom rules
    void addCustomRule(const HighlightingRule &rule);
    void clearCustomRules();

    // Highlighting control
    void setHighlightingEnabled(bool enabled);
    bool isHighlightingEnabled() const { return enabled; }

protected:
    void highlightBlock(const QString &text) override;

private:
    void initializeCppRules();
    void initializePythonRules();
    void initializeJavaScriptRules();
    void initializeJsonRules();
    void initializeXmlRules();
    void initializeHtmlRules();
    void initializeCssRules();
    void initializeSqlRules();

    void applyTheme(const QString &themeName);
    void applyLightTheme();
    void applyDarkTheme();

    QTextCharFormat getFormat(const QString &formatType) const;
    void highlightMultilineComment(const QString &text, const QRegularExpression &startExpression,
                                   const QRegularExpression &endExpression, int state);

    // Highlighting rules
    QVector<HighlightingRule> highlightingRules;
    QVector<HighlightingRule> customRules;

    // Formats
    QHash<QString, QTextCharFormat> formats;

    // State
    Language currentLanguage;
    QString theme;
    bool enabled;

    // Multi-line comment states
    enum State
    {
        Default = 0,
        InCppComment = 1,
        InPythonString = 2,
        InXmlComment = 3
    };
};

#endif // SYNTAXHIGHLIGHTER_H
