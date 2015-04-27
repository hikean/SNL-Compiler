
#include <QtWidgets>
#include <QDebug>
#include <QLayout>
#include <QTextStream>
#include <QCompleter>
#include <QStyleFactory>
#include <QProcess>
#include <QFile>

#include "mainwindow.h"
#include "mdichild.h"
#include "lexer.h"
#include "parserLL1.h"
#include "treemodel.h"
#include "sem.h"
#include "ParserRecur.h"
#include "compile.h"
#include "snl2cpp.h"
#define ParserTreeDefaultDepth 4
void InitPredict();

//#define debugOut

#ifdef debugOut
#include<iostream>
using namespace std;
#endif

MainWindow::MainWindow()
{
    /****Completer*****/
    completer = new QCompleter(this);
    completer->setModel(modelFromFile(":/txt/words.txt"));
    completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setWrapAround(false);

    doLexer=false;
    doParserLl1=false;
    doParserRecur=false;
    doSemantics=false;

    trView = new QTreeView();//*******************
    trView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    mdiArea = new QMdiArea;
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    this->setStyleSheet(tr("MdiChild{background-color: rgb(228,244,250);}"));//rgb(239,228,176) yellow (225,196,255)
    mdiArea->setViewMode(QMdiArea::TabbedView);
    mdiArea->setBackground(QBrush(QColor(255,255,192),Qt::Dense5Pattern));//rgb(233,210,255)

    isTabView=true;
    mdiArea->setTabsClosable(true);
    mdiArea->setTabsMovable(true);


    setCentralWidget(mdiArea);
    connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),
            this, SLOT(updateMenus()));

    windowMapper = new QSignalMapper(this);
    connect(windowMapper, SIGNAL(mapped(QWidget*)),
            this, SLOT(setActiveSubWindow(QWidget*)));


    //log & infos
    statusWindow = new QDockWidget(tr("Info & logs"),this);
    statusWindow->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea|Qt::BottomDockWidgetArea);
    statusWindow->setFloating(false);
    addDockWidget(Qt::LeftDockWidgetArea,statusWindow);

    logInfoEditWindow = new QTextEdit();
    logInfoEditWindow->setReadOnly(true);
    logInfoEditWindow->setStyleSheet("background-color: rgb(233,210,255);");
    statusWindow->setWidget(logInfoEditWindow);

    treeViewWindow = new QDockWidget(tr("Show Tree Window"),this);
    treeViewWindow->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea|Qt::BottomDockWidgetArea);
    treeViewWindow->setFloating(false);
    addDockWidget(Qt::RightDockWidgetArea,treeViewWindow);
    treeViewWindow->setWidget(trView);
    trView->setStyleSheet("background-color: rgb(233,210,255);");
    QString stylesht="QScrollBar:vertical"
                "{"
                "width:8px;"
                "background:rgba(0,0,0,0%);"
                "margin:0px,0px,0px,0px;"
                "padding-top:9px;"
                "padding-bottom:9px;"
                "}"
                "QScrollBar::handle:vertical"
                "{"
                "width:8px;"
                "background:rgba(0,0,0,25%);"
                " border-radius:4px;"
                "min-height:20;"
                "}"
                "QScrollBar::handle:vertical:hover"
                "{"
                "width:8px;"
                "background:rgba(0,0,0,50%);"
                " border-radius:4px;"
                "min-height:20;"
                "}"
                "QScrollBar::add-line:vertical"
                "{"
                "height:9px;width:8px;"
                "subcontrol-position:bottom;"
                "}"
                "QScrollBar::sub-line:vertical"
                "{"
                "height:9px;width:8px;"
                "subcontrol-position:top;"
                "}"
                "QScrollBar::add-line:vertical:hover"
                "{"
                "height:9px;width:8px;"
                "subcontrol-position:bottom;"
                "}"
                "QScrollBar::sub-line:vertical:hover"
                "{"
                "height:9px;width:8px;"
                "subcontrol-position:top;"
                "}"
                "QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical"
                "{"
                "background:rgba(0,0,0,10%);"
                "border-radius:4px;"
                "}";
    logInfoEditWindow->verticalScrollBar()->setStyleSheet(stylesht);
    //logInfoEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    trView->horizontalScrollBar()->setStyleSheet(stylesht);
    //mdiArea->horizontalScrollBar()->setStyleSheet(stylesht);
    mdiArea->verticalScrollBar()->setStyleSheet(stylesht);
    trView->verticalScrollBar()->setStyleSheet(stylesht);
    //logInfoEdit->horizontalScrollBar()->setStyleSheet(stylesht);
    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    updateMenus();
    connect(buildToolBar,SIGNAL(actionTriggered(QAction*)),this,SLOT(updateMenus()));
    readSettings();

    setWindowTitle(tr("SNL Compiler"));
    setUnifiedTitleAndToolBarOnMac(true);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    mdiArea->closeAllSubWindows();
    if (mdiArea->currentSubWindow()) {
        event->ignore();
    } else {
        writeSettings();
        event->accept();
    }
}

