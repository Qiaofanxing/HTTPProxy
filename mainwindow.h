#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "myproxyserver.h"
#include <QMutex>
#include <zlib.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    myProxyServer server;
    void _InitKSService();
    void _InitZLIB();
    QByteArray Compress(QByteArray postBody);
    QByteArray UnCompress(QByteArray postBody);
private slots:
    void ChangeUIText(QByteArray cmd);
};

extern QMutex mutex;

#endif // MAINWINDOW_H
