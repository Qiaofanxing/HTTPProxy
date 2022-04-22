#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QNetworkRequest>

class HttpHeader
{
public:
    QString method;
    QString url; // 请求的 url
    char host[1024]; // 目标主机
    QString others;
    QString body;
    HttpHeader() {
        memset(this,'\0', sizeof(HttpHeader));
    }
};

class ServerThread : public QThread
{
    Q_OBJECT
public:
    ServerThread(int socketDescriptor, QObject *parent);

    void run() override;

private:
    int socketDescriptor;
    QString text;
    void ParseHttpHead(QString*,QNetworkRequest*,HttpHeader*);
private slots:


signals:
    void error(QTcpSocket::SocketError socketError);
    void needChangeText(QByteArray cmd);
};

#endif // SERVERTHREAD_H