void MainWindow::newFile()
{
    MdiChild *child = createMdiChild();
    child->newFile();
    child->show();

}

void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty()) {
        QMdiSubWindow *existing = findMdiChild(fileName);
        if (existing) {
            mdiArea->setActiveSubWindow(existing);
            return;
        }

        MdiChild *child = createMdiChild();
        if (child->loadFile(fileName)) {
            statusBar()->showMessage(tr("File loaded"), 2000);
            child->show();
        } else {
            child->close();
        }
    }
}

void MainWindow::save()
{
    qDebug()<<"Main save";
    if (activeMdiChild() && activeMdiChild()->save())
        statusBar()->showMessage(tr("File saved"), 2000);
}

void MainWindow::saveAs()
{
    if (activeMdiChild() && activeMdiChild()->saveAs())
        statusBar()->showMessage(tr("File saved"), 2000);
}

#ifndef QT_NO_CLIPBOARD
void MainWindow::cut()
{
    if (activeMdiChild())
        activeMdiChild()->cut();
}

void MainWindow::copy()
{
    if (activeMdiChild())
        activeMdiChild()->copy();
}

void MainWindow::paste()
{
    if (activeMdiChild())
        activeMdiChild()->paste();
}
#endif

void MainWindow::about()
{
    QMessageBox::about(this, tr("About SNLC"),
                       tr("<img src=\":/images/jluname.png\"/><br/>"
                          "<p>The <b>SNL Compiler</b>  demonstrates how to make a compiler.<br/> "
                          "This project is finished using Qt.<br/>"
                          "It's JLU's Course-Design Project.<br/>"
                          /***"<img src=\":/images/jlumodel.gif\" style=\"text-align:center\"/><br/>"***/
                          "<b>Copyrights (c) 2015 All Reserved by JLU.</b><br/></p>"
                          /* "<b>Group Leader </b>"
                                     "<img src=\":/images/Kean.png\"/><br/>"*/
                          "<pre><p>Group Members:<br/>"
                          "+--------------------------------------+<br/>"
                          "+    Identify        Name    StuNum    +<br/>"
                          "+--------------------------------------+<br/>"
                          "+    Group Leader    <b>Kean</b>    53120201  +<br/>"
                          "+    Group Manager   <b>LBL</b>     53120205  +<br/>"
                          "+    Group Member    <b>LiuQi</b>   53120202  +<br/>"
                          "+--------------------------------------+<br/></p></pre>"
                          "<img src=\":/ico/bl.png\"/> <br/>"
                          "<img src=\":/ico/K.png\"/>"
                          ));
}

