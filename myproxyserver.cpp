#include "myproxyserver.h"
#include "mainwindow.h"


myProxyServer::myProxyServer(QObject *parent)
    : QTcpServer(parent)
{

}

myProxyServer::~myProxyServer()
{
    if(!allThread.isEmpty())
    {
        for(int i=0;i<allThread.size();i++){
            if(allThread.at(i)->isRunning())
            {
                allThread.at(i)->quit();
                allThread.at(i)->wait();
                allThread.remove(i);
                i--;
            }
        }
    }
}

void myProxyServer::incomingConnection(qintptr socketDescriptor)
{

        qDebug()<<"SOCKET描述符："<<socketDescriptor;
        ServerThread *thread = new ServerThread(socketDescriptor, this);
        connect(thread,&ServerThread::finished, thread, &ServerThread::deleteLater);
        connect(thread,&ServerThread::finished, this, &myProxyServer::onThreadExited);
        connect(thread,&ServerThread::needChangeText,this,&myProxyServer::calltoChangeText);
        INTCON ++;
        thread->start();
        allThread.append(thread);

}

void myProxyServer::calltoChangeText(QByteArray cmd)
{
    emit changeUITextSignal(cmd);
}

void myProxyServer::onThreadExited()
{
 //    emit changeUITextSignal("子线程结束");
    if(INTCON >0)
        INTCON--;
}

void myProxyServer::closeAllThread()
{
 //    emit changeUITextSignal("子线程结束");
    if(!allThread.isEmpty())
    {
        for(int i=0;i<allThread.size();i++){
            if(allThread.at(i)->isRunning())
            {
                allThread.at(i)->quit();
                allThread.remove(i);
                i--;
            }
        }
    }
}
