#pragma once
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <vector>

class GscHighlighter : public QSyntaxHighlighter {
    Q_OBJECT

public:
    explicit GscHighlighter(QTextDocument* parent = nullptr);

protected:
    void highlightBlock(const QString& text) override;

private:
    struct Rule {
        QRegularExpression pattern;
        QTextCharFormat    format;
    };

    std::vector<Rule> rules;

    QTextCharFormat devBlockFormat;

    void setupRules();
};
