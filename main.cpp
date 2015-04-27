#include <QApplication>
#include <QTranslator>
#include <QSplashScreen>
#include <QPixmap>
#include <ctime>
#include "mainwindow.h"


int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(mdi);

    QApplication app(argc, argv);
    QPixmap pixmap("./splash.png");
    QSplashScreen splash(pixmap);
    splash.show();
    splash.setFont(QFont("Times", 20, QFont::DemiBold));
   // splash.showMessage("\n\n\n\n正在载入文件..........");
    app.processEvents();
    int startTime=clock(),seg=0;
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
