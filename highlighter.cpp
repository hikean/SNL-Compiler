
#include "highlighter.h"
#include "lexer.h"

//! [0]
Highlighter::Highlighter(QTextDocument *parent, bool isCPP)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;
    multiLineCommentFormat.setForeground(Qt::gray);
    if(isCPP)
    {
        cppKeywordFormat.setForeground(Qt::darkBlue);
        cppKeywordFormat.setFontWeight(QFont::Bold);
        QStringList keywordPatterns;
        keywordPatterns << "\\bchar\\b" << "\\bclass\\b" << "\\bconst\\b"
                        << "\\bdouble\\b" << "\\benum\\b" << "\\bexplicit\\b"
                        << "\\bfriend\\b" << "\\binline\\b" << "\\bint\\b"
                        << "\\blong\\b" << "\\bnamespace\\b" << "\\boperator\\b"
                        << "\\bprivate\\b" << "\\bprotected\\b" << "\\bpublic\\b"
                        << "\\bshort\\b" << "\\bsignals\\b" << "\\bsigned\\b"
                        << "\\bslots\\b" << "\\bstatic\\b" << "\\bstruct\\b"
                        << "\\btemplate\\b" << "\\btypedef\\b" << "\\btypename\\b"
                        << "\\bunion\\b" << "\\bunsigned\\b" << "\\bvirtual\\b"
                        << "\\bvoid\\b" << "\\bvolatile\\b"
                        <<"\\bmain\\b"<<"\\breturn\\b";

        foreach (const QString &pattern, keywordPatterns) {
            rule.pattern = QRegExp(pattern);
            rule.format = cppKeywordFormat;
            highlightingRules.append(rule);
            //! [0] //! [1]
        }
        cppSTLFormat.setForeground(Qt::green);///cyan
        QStringList stlwords;
        stlwords << "\\bvector\\b" << "\\bset\\b"<<"\\bmap\\b"<<"\\blist\\b";
        foreach (const QString &pattern, stlwords) {
            rule.pattern = QRegExp(pattern);
            rule.format = cppSTLFormat;
            highlightingRules.append(rule);
            //! [0] //! [1]
        }
        //! [1]

        //! [2]
        cppClassFormat.setFontWeight(QFont::Bold);
        cppClassFormat.setForeground(Qt::darkMagenta);
        rule.pattern = QRegExp("\\bQ[A-Za-z]+\\b");
        rule.format = cppClassFormat;
        highlightingRules.append(rule);
        //! [2]

        //! [3]
        cppSingleLineCommentFormat.setForeground(Qt::green);
        rule.pattern = QRegExp("//[^\n]*");
        rule.format = cppSingleLineCommentFormat;
        highlightingRules.append(rule);


        //! [3]

        //! [4]
        cppQuotationFormat.setForeground(Qt::darkGreen);
        rule.pattern = QRegExp("\".*\"");
        rule.format = cppQuotationFormat;
        highlightingRules.append(rule);
        //! [4]

        //! [5]
        cppFunctionFormat.setFontItalic(true);
        cppFunctionFormat.setForeground(Qt::blue);
        rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
        rule.format = cppFunctionFormat;
        highlightingRules.append(rule);

        //! [5]

        //! [6]
        commentStartExpression = QRegExp("/\\*");
        commentEndExpression = QRegExp("\\*/");
    }
    else //SNL
    {


        snlKeywordFarmat.setForeground(Qt::darkBlue);
        snlKeywordFarmat.setFontWeight(QFont::Bold);
        QStringList keywordsPat;
        keywordsPat << "\\bprogram\\b"      << "\\bdo\\b"<< "\\bprocedure\\b"
                       /* << "\\bif\\b"      << "\\bthen\\b"
                    << "\\belse\\b"     << "\\bfi\\b"           << "\\bwhile\\b"
                         << "\\bbegin\\b" << "\\bendwh\\b"  << "\\bend\\b"*/
                    << "\\bread\\b"         << "\\bwrite\\b"<< "\\bof\\b"

                    /*<< "\\breturn\\b" << "\\btype\\b"
                    << "\\brecord\\b"  << "\\binteger\\b"  << "\\barray\\b"
                    <<"\\bchar\\b" << "\\bvar\\b"*/;
        foreach(const QString &pat ,keywordsPat)
        {
            rule.pattern = QRegExp(pat);
            rule.format = snlKeywordFarmat;
            highlightingRules.append(rule);
        }
        snlFunctionFormat.setFontItalic(true);
        snlFunctionFormat.setFontWeight(QFont::Bold);
        snlFunctionFormat.setForeground(QBrush(QColor(0,155,0)));
        rule.pattern = QRegExp("\\b[A-Za-z]([A-Za-z0-9]+)?(?=\\()");
        rule.format = snlFunctionFormat;
        highlightingRules.append(rule);

        snlNumFormat.setForeground(QBrush(QColor(128,0,128)));
        rule.pattern=QRegExp("\\b\\d+\\b");
        rule.format=snlNumFormat;
        highlightingRules.append(rule);



        QTextCharFormat tmpFormat;
        tmpFormat.setForeground(QBrush(QColor(136,0,21)));
        tmpFormat.setFontWeight(QFont::DemiBold);
        rule.format=tmpFormat;
        QStringList tmpList;
        tmpList<<"\\bbegin\\b"<<"\\bend\\b"<<"\\bif\\b"<<"\\bthen\\b"
                 <<"\\belse\\b"<<"\\bfi\\b"<<"\\bwhile\\b"<<"\\bendwh\\b";
        foreach (const QString &pat, tmpList) {
            rule.pattern=QRegExp(pat);
            highlightingRules.append(rule);
        }
        highlightingRules.append(rule);
        rule.format.setForeground(QBrush(QColor(205,191,0)));
        rule.format.setFont(QFont("Times", 10, QFont::Bold));
        tmpList.clear();
        tmpList<< "\\breturn\\b" << "\\btype\\b"
              << "\\brecord\\b"  << "\\binteger\\b"  << "\\barray\\b"
              <<"\\bchar\\b" << "\\bvar\\b";
        foreach (const QString &pat, tmpList) {
            rule.pattern=QRegExp(pat);
            highlightingRules.append(rule);
        }
        tmpList.clear();
        tmpList<<"\\+"<<"\\-"<<"\\*"
              <<"\\:\\="<<"\\/"<<"\\="
                <<"\\."<<"\\;"<<"\\,"<<"\\("
                  <<"\\["<<"\\]"<<"\\)";
        rule.format.setForeground(QBrush(QColor(255,0,1)));//51,164,138
        foreach (const QString &pat, tmpList) {
            rule.pattern=QRegExp(pat);
            highlightingRules.append(rule);
        }
#ifdef SingleLineComment
        snlNumFormat.setForeground(Qt::gray);
        rule.pattern=QRegExp("[^']#[^\n]*");
        rule.format=snlNumFormat;
        highlightingRules.append(rule);
        //rule.pattern=QRegExp("#[^\n]*");
#endif
        commentStartExpression = QRegExp("\\{");
        commentEndExpression = QRegExp("\\}");
    }
}
//! [6]

//! [7]
void Highlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
    //! [7] //! [8]
    setCurrentBlockState(0);
    //! [8]

    //! [9]
    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = commentStartExpression.indexIn(text);

    //! [9] //! [10]
    while (startIndex >= 0) {
        //! [10] //! [11]
        int endIndex = commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                    + commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
    }
}
//! [11]
