#include "myproxyserver.h"
#include "serverthread.h"

myProxyServer::myProxyServer(QObject *parent)
    : QTcpServer(parent)
{

}

void myProxyServer::incomingConnection(qintptr socketDescriptor)
{

    if(socketDescriptor != lastsocketDescriptor)
    {
        qDebug()<<"SOCKET描述符："<<socketDescriptor;
        ServerThread *thread = new ServerThread(socketDescriptor, this);
        connect(thread,&ServerThread::finished, thread, &ServerThread::deleteLater);
        connect(thread,&ServerThread::needChangeText,this,&myProxyServer::calltoChangeText);
        lastsocketDescriptor = socketDescriptor;
        thread->start();
    }
}

void myProxyServer::calltoChangeText(QByteArray cmd)
{
    emit changeUITextSignal(cmd);
}
