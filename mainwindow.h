/****************************************************************************
**
** Copyright (C) 2015 Kean
**
****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QLayout>
#include <QFile>
#include <QTreeView>
//#include <QTreeWidget>

class MdiChild;
QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QCompleter;
class QMdiArea;
class QMdiSubWindow;
class QSignalMapper;
class QAbstractItemModel;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();


protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void newFile();
    void open();
    void save();
    void saveAs();
#ifndef QT_NO_CLIPBOARD
    void cut();
    void copy();
    void paste();
#endif
    void about();
    void updateMenus();
    void updateWindowMenu();
    MdiChild *createMdiChild();
    void switchLayoutDirection();
    void setActiveSubWindow(QWidget *window);
    void lexerSlot();/**************/
    void parserLL1Slot();
    void parserRecurSlot();
    void semanticsSlot();
    void tokenListSlot();
    void parserTreeSlot();
    void semanticsTableSlot();
    void buildAllSlot();
    void infoLogWindowShowHideSlot();
    void disableTabViewSlot();
    void setMdiChildViewModeSlot();
    void showProductsSlot();
    void showPredictsSlot();
    void showProductTreeSlot();
    void runCppKeanSlot();
    void runCppBlSlot();
    void treeWindowShowHideSlot();

private:

    QString getCurrentFileName();
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void readSettings();
    void writeSettings();

    bool setInfoLogText(const QString &fileName);


    MdiChild *activeMdiChild();
    QMdiSubWindow *findMdiChild(const QString &fileName);


    QDockWidget * statusWindow;
    QTextEdit * logInfoEditWindow;
    QAction *infoLogWindowShowHideAct;///显示隐藏信息栏
    bool doLexer;
    bool doParserLl1;
    bool doParserRecur;
    bool doSemantics;
    bool isTabView;

    //QTreeWidget * trView;
    QDockWidget *treeViewWindow;
    QTreeView * trView;
    QAction *treeWindowShowHideAct;
    void treeViewRenew(const char treeName[], const char filename[]);

    QMdiArea *mdiArea;
    QSignalMapper *windowMapper;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *windowMenu;
    QMenu *helpMenu;
    QMenu *buildMenu;/*****/

    QAction *runCppActKean;
    QAction *runCppActBl;
    QAction *buildAllAct;
    QAction *lexerAct;
    QAction *parserLL1Act;
    QAction *parserRecurAct;
    QAction *semanticsAct;/****/

    QMenu *othersMenu;
    QAction *showProductsAct;
    QAction *showPredictsAct;


    QMenu *infoMenu;/******/
    QAction *parserTreeAct;
    QAction *tokenListAct;
    QAction *semanticsTableAct;
    QAction *showProductTreeAct;

    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QToolBar *buildToolBar;
    QToolBar *infoToolBar;
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *exitAct;
#ifndef QT_NO_CLIPBOARD
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
#endif
    QAction *closeAct;
    QAction *closeAllAct;
    QAction *tileAct;
    QAction *cascadeAct;
    QAction *setMdiChildViewModeAct;
    QAction *nextAct;
    QAction *previousAct;
    QAction *separatorAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
    /******Completer******/
    QCompleter *completer;
    QAbstractItemModel *modelFromFile(const QString& fileName);


};


#endif