void MainWindow::updateMenus()
{
    bool hasMdiChild = (activeMdiChild() != 0 || trView!=0);
    saveAct->setEnabled(hasMdiChild);
    saveAsAct->setEnabled(hasMdiChild);
#ifndef QT_NO_CLIPBOARD
    pasteAct->setEnabled(hasMdiChild);
#endif
    closeAct->setEnabled(hasMdiChild);
    closeAllAct->setEnabled(hasMdiChild);
    tileAct->setEnabled(hasMdiChild);
    cascadeAct->setEnabled(hasMdiChild);
    setMdiChildViewModeAct->setEnabled(hasMdiChild);
    nextAct->setEnabled(hasMdiChild);
    previousAct->setEnabled(hasMdiChild);
    separatorAct->setVisible(hasMdiChild);

#ifndef QT_NO_CLIPBOARD
    bool hasSelection = (activeMdiChild() &&
                         activeMdiChild()->textCursor().hasSelection());
    cutAct->setEnabled(hasSelection);
    copyAct->setEnabled(hasSelection);
#endif
    if(!hasMdiChild)
    {
        doLexer=doParserLl1=doParserRecur=doSemantics=false;
    }

    buildAllAct->setEnabled(hasMdiChild);
    parserLL1Act->setEnabled(doLexer);
    parserRecurAct->setEnabled(doLexer);
    semanticsAct->setEnabled(doParserLl1||doParserRecur);
    lexerAct->setEnabled(hasMdiChild);

    tokenListAct->setEnabled(doLexer);
    parserTreeAct->setEnabled(doParserLl1||doParserRecur);
    semanticsTableAct->setEnabled(doSemantics);
    showProductTreeAct->setEnabled(doParserLl1);
    runCppActKean->setEnabled(doSemantics);
    runCppActBl->setEnabled(doSemantics);
}

void MainWindow::updateWindowMenu()
{
    windowMenu->clear();
    windowMenu->addAction(closeAct);
    windowMenu->addAction(closeAllAct);
    windowMenu->addSeparator();
    windowMenu->addAction(tileAct);
    windowMenu->addAction(cascadeAct);
    windowMenu->addAction(setMdiChildViewModeAct);
    windowMenu->addSeparator();
    windowMenu->addAction(nextAct);
    windowMenu->addAction(previousAct);
    windowMenu->addAction(separatorAct);

    QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
    separatorAct->setVisible(!windows.isEmpty());

    for (int i = 0; i < windows.size(); ++i) {
        MdiChild *child = qobject_cast<MdiChild *>(windows.at(i)->widget());

        QString text;
        if (i < 9) {
            text = tr("&%1 %2").arg(i + 1)
                    .arg(child->userFriendlyCurrentFile());
        } else {
            text = tr("%1 %2").arg(i + 1)
                    .arg(child->userFriendlyCurrentFile());
        }
        QAction *action  = windowMenu->addAction(text);
        action->setCheckable(true);
        action ->setChecked(child == activeMdiChild());
        connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
        windowMapper->setMapping(action, windows.at(i));
    }
}

MdiChild *MainWindow::createMdiChild()
{
    MdiChild *child = new MdiChild;
    child->setCompleter(this->completer);
    child->setStatusBar(this->statusBar());
    mdiArea->addSubWindow(child);

#ifndef QT_NO_CLIPBOARD
    connect(child, SIGNAL(copyAvailable(bool)),
            cutAct, SLOT(setEnabled(bool)));
    connect(child, SIGNAL(copyAvailable(bool)),
            copyAct, SLOT(setEnabled(bool)));
#endif

    return child;
}

