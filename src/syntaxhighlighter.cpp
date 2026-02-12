#include "syntaxhighlighter.h"
#include <QDebug>

SyntaxHighlighter::SyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent), currentLanguage(PlainText), theme("Light"), enabled(true)
{
    setLanguage(PlainText);
}

SyntaxHighlighter::~SyntaxHighlighter() = default;

void SyntaxHighlighter::setLanguage(Language lang)
{
    currentLanguage = lang;
    highlightingRules.clear();

    switch (lang)
    {
    case CPlusPlus:
        initializeCppRules();
        break;
    case Python:
        initializePythonRules();
        break;
    case JavaScript:
        initializeJavaScriptRules();
        break;
    case JSON:
        initializeJsonRules();
        break;
    case XML:
        initializeXmlRules();
        break;
    case HTML:
        initializeHtmlRules();
        break;
    case CSS:
        initializeCssRules();
        break;
    case SQL:
        initializeSqlRules();
        break;
    default:
        break;
    }

    rehighlight();
}

void SyntaxHighlighter::detectLanguageFromExtension(const QString &extension)
{
    if (extension == "cpp" || extension == "cc" || extension == "cxx" || extension == "h" || extension == "hpp")
    {
        setLanguage(CPlusPlus);
    }
    else if (extension == "py")
    {
        setLanguage(Python);
    }
    else if (extension == "js" || extension == "ts")
    {
        setLanguage(JavaScript);
    }
    else if (extension == "json")
    {
        setLanguage(JSON);
    }
    else if (extension == "xml")
    {
        setLanguage(XML);
    }
    else if (extension == "html" || extension == "htm")
    {
        setLanguage(HTML);
    }
    else if (extension == "css")
    {
        setLanguage(CSS);
    }
    else if (extension == "sql")
    {
        setLanguage(SQL);
    }
    else
    {
        setLanguage(PlainText);
    }
}

void SyntaxHighlighter::setTheme(const QString &themeName)
{
    theme = themeName;
    if (themeName == "Dark")
    {
        applyDarkTheme();
    }
    else
    {
        applyLightTheme();
    }
    rehighlight();
}

void SyntaxHighlighter::addCustomRule(const HighlightingRule &rule)
{
    customRules.append(rule);
    rehighlight();
}

void SyntaxHighlighter::clearCustomRules()
{
    customRules.clear();
    rehighlight();
}

void SyntaxHighlighter::setHighlightingEnabled(bool enable)
{
    enabled = enable;
    rehighlight();
}

