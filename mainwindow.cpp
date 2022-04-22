#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "myproxyserver.h"
#include <QtWidgets>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(&server,&myProxyServer::changeUITextSignal,this,&MainWindow::ChangeUIText);

    const QHostAddress addr("127.0.0.1");
    const quint16 port = 10240;
    if (!server.listen(addr,port)) {
            QMessageBox::critical(this, tr("Threaded Fortune Server"),
                                  tr("Unable to start the server: %1.")
                                  .arg(server.errorString()));
            close();
            return;
        }
    ui->text_output->append("服务器启动成功");

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ChangeUIText(QByteArray cmd)
{
    ui->text_output->append(cmd);
}