void MainWindow::createActions()/****************************/
{
    //info menu
    tokenListAct = new QAction(QIcon(":/images/tokenlist.png"),tr("&Token List"),this);
    tokenListAct->setShortcut(QKeySequence(tr("Ctrl+L")));
    tokenListAct->setStatusTip(tr("Show Token List"));
    connect(tokenListAct,SIGNAL(triggered()),this,SLOT(tokenListSlot()));

    showProductTreeAct = new QAction(QIcon(":/images/productstree.png"),tr("P&roducts Tree"),this);
    connect(showProductTreeAct,SIGNAL(triggered()),this,SLOT(showProductTreeSlot()));

    parserTreeAct = new QAction(QIcon(":/images/parsertree.png"),tr("&Parser Tree"),this);
    parserTreeAct->setShortcut(QKeySequence(tr("Ctrl+P")));
    parserTreeAct->setStatusTip(tr("Show Parser Tree"));
    connect(parserTreeAct,SIGNAL(triggered()),this,SLOT(parserTreeSlot()));

    semanticsTableAct = new QAction(QIcon(":/images/semanticsTable.png"),tr("S&emantics Table"),this);
    semanticsTableAct->setShortcut(QKeySequence(tr("Ctrl+E")));
    semanticsTableAct->setStatusTip(tr("Show Semantics Table"));
    connect(semanticsTableAct,SIGNAL(triggered()),this,SLOT(semanticsTableSlot()));


    //Build Menu
    runCppActKean = new QAction(QIcon(":/images/runKean.png"),tr("&Run K"),this);
    runCppActKean->setShortcut(QKeySequence(tr("F9")));
    runCppActKean->setStatusTip("compile snl to cpp and run,using Kean's way.");
    connect(runCppActKean,SIGNAL(triggered()),this,SLOT(runCppKeanSlot()));

    runCppActBl = new QAction(QIcon(":/images/runBl.png"),tr("Run &B"),this);
    runCppActBl->setShortcut(QKeySequence(tr("F10")));
    runCppActBl->setStatusTip("compile snl to cpp and run,using Bl's way.");
    connect(runCppActBl,SIGNAL(triggered()),this,SLOT(runCppBlSlot()));

    buildAllAct = new QAction(QIcon(":/images/build.png"),tr("&Build All"),this);
    buildAllAct->setShortcut(QKeySequence(tr("F4")));
    buildAllAct->setStatusTip("Do the Follwing Four Steps Now");
    connect(buildAllAct,SIGNAL(triggered()),this,SLOT(buildAllSlot()));

    lexerAct = new QAction(QIcon(":/images/lexer.png"),tr("&Lexer"),this);
    lexerAct->setShortcut(QKeySequence(tr("F5")));
    lexerAct->setStatusTip(tr("Lexer"));
    connect(lexerAct,SIGNAL(triggered()),this,SLOT(lexerSlot()));

    parserLL1Act = new QAction(QIcon(":/images/parserLL1.png"),tr("&ParserLL1"),this);
    parserLL1Act->setShortcut(QKeySequence(tr("F6")));
    parserLL1Act->setStatusTip(tr("Parser LL1"));
    connect(parserLL1Act,SIGNAL(triggered()),this,SLOT(parserLL1Slot()));

    parserRecurAct = new QAction(QIcon(":/images/parserRecur.png"),tr("Parser&Recur"),this);
    parserRecurAct->setShortcut(QKeySequence(tr("F7")));
    parserRecurAct->setStatusTip(tr("Parser Recur"));
    connect(parserRecurAct,SIGNAL(triggered()),this,SLOT(parserRecurSlot()));


    semanticsAct = new QAction(QIcon(":/images/semantics.png"),tr("&Semantics"),this);
    semanticsAct->setShortcut(QKeySequence(tr("F8")));
    semanticsAct->setStatusTip(tr("Semantics"));
    connect(semanticsAct,SIGNAL(triggered()),this,SLOT(semanticsSlot()));


    newAct = new QAction(QIcon(":/images/new.png"), tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(QIcon(":/images/saveas.png"),tr("Save &As..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    //! [0]
    exitAct = new QAction(QIcon(":/images/exit.png"),tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));
    //! [0]

#ifndef QT_NO_CLIPBOARD
    cutAct = new QAction(QIcon(":/images/cut.png"), tr("Cu&t"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cutAct, SIGNAL(triggered()), this, SLOT(cut()));

    copyAct = new QAction(QIcon(":/images/copy.png"), tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, SIGNAL(triggered()), this, SLOT(copy()));

    pasteAct = new QAction(QIcon(":/images/paste.png"), tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, SIGNAL(triggered()), this, SLOT(paste()));
#endif

    closeAct = new QAction(QIcon(":/images/close.png"),tr("Cl&ose"), this);
    closeAct->setStatusTip(tr("Close the active window"));
    closeAct->setShortcut(QKeySequence(tr("Ctrl+G")));
    connect(closeAct, SIGNAL(triggered()),
            mdiArea, SLOT(closeActiveSubWindow()));

    closeAllAct = new QAction(QIcon(":/images/closeall.png"),tr("Close &All"), this);///////////////
    closeAllAct->setStatusTip(tr("Close all the windows"));
    closeAllAct->setShortcut(QKeySequence(tr("Ctrl+Q")));
    connect(closeAllAct, SIGNAL(triggered()),
            mdiArea, SLOT(closeAllSubWindows()));

    tileAct = new QAction(QIcon(":/images/fullscreen.png"),tr("&Tile View Mode"), this);
    tileAct->setStatusTip(tr("Tile the windows"));
    tileAct->setShortcut(QKeySequence(tr("Ctrl+I")));
    connect(tileAct, SIGNAL(triggered()), mdiArea, SLOT(tileSubWindows()));
    connect(tileAct, SIGNAL(triggered()), this, SLOT(disableTabViewSlot()));

    cascadeAct = new QAction(QIcon(":/images/cascade.png"),tr("&Cascade View Mode"), this);
    cascadeAct->setStatusTip(tr("Cascade the windows"));
    // cascadeAct->setShortcut();
    connect(cascadeAct, SIGNAL(triggered()), mdiArea, SLOT(cascadeSubWindows()));
    connect(cascadeAct, SIGNAL(triggered()), this, SLOT(disableTabViewSlot()));

    setMdiChildViewModeAct = new QAction(QIcon(":/images/settab.png"),tr("&Tab View Mode"),this);
    setMdiChildViewModeAct->setShortcut(QKeySequence(tr("Ctrl+T")));
    connect(setMdiChildViewModeAct,SIGNAL(triggered()),this,SLOT(setMdiChildViewModeSlot()));


    infoLogWindowShowHideAct = new QAction(QIcon(":/images/loginfo.png"),tr("Show /&Hide Info Log Window"),this);
    infoLogWindowShowHideAct->setShortcut(QKeySequence(tr("F2")));
    infoLogWindowShowHideAct->setStatusTip(tr("Show/Hide Info  Log Window"));
    connect(infoLogWindowShowHideAct,SIGNAL(triggered()),this,SLOT(infoLogWindowShowHideSlot()));

    treeWindowShowHideAct = new QAction(QIcon(":/images/treeWindow.png"),tr("Show /&Hide Tree Window"),this);
    treeWindowShowHideAct->setShortcut(QKeySequence(tr("Shift+F2")));
    treeWindowShowHideAct->setStatusTip(tr("Show/Hide Tree Window"));
    connect(treeWindowShowHideAct,SIGNAL(triggered()),this,SLOT(treeWindowShowHideSlot()));

    nextAct = new QAction(QIcon(":/images/next.png"),tr("Ne&xt"), this);
    nextAct->setShortcuts(QKeySequence::NextChild);
    nextAct->setStatusTip(tr("Move the focus to the next window"));
    connect(nextAct, SIGNAL(triggered()),
            mdiArea, SLOT(activateNextSubWindow()));

    previousAct = new QAction(QIcon(":/images/prev.png"),tr("Pre&vious"), this);
    previousAct->setShortcuts(QKeySequence::PreviousChild);
    previousAct->setStatusTip(tr("Move the focus to the previous "
                                 "window"));
    connect(previousAct, SIGNAL(triggered()),
            mdiArea, SLOT(activatePreviousSubWindow()));

    separatorAct = new QAction(this);
    separatorAct->setSeparator(true);

    showProductsAct = new QAction(QIcon(":/images/products.png"),tr("&Show Products"),this);
    showProductsAct->setShortcut(QKeySequence(tr("Ctrl+U")));
    connect(showProductsAct,SIGNAL(triggered()),this,SLOT(showProductsSlot()));

    showPredictsAct = new QAction(QIcon(":/images/predicts.png"),tr("&Show Predicts Set"),this);
    showPredictsAct->setShortcut(QKeySequence(tr("Ctrl+K")));
    connect(showPredictsAct,SIGNAL(triggered()),this,SLOT(showPredictsSlot()));

    separatorAct->setSeparator(true);
    aboutAct = new QAction(QIcon(":/images/idea.png"),tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(QIcon(":/images/about.png"),tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::createMenus()/*******************/
{
    fileMenu = menuBar()->addMenu(tr("File(&F)"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();

    /**QAction *action = fileMenu->addAction(tr("Switch layout direction"));
    connect(action, SIGNAL(triggered()), this, SLOT(switchLayoutDirection()));**/
    fileMenu->addAction(exitAct);


    editMenu = menuBar()->addMenu(tr("Edit(&E)"));
#ifndef QT_NO_CLIPBOARD
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);
#endif

    windowMenu = menuBar()->addMenu(tr("Window(&W)"));
    updateWindowMenu();
    connect(windowMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));

    menuBar()->addSeparator();

    /**********Build***********/
    buildMenu = menuBar()->addMenu(tr("Build(&B)"));
    buildMenu->addAction(buildAllAct);
    buildMenu->addAction(runCppActBl);
    buildMenu->addAction(runCppActKean);
    buildMenu->addSeparator();
    buildMenu->addAction(lexerAct);
    buildMenu->addAction(parserLL1Act);
    buildMenu->addAction(parserRecurAct);
    buildMenu->addAction(semanticsAct);
    /*****************Info****************/

    infoMenu = menuBar()->addMenu(tr("Info(&I)"));
    infoMenu->addAction(infoLogWindowShowHideAct);
    infoMenu->addAction(treeWindowShowHideAct);
    infoMenu->addAction(tokenListAct);
    infoMenu->addAction(parserTreeAct);
    infoMenu->addAction(showProductTreeAct);
    infoMenu->addAction(semanticsTableAct);
    /**************others*****************/
    othersMenu = menuBar()->addMenu(tr("Other(&O)"));
    othersMenu->addAction(showProductsAct);
    othersMenu->addAction(showPredictsAct);

    /*****************Help*****************/
    helpMenu = menuBar()->addMenu(tr("Help(&H)"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}

void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);



#ifndef QT_NO_CLIPBOARD
    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(cutAct);
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);
#endif

    buildToolBar = addToolBar(tr("Build"));
    buildToolBar->addAction(buildAllAct);
    buildToolBar->addAction(runCppActBl);
    buildToolBar->addSeparator();
    buildToolBar->addAction(lexerAct);
    buildToolBar->addAction(parserLL1Act);
    buildToolBar->addAction(parserRecurAct);
    buildToolBar->addAction(semanticsAct);

    infoToolBar = addToolBar(tr("Info"));
    infoToolBar->addAction(tokenListAct);
    infoToolBar->addAction(parserTreeAct);
    infoToolBar->addAction(showProductTreeAct);
    infoToolBar->addAction(semanticsTableAct);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings()
{
    QSettings settings("QtProject", "SNL Example");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    move(pos);
    resize(size);
}

void MainWindow::writeSettings()
{
    QSettings settings("QtProject", "SNL Example");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

MdiChild *MainWindow::activeMdiChild()
{
    if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
        return qobject_cast<MdiChild *>(activeSubWindow->widget());
    return 0;
}

QMdiSubWindow *MainWindow::findMdiChild(const QString &fileName)
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    foreach (QMdiSubWindow *window, mdiArea->subWindowList()) {
        MdiChild *mdiChild = qobject_cast<MdiChild *>(window->widget());
        if (mdiChild->currentFile() == canonicalFilePath)
            return window;
    }
    return 0;
}

void MainWindow::switchLayoutDirection()
{
    if (layoutDirection() == Qt::LeftToRight)
        qApp->setLayoutDirection(Qt::RightToLeft);
    else
        qApp->setLayoutDirection(Qt::LeftToRight);
}

void MainWindow::setActiveSubWindow(QWidget *window)
{
    if (!window)
        return;
    mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}

void MainWindow ::lexerSlot()
{
    this->save();
    qDebug()<<this->getCurrentFileName()<<" are u kidding me?";
    doLexer=doParserLl1=doParserRecur=doSemantics=false;
    if(this->getCurrentFileName().length()<=0)
    {
        //QMessageBox::information(this,"Warnning!","Please Save the File");
        doLexer=false;
        //return;
    }
    if(doLexer=lexer(this->getCurrentFileName().toStdString().c_str()))
    {
        outputTokens(false,"tokenList.txt");
        this->setInfoLogText("tokenList.txt");
    }
    else
    {
        outputLexerErrors(false,"lexerErrors.txt");
        this->setInfoLogText("lexerErrors.txt");
    }
#ifdef debugOut
    cout<<"Lexer Slot"<<endl;
#endif
}

void MainWindow::parserLL1Slot()
{
    doSemantics=false;
    initReadOneToken();
    if(doParserLl1=parserLL1())
    {
        outputParserTree(getParserLL1TreeRoot(),"parserLL1Tree.txt");
#ifndef ShowNewTree
        this->setInfoLogText("parserLL1Tree.txt");
#else
        this->treeViewRenew("Parser LL1 Tree","parserLL1Tree.txt");
        trView->expandToDepth(ParserTreeDefaultDepth);
        trView->show();
        logInfoEditWindow->insertPlainText(tr(" \nParserLL1 Finished Successfully!\n"));
#endif

    }
    else
    {
        outputParserLL1Errors(false,"parserLL1Errors.txt");
        this->setInfoLogText("parserLL1Errors.txt");
    }
    initReadOneToken();
#ifdef debugOut
    cout<<"Parser LL1 Slot"<<endl;
#endif
}
void MainWindow::parserRecurSlot()
{
    InitPredict();
    doSemantics=false;
    initReadOneToken();
    TreeNode * rt=parserecur();
    if(rt==NULL)
    {
        doParserRecur=false;
        this->setInfoLogText("out.txt");
        return;
    }
    outputParserTree(rt,"parserRecurTree.txt");
#ifndef ShowNewTree
    this->setInfoLogText("parserRecurTree.txt");
#else
    this->treeViewRenew("Parser Recur Tree","parserRecurTree.txt");
    trView->expandToDepth(ParserTreeDefaultDepth);
    trView->show();
    logInfoEditWindow->insertPlainText(tr(" \nParserRecur Finished Successfully!\n"));
#endif
    doParserRecur=true;

#ifdef debugOut
    cout<<"Parser Recur Slot"<<endl;
#endif
}
void MainWindow::semanticsSlot()
{
    if(!doParserLl1)
    {
        this->parserLL1Slot();
    }
    if(!doParserLl1) return;
    doSemantics=analyze(getParserLL1TreeRoot());
    if(doSemantics)
    {
        this->setInfoLogText("Semout.txt");

    }
    else
    {
        outputErrorlist(false,"semanticsError.txt");
        this->setInfoLogText("semanticsError.txt");
    }

#ifdef debugOut
    cout<<"semantics Slot"<<endl;
#endif

}

void MainWindow::parserTreeSlot()
{
    if(doParserLl1)
    {
#ifndef ShowNewTree
        this->setInfoLogText("parserLL1Tree.txt");
#else
        this->treeViewRenew("Parser LL1 Tree","parserLL1Tree.txt");
        trView->expandToDepth(ParserTreeDefaultDepth);
        trView->show();
#endif
    }
    else if(doParserRecur)
    {
#ifndef ShowNewTree
        this->setInfoLogText("parserRecurTree.txt");
#else
        this->treeViewRenew("Parser Recur Tree","parserRecurTree.txt");
        trView->expandToDepth(ParserTreeDefaultDepth);
        trView->show();
#endif
    }

}

void MainWindow::tokenListSlot()
{
    if(doLexer)
    {
        this->setInfoLogText("tokenList.txt");
    }

}

void MainWindow::semanticsTableSlot()
{
    if(doSemantics)
    {
        this->setInfoLogText("Semout.txt");
    }
    else
    {
        ;
    }

}

void MainWindow::buildAllSlot()
{
    lexerSlot();
    parserLL1Slot();
    semanticsSlot();
}

QString MainWindow::getCurrentFileName()
{
    if(this->activeMdiChild())
        return this->activeMdiChild()->currentFile();/////////////////

    if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
        return qobject_cast<MdiChild *>(activeSubWindow->widget())->currentFile();
    return "";
}

void MainWindow::infoLogWindowShowHideSlot()
{
    statusWindow->setHidden(!statusWindow->isHidden());
}
void MainWindow::setMdiChildViewModeSlot()
{
    if(!isTabView)
    {
        mdiArea->setViewMode(QMdiArea::TabbedView);
        isTabView=true;
    }
    else
    {
        mdiArea->setViewMode(QMdiArea::SubWindowView);
        isTabView=false;
    }
}
void MainWindow::disableTabViewSlot()
{
    isTabView=false;
    mdiArea->setViewMode(QMdiArea::SubWindowView);
}

bool MainWindow::setInfoLogText(const QString &fileName)
{
    if(QFile::exists(fileName))
    {
        QFile file(fileName);
        QTextStream in(&file);
        if (!file.open(QFile::ReadOnly | QFile::Text)) {
            QMessageBox::warning(this, tr("SNL"),
                                 tr("Cannot read file %1:\n%2.")
                                 .arg(fileName)
                                 .arg(file.errorString()));
            return false;
        }
        //QApplication::setOverrideCursor(Qt::WaitCursor);
        logInfoEditWindow->setPlainText(in.readAll());
        return true;
    }
    return false;
}

void MainWindow::showPredictsSlot()
{
    this->setInfoLogText(":/txt/predicts.txt");

}

void MainWindow::showProductsSlot()
{

    this->setInfoLogText(":/txt/products.txt");

}

void MainWindow::showProductTreeSlot()
{
    outputProductsTree("productsTree.txt");
    treeViewRenew("ProductsTree","productsTree.txt");
    trView->expandToDepth(5);  //trView->expandAll();
    trView->setColumnWidth(0,2000);
    trView->show();
}

void MainWindow::runCppBlSlot()
{
    if(!doSemantics) return;
    QString program="Astyle.exe --style=ansi -xd outcpp.cpp  outcpps.cpp";
    //QProcess::startDetached(program);
    astyleBeatify("outcpp.cpp");
    this->setInfoLogText("outcpp.cpp");
    QFile::remove("outCppBl.exe");
    QFile::remove("compileError.txt");
    compileCpp("outcpp.cpp","outCppBl.exe");
    if(QFile::exists("outCppBl.exe"))
    {
        program="C:/x64x86/CodeBlocks/cb_console_runner.exe  ./outCppBl.exe";
        QProcess::startDetached(program);
    }
    else
    {
        this->setInfoLogText("compileError.txt");
        logInfoEditWindow->insertPlainText(tr("Sorry To Tell You That Some Unexpected \
                                           Errors happened, While Compiling the CPP Code Using GCC.\
                                           (If You've Seen This Message That Shows An Awful Bug,Please \
                                           Just Inform Us ,We'll Fix This As Soon As Possible.) \\n"));
    }
}

void MainWindow::runCppKeanSlot()
{
    if(!doSemantics) return;
    snl2cpp("snl.cpp");
    this->setInfoLogText("snl.cpp");
    QFile::remove("outCppKean.exe");
    QFile::remove("compileError.txt");
    compileCpp("snl.cpp","outCppKean.exe");
    if(QFile::exists("outCppKean.exe"))
    {
        QString program="C:/x64x86/CodeBlocks/cb_console_runner.exe  ./outCppKean.exe";
        //runCppExe("outCppKean.exe");
        QProcess::startDetached(program);
    }
    else
    {
        this->setInfoLogText("CompileError.txt");
    }
}

/*******Completer********/

QAbstractItemModel *MainWindow::modelFromFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly))
        return new QStringListModel(completer);

#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
#endif
    QStringList words;

    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        if (!line.isEmpty())
            words << line.trimmed();
    }

#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
    return new QStringListModel(words, completer);
}


/*更新树*/
void MainWindow::treeViewRenew(const char treeName[], const char filename[])
{
    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    TreeModel * ptrmodel = new TreeModel(QString(treeName), file.readAll());
    file.close();
    QAbstractItemModel *tmpmdl=trView->model();
    trView->setModel(ptrmodel);
    delete tmpmdl;
    trView->setStyle(QStyleFactory::create("Windows"));
    trView->setColumnWidth(0,1000);
}

void MainWindow::treeWindowShowHideSlot()
{
    treeViewWindow->setHidden(!treeViewWindow->isHidden());
}