void SyntaxHighlighter::highlightBlock(const QString &text)
{
    if (!enabled)
        return;

    // Apply highlighting rules
    for (const HighlightingRule &rule : highlightingRules)
    {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext())
        {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    // Apply custom rules
    for (const HighlightingRule &rule : customRules)
    {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext())
        {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}

void SyntaxHighlighter::initializeCppRules()
{
    HighlightingRule rule;

    // Keywords
    QTextCharFormat keywordFormat = getFormat("keyword");
    QStringList keywords = {
        "auto", "bool", "break", "case", "catch", "char", "class", "const",
        "continue", "default", "delete", "do", "double", "else", "enum", "explicit",
        "extern", "false", "float", "for", "friend", "goto", "if", "inline",
        "int", "long", "mutable", "namespace", "new", "nullptr", "operator",
        "private", "protected", "public", "register", "return", "short", "signed",
        "sizeof", "static", "struct", "switch", "template", "this", "throw",
        "true", "try", "typedef", "typeid", "typename", "union", "unsigned",
        "virtual", "void", "volatile", "wchar_t", "while"};

    for (const QString &keyword : keywords)
    {
        rule.pattern = QRegularExpression("\\b" + keyword + "\\b");
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    // Strings
    rule.pattern = QRegularExpression("\".*?\"");
    rule.format = getFormat("string");
    highlightingRules.append(rule);

    // Comments
    rule.pattern = QRegularExpression("//.*");
    rule.format = getFormat("comment");
    highlightingRules.append(rule);

    // Numbers
    rule.pattern = QRegularExpression("\\b[0-9]+\\b");
    rule.format = getFormat("number");
    highlightingRules.append(rule);
}

void SyntaxHighlighter::initializePythonRules()
{
    HighlightingRule rule;

    // Keywords
    QTextCharFormat keywordFormat = getFormat("keyword");
    QStringList keywords = {
        "and", "as", "assert", "break", "class", "continue", "def", "del",
        "elif", "else", "except", "False", "finally", "for", "from", "global",
        "if", "import", "in", "is", "lambda", "None", "not", "or", "pass",
        "raise", "return", "True", "try", "while", "with", "yield"};

    for (const QString &keyword : keywords)
    {
        rule.pattern = QRegularExpression("\\b" + keyword + "\\b");
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    // Strings
    rule.pattern = QRegularExpression("\".*?\"");
    rule.format = getFormat("string");
    highlightingRules.append(rule);

    rule.pattern = QRegularExpression("'.*?'");
    rule.format = getFormat("string");
    highlightingRules.append(rule);

    // Comments
    rule.pattern = QRegularExpression("#.*");
    rule.format = getFormat("comment");
    highlightingRules.append(rule);
}

void SyntaxHighlighter::initializeJavaScriptRules()
{
    HighlightingRule rule;

    // Keywords
    QTextCharFormat keywordFormat = getFormat("keyword");
    QStringList keywords = {
        "break", "case", "catch", "class", "const", "continue", "debugger",
        "default", "delete", "do", "else", "export", "extends", "finally",
        "for", "function", "if", "import", "in", "instanceof", "new", "return",
        "super", "switch", "this", "throw", "try", "typeof", "var", "void",
        "while", "with", "yield", "let", "static", "enum", "await", "async"};

    for (const QString &keyword : keywords)
    {
        rule.pattern = QRegularExpression("\\b" + keyword + "\\b");
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    // Strings
    rule.pattern = QRegularExpression("\".*?\"");
    rule.format = getFormat("string");
    highlightingRules.append(rule);

    rule.pattern = QRegularExpression("'.*?'");
    rule.format = getFormat("string");
    highlightingRules.append(rule);

    // Comments
    rule.pattern = QRegularExpression("//.*");
    rule.format = getFormat("comment");
    highlightingRules.append(rule);
}

void SyntaxHighlighter::initializeJsonRules()
{
    HighlightingRule rule;

    // Strings
    rule.pattern = QRegularExpression("\".*?\"");
    rule.format = getFormat("string");
    highlightingRules.append(rule);

    // Numbers
    rule.pattern = QRegularExpression("[0-9]+");
    rule.format = getFormat("number");
    highlightingRules.append(rule);

    // Keywords
    QTextCharFormat keywordFormat = getFormat("keyword");
    QStringList keywords = {"true", "false", "null"};

    for (const QString &keyword : keywords)
    {
        rule.pattern = QRegularExpression("\\b" + keyword + "\\b");
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }
}

void SyntaxHighlighter::initializeXmlRules()
{
    HighlightingRule rule;

    // Tags
    rule.pattern = QRegularExpression("<.*?>");
    rule.format = getFormat("keyword");
    highlightingRules.append(rule);

    // Attributes
    rule.pattern = QRegularExpression("\\b[A-Za-z_:][A-Za-z0-9_:.-]*=");
    rule.format = getFormat("keyword");
    highlightingRules.append(rule);

    // Strings
    rule.pattern = QRegularExpression("\".*?\"");
    rule.format = getFormat("string");
    highlightingRules.append(rule);

    // Comments
    rule.pattern = QRegularExpression("<!--.*?-->");
    rule.format = getFormat("comment");
    highlightingRules.append(rule);
}

void SyntaxHighlighter::initializeHtmlRules()
{
    HighlightingRule rule;

    // Tags
    rule.pattern = QRegularExpression("</?[A-Za-z].*?>");
    rule.format = getFormat("keyword");
    highlightingRules.append(rule);

    // Strings
    rule.pattern = QRegularExpression("\".*?\"");
    rule.format = getFormat("string");
    highlightingRules.append(rule);

    // Comments
    rule.pattern = QRegularExpression("<!--.*?-->");
    rule.format = getFormat("comment");
    highlightingRules.append(rule);
}

void SyntaxHighlighter::initializeCssRules()
{
    HighlightingRule rule;

    // Selectors
    rule.pattern = QRegularExpression("[.#]?[A-Za-z_][A-Za-z0-9_-]*");
    rule.format = getFormat("keyword");
    highlightingRules.append(rule);

    // Properties
    rule.pattern = QRegularExpression("[A-Za-z-]+(?=:)");
    rule.format = getFormat("keyword");
    highlightingRules.append(rule);

    // Strings
    rule.pattern = QRegularExpression("\".*?\"");
    rule.format = getFormat("string");
    highlightingRules.append(rule);

    // Comments
    rule.pattern = QRegularExpression("/\\*.*?\\*/");
    rule.format = getFormat("comment");
    highlightingRules.append(rule);
}

void SyntaxHighlighter::initializeSqlRules()
{
    HighlightingRule rule;

    // Keywords
    QTextCharFormat keywordFormat = getFormat("keyword");
    QStringList keywords = {
        "SELECT", "FROM", "WHERE", "AND", "OR", "NOT", "INSERT", "UPDATE",
        "DELETE", "CREATE", "ALTER", "DROP", "TABLE", "DATABASE", "PRIMARY",
        "KEY", "FOREIGN", "JOIN", "LEFT", "RIGHT", "INNER", "OUTER", "ON",
        "GROUP", "BY", "ORDER", "ASC", "DESC", "LIMIT", "OFFSET", "DISTINCT"};

    for (const QString &keyword : keywords)
    {
        rule.pattern = QRegularExpression("\\b" + keyword + "\\b", QRegularExpression::CaseInsensitiveOption);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    // Strings
    rule.pattern = QRegularExpression("'.*?'");
    rule.format = getFormat("string");
    highlightingRules.append(rule);

    // Comments
    rule.pattern = QRegularExpression("--.*");
    rule.format = getFormat("comment");
    highlightingRules.append(rule);
}

void SyntaxHighlighter::applyTheme(const QString &themeName)
{
    if (themeName == "Dark")
    {
        applyDarkTheme();
    }
    else
    {
        applyLightTheme();
    }
}

void SyntaxHighlighter::applyLightTheme()
{
    QTextCharFormat format;

    // Keyword format
    format.setForeground(Qt::blue);
    format.setFontWeight(QFont::Bold);
    formats["keyword"] = format;

    // String format
    format.setForeground(Qt::darkGreen);
    format.setFontWeight(QFont::Normal);
    formats["string"] = format;

    // Comment format
    format.setForeground(Qt::gray);
    format.setFontWeight(QFont::Normal);
    format.setFontItalic(true);
    formats["comment"] = format;

    // Number format
    format.setForeground(Qt::darkMagenta);
    format.setFontWeight(QFont::Normal);
    formats["number"] = format;
}

void SyntaxHighlighter::applyDarkTheme()
{
    QTextCharFormat format;

    // Keyword format
    format.setForeground(Qt::cyan);
    format.setFontWeight(QFont::Bold);
    formats["keyword"] = format;

    // String format
    format.setForeground(Qt::green);
    format.setFontWeight(QFont::Normal);
    formats["string"] = format;

    // Comment format
    format.setForeground(Qt::gray);
    format.setFontWeight(QFont::Normal);
    format.setFontItalic(true);
    formats["comment"] = format;

    // Number format
    format.setForeground(Qt::magenta);
    format.setFontWeight(QFont::Normal);
    formats["number"] = format;
}

QTextCharFormat SyntaxHighlighter::getFormat(const QString &formatType) const
{
    return formats.value(formatType, QTextCharFormat());
}

void SyntaxHighlighter::highlightMultilineComment(const QString &text, const QRegularExpression &startExpression,
                                                  const QRegularExpression &endExpression, int state)
{
    int startIndex = 0;
    if (previousBlockState() == state)
    {
        startIndex = 0;
    }
    else
    {
        startIndex = text.indexOf(startExpression);
    }

    while (startIndex >= 0)
    {
        QRegularExpressionMatch endMatch = endExpression.match(text, startIndex);
        int endIndex = endMatch.capturedStart();
        int commentLength;

        if (endIndex == -1)
        {
            setCurrentBlockState(state);
            commentLength = text.length() - startIndex;
        }
        else
        {
            commentLength = endIndex - startIndex + endMatch.capturedLength();
        }

        setFormat(startIndex, commentLength, getFormat("comment"));
        startIndex = text.indexOf(startExpression, startIndex + commentLength);
    }
}
