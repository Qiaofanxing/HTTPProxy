#include "mainwindow.h"

#include <QApplication>
#include <QTextCodec>

QMutex mutex;

int main(int argc, char *argv[])
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

  //          QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

