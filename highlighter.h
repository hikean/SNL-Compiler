
#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

//! [0]
class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    Highlighter(QTextDocument *parent = 0,bool isCPP=false);

protected:
    void highlightBlock(const QString &text);

private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QRegExp commentStartExpression;
    QRegExp commentEndExpression;
    ///cpp
    QTextCharFormat cppKeywordFormat;
    QTextCharFormat cppClassFormat;
    QTextCharFormat cppSingleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat cppQuotationFormat;
    QTextCharFormat cppFunctionFormat;
    QTextCharFormat cppSTLFormat;
    QTextCharFormat cppNumFormat;

    ///snl
    QTextCharFormat snlKeywordFarmat;
    QTextCharFormat snlFunctionFormat;
    QTextCharFormat snlNumFormat;
};
//! [0]

#endif // HIGHLIGHTER_H
