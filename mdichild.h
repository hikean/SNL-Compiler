

#ifndef MDICHILD_H
#define MDICHILD_H

#include <QTextEdit>
#include <QStatusBar>
#include "highlighter.h"
#include <QObject>
#include <QPlainTextEdit>

QT_BEGIN_NAMESPACE
class QCompleter;
class QPaintEvent;
class QSize;
class QWidget;
QT_END_NAMESPACE

class LineNumberArea;

class MdiChild : public QPlainTextEdit
{
    Q_OBJECT

public:
    MdiChild();

    void newFile();
    bool loadFile(const QString &fileName);
    bool save();
    bool saveAs();
    bool saveFile(const QString &fileName);
    QString userFriendlyCurrentFile();
    QString currentFile() { return curFile; }
    void setStatusBar(QStatusBar * stb){   stbr=stb;}
    /******completer*******/
    void setCompleter(QCompleter *textEditCompltr);
    QCompleter *completer() const;
    /**Line number***/
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

protected:
    void closeEvent(QCloseEvent *event);
     /******completer*******/
    void keyPressEvent(QKeyEvent *e);
    void focusInEvent(QFocusEvent *e);
    /***line Number***/
    void resizeEvent(QResizeEvent *e);

private slots:
    void documentWasModified();
    void setUpEditor(bool isCPP=false);
    void updateStatusTipSlot();
    /******completer*******/
    void insertCompletion(const QString &completion);
    /*******line number*********/
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);
private:
    bool maybeSave();/*****private********/
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);
    QStatusBar * stbr;


    QString curFile;
    bool isUntitled;
    Highlighter *highlighter;

    /******completer*******/
     QString textUnderCursor() const;
    QCompleter *textEditCompltr;
    QWidget * lineNumberArea;
};

class LineNumberArea : public QWidget
{
public:
    LineNumberArea(MdiChild *editor) : QWidget(editor) {
        codeEditor = editor;
    }

    QSize sizeHint() const {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) {
        codeEditor->lineNumberAreaPaintEvent(event);
    }
private:
    MdiChild *codeEditor;
};

#endif
