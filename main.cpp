#include <QApplication>
#include <QTranslator>
#include <QSplashScreen>
#include <QPixmap>
#include <ctime>
#include "mainwindow.h"

void loadFiles();
int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(mdi);

    QApplication app(argc, argv);
    QPixmap pixmap("./splash.png");
    QSplashScreen splash(pixmap);
    splash.show();
    splash.setFont(QFont("Times", 20, QFont::DemiBold));
    ///splash.showMessage("\n\n\n\n正在载入文件..........");
    app.processEvents();
    int startTime=clock(),seg=0;
    loadFiles();
    while(seg < 3000)
    {
        seg=clock()-startTime;
    }

    QTranslator translator;
    translator.load("SNLCompiler_zh_cn.qm");
    app.installTranslator(&translator);
    MainWindow mainWin;
    mainWin.show();
    splash.finish(&mainWin);
    return app.exec();
}

bool __loadFile(QString name)
{
    if(!QFile::exists("./"+name))
    {
        QFile infile(":/others/"+name);
        if (!infile.open(QIODevice::ReadOnly))
                return false;///errors
        QFile outfile("./"+name);
        outfile.open(QIODevice::WriteOnly);
        outfile.write(infile.readAll());
        infile.close();
        outfile.close();
    }
    return true;
}

void loadFiles()
{
    if(QFile::exists("notloadfile"))
        return;
    __loadFile("runner.exe");
    __loadFile("SNLCompiler_zh_cn.qm");
    __loadFile("AStyle.exe");
    __loadFile("splash.png");
    QFile notload("notloadfile");
    notload.open(QIODevice::WriteOnly);
    notload.close();
}
