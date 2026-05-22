#include <ui_includes.hpp>
#include "GscHighlighter.h"
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>

GscHighlighter::GscHighlighter(QTextDocument* parent) : QSyntaxHighlighter(parent) {
    setupRules();
}
void GscHighlighter::highlightBlock(const QString& text) {
    for (const auto& rule : rules) {
        auto it = rule.pattern.globalMatch(text);
        while (it.hasNext()) {
            auto m = it.next();
            setFormat(m.capturedStart(), m.capturedLength(), rule.format);
        }
    }

    // simple dev-block markers /# and #/
    if (text.contains("/#") || text.contains("#/"))
        setFormat(0, text.length(), devBlockFormat);
}

namespace GscColors
{
    // Keywords (function, if, else, class…)
    constexpr auto Keyword = "#0033cc";

    // Preprocessor (#include, #using…)
    constexpr auto Preprocessor = "#7a00cc";

    // Constants
    constexpr auto Constants = "#2222aa";

    // Strings ("hello", &"str", s"str")
    constexpr auto String = "#aa2222";

    // Comments (// comment)
    constexpr auto Comment = "#007700";

    // Dev blocks (/# … #/)
    constexpr auto DevBlock = Comment;
}

void GscHighlighter::setupRules()
{
    rules.clear();

    //
    // --- KEYWORDS ---
    //
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(QColor(GscColors::Keyword));
    keywordFormat.setFontWeight(QFont::Bold);

    auto addKeyword = [&](const char* kw) {
        rules.push_back({
            QRegularExpression(QStringLiteral("\\b%1\\b").arg(kw)),
            keywordFormat
            });
        };

    const char* keywords[] = {
        "function", "private", "autoexec", "event_handler", "detour",
        "thread", "childthread", "threadendon", "builtin",
        "for", "while", "do", "foreach", "in",
        "if", "else", "switch", "case", "default",
        "class", "new", "is", "not",
        "var",

        "break", "continue", "goto", "return", "wait", "jumpdev"
    };
    for (auto* kw : keywords)
        addKeyword(kw);

    //
    // --- PREPROCESSOR ---
    //
    QTextCharFormat ppFormat;
    ppFormat.setForeground(QColor(GscColors::Preprocessor));
    ppFormat.setFontWeight(QFont::Bold);

    const char* pp[] = {
        "#include", "#using", "#using_animtree", "#precache",
        "#define", "#undef", "#if", "#elif", "#else", "#endif",
        "#namespace", "#file", "#constexpr"
    };
    for (auto* kw : pp) {
        rules.push_back({
            QRegularExpression(QStringLiteral("%1\\b").arg(kw)),
            ppFormat
            });
    }

    //
    // --- BOOL / UNDEFINED ---
    //
    QTextCharFormat boolFormat;
    boolFormat.setForeground(QColor(GscColors::Constants));

    const char* bools[] = { "true", "false", "undefined" };
    for (auto* b : bools) {
        rules.push_back({
            QRegularExpression(QStringLiteral("\\b%1\\b").arg(b)),
            boolFormat
            });
    }

    //
    // --- NUMBERS (corrected: only standalone tokens) ---
    //
    QTextCharFormat numberFormat;
    numberFormat.setForeground(QColor(GscColors::Constants));

    // Integer: hex, binary, octal, decimal
    rules.push_back({
        QRegularExpression(
            R"(\b-?(0[xX][0-9a-fA-F]+|0[bB][01]+|0[0-7]+|[1-9][0-9]*|0)\b)"
        ),
        numberFormat
        });

    // Float
    rules.push_back({
        QRegularExpression(
            R"(\b-?((\d*\.\d+|\d+\.\d*)([eE][+-]?\d+)?|\d+[eE][+-]?\d+)\b)"
        ),
        numberFormat
        });

    //
    // --- STRINGS / ISTRING / HASHSTRING ---
    //
    QTextCharFormat stringFormat;
    stringFormat.setForeground(QColor(GscColors::String));

    rules.push_back({
        QRegularExpression(R"STR("([^"\\]|\\.)*")STR"),
        stringFormat
        });

    rules.push_back({
        QRegularExpression(R"STR(&"([^"\\]|\\.)*")STR"),
        stringFormat
        });

    rules.push_back({
        QRegularExpression(R"STR((?:s|r|#|@|t|%|o)"([^"\\]|\\.)*")STR"),
        stringFormat
        });

    //
    // --- COMMENTS ---
    //
    QTextCharFormat commentFormat;
    commentFormat.setForeground(QColor(GscColors::Comment));

    rules.push_back({
        QRegularExpression(R"(//[^\n]*)"),
        commentFormat
        });

    //
    // --- DEV BLOCKS (/# ... #/) ---
    //
    devBlockFormat = commentFormat;
    devBlockFormat.setFontItalic(true);
}
