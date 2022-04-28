#ifndef MYPROXYSERVER_H
#define MYPROXYSERVER_H

#include <QTcpServer>
#include "serverthread.h"

class myProxyServer : public QTcpServer
{
    Q_OBJECT
public:
    myProxyServer(QObject *parent = nullptr);
    ~myProxyServer();
    void closeAllThread();
protected:
    void incomingConnection(qintptr socketDescriptor) override;

private:
    QList <ServerThread*> allThread;
signals:
    void changeUITextSignal(QByteArray cmd);
    void ChangeUIOptionsSignal(int select , QByteArray cmd);

private slots:
    void calltoChangeText(QByteArray cmd);
    void onThreadExited();
};

#endif // MYPROXYSERVER_H
