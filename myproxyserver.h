#ifndef MYPROXYSERVER_H
#define MYPROXYSERVER_H

#include <QTcpServer>


class myProxyServer : public QTcpServer
{
    Q_OBJECT
public:
    myProxyServer(QObject *parent = nullptr);

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private:
    qintptr lastsocketDescriptor = 0;

signals:
    void changeUITextSignal(QByteArray cmd);

private slots:
    void calltoChangeText(QByteArray cmd);

};

#endif // MYPROXYSERVER_H
