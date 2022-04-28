#include "mainwindow.h"

#include <QApplication>
#include <QTextCodec>

QMutex LOGmutex;
bool FENCI_open;
int FENCI_select;
bool KS_OUTDetails;
bool REFUSEHTML;
int KS_SCANMODE;
QFile LOGfile("log.txt");
bool LOG_OPEN;
QFile NLPLog("NLPIRlog.txt");
int INTCON;
int INTFENC;

int main(int argc, char *argv[])
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

