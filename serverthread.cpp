#include "serverthread.h"
#include <QStringList>
#include <QString>
#include <QRegularExpression>
#include <QtNetwork>

ServerThread::ServerThread(int socketDescriptor,  QObject *parent)
    : QThread(parent), socketDescriptor(socketDescriptor)
{

}

void ServerThread::run()
{
    emit needChangeText("子线程启动");
    QTcpSocket ClientSocket;
    QNetworkAccessManager ServerManager;
    if (!ClientSocket.setSocketDescriptor(socketDescriptor)) {
        emit error(ClientSocket.error());
        return;
    }
    ClientSocket.waitForConnected(-1);
    while(ClientSocket.state() == QAbstractSocket::ConnectedState)
    {
    QNetworkRequest *requestInfo = nullptr;
    HttpHeader *httpHeader = nullptr;
    ClientSocket.waitForReadyRead();
    if(ClientSocket.state() == QAbstractSocket::UnconnectedState)
    {
       if(requestInfo != nullptr)
           delete requestInfo;
       if(httpHeader != nullptr)
           delete httpHeader;
       requestInfo = nullptr;
       httpHeader = nullptr;
        return;
    }
    requestInfo = new QNetworkRequest;
    httpHeader = new HttpHeader;
    QString  str_;
    str_.clear();
    str_ = ClientSocket.readAll();
    if(str_.isEmpty())
    {
        if(requestInfo != nullptr)
            delete requestInfo;
        if(httpHeader != nullptr)
            delete httpHeader;
        requestInfo = nullptr;
        httpHeader = nullptr;
        continue;
    }
 //   emit needChangeText(str_.toUtf8());
    ParseHttpHead(&str_,requestInfo,httpHeader);

    QNetworkReply *reply = nullptr;

    if(httpHeader->method=="GET")
    {
        reply = ServerManager.get(*requestInfo);
        emit needChangeText("远程服务器地址:"  + httpHeader->url.toUtf8());
    }
    else if(httpHeader->method=="POST")
    {
        reply = ServerManager.post(*requestInfo,httpHeader->body.toUtf8());
    }
    else
    {
        reply = ServerManager.sendCustomRequest(*requestInfo,httpHeader->body.toUtf8());
    }
    QEventLoop eventLoop;
    connect(&ServerManager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
    eventLoop.exec();

    emit needChangeText("收到服务器消息，开始处理信息");
    if (reply->error() == QNetworkReply::NoError)
    {
        emit needChangeText("Reply 正常");

        QList < QNetworkReply::RawHeaderPair> temp(reply->rawHeaderPairs());
       //  QVariant strJsonText= temp[0];
        /*
         for(int i=0;i<temp.size();i++)
         {
             emit resultNotify(temp[i].first + ": "+ temp[i].second);
         }

         QByteArray strJsonText = reply->readAll();
         emit resultNotify(strJsonText);
         */
        QVariant requestType = reply->header(QNetworkRequest::ContentTypeHeader);
        emit needChangeText(requestType.toByteArray());
         QByteArray datarequest;
         datarequest.append("HTTP/1.1 200 OK\r\n");
         for(int i=0;i<temp.size();i++)
         {
             datarequest.append(temp[i].first);
             datarequest.append(": ");
             datarequest.append(temp[i].second);
             datarequest.append("\r\n");
         }
         datarequest.append("\r\n");
         datarequest.append(reply->readAll());

         emit needChangeText("-----返回信息-----");
     //    emit needChangeText(datarequest);
         emit needChangeText("-----END-----");

         if(!ClientSocket.write(datarequest))
         {
             emit error(ClientSocket.error());
             if(requestInfo != nullptr)
                 delete requestInfo;
             if(httpHeader != nullptr)
                 delete httpHeader;
             requestInfo = nullptr;
             httpHeader = nullptr;
             return;
         }
         ClientSocket.waitForBytesWritten();
         ClientSocket.flush();
         reply->deleteLater();
    }
    else
    {
        emit needChangeText("Reply Error:");
        QVariant statusCodeV = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
        qDebug()<<reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
        emit needChangeText(statusCodeV.toByteArray());
    }
    delete requestInfo;
    requestInfo = nullptr;
    delete httpHeader;
    httpHeader = nullptr;
    }
    ClientSocket.waitForDisconnected(-1);
}

void ServerThread::ParseHttpHead(QString* data,QNetworkRequest* finalRequest,HttpHeader * httpHeader)
{
 //   qDebug()<<data;
    QStringList list = data->split(QRegularExpression("[\r\n]"),Qt::KeepEmptyParts);
    int index = 0;
    //提取第一行:
    QStringList insidething = list[index].split(" ");
 //   emit needChangeText("-----HTTP头开始------");
 //   emit needChangeText(insidething[0].toUtf8() + "|分隔符|"+ insidething[1].toUtf8() +"|分隔符|" + insidething[2].toUtf8());
    httpHeader->method = insidething[0] ;
    httpHeader->url = insidething[1];
    qDebug()<<insidething[1];
    finalRequest->setUrl(insidething[1]);
    index+=2;
   // emit needChangeText("-----HTTP头结束------");
  //  emit needChangeText("-----HTTP正文------");
    //提取后面几行
    for(;index < list.length() && (!list[index].isEmpty())  ; index += 2 )
    {
        if(!(index <list.length()))
            break;
        QStringList insidething = list[index].split(QRegularExpression("[:]"),Qt::KeepEmptyParts);
        QString cmd1,cmd2;
        cmd1.clear();
        cmd2.clear();
        for(int j=0 ; j < insidething.length();j++)
        {
            if(j == 0)
            {
                cmd1=insidething[0];
            }
            else
                cmd2 += insidething[j];
        }
  //      emit needChangeText(cmd1.toUtf8() + ":" + cmd2.toUtf8());
        finalRequest->setRawHeader(cmd1.toUtf8(),cmd2.toUtf8());
   //     emit needChangeText("------------------");
    }
    index++;
 //   emit needChangeText("-----以下是BODY部分--------");
    for(;index< list.length();index++)
    {
        httpHeader->body +=list[index];
   //     emit needChangeText( httpHeader->body.toUtf8());
    }
}
