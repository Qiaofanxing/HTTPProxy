#include "serverthread.h"
#include <QStringList>
#include <QString>
#include <QRegularExpression>
#include <QtNetwork>
#include "KeyScanAPI.h"
#include "NLPIR.h"
#include <QTextCodec>
#include "mainwindow.h"

ServerThread::ServerThread(int socketDescriptor,  QObject *parent)
    : QThread(parent), socketDescriptor(socketDescriptor)
{

}

void ServerThread::run()
{
  //  emit needChangeText("子线程启动");
    QTcpSocket ClientSocket;
    QNetworkAccessManager ServerManager;

    if (!ClientSocket.setSocketDescriptor(socketDescriptor)) {
        emit error(ClientSocket.error());
        return;
    }
    ClientSocket.waitForConnected(-1); //确保连接上socket

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
//    emit needChangeText(str_.toUtf8());
    ParseHttpHead(&str_,requestInfo,httpHeader);

    QNetworkReply *reply = nullptr;

    if(httpHeader->method=="GET")
    {
        reply = ServerManager.get(*requestInfo);
   //     emit needChangeText("发送了一个GET:"  + httpHeader->url.toUtf8());
    }
    else if(httpHeader->method=="POST")
    {
    //   emit needChangeText("发送了一个POST:"+ httpHeader->body.toUtf8());
        reply = ServerManager.post(*requestInfo,httpHeader->body.toUtf8());
    }
    else
    {
  //      emit needChangeText("发送了一个"+httpHeader->method.toUtf8());
        reply = ServerManager.sendCustomRequest(*requestInfo,httpHeader->body.toUtf8());
    }

    QEventLoop eventLoop;
    connect(&ServerManager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
    eventLoop.exec();

    bool IS_GZIP = false, IS_HTML = false;
 //   emit needChangeText("收到服务器消息，开始处理信息");
    if (reply->error() == QNetworkReply::NoError)
    {
    //    emit needChangeText("Reply 正常");
        QByteArray info_body = reply->readAll();

        IS_GZIP = false;
        IS_HTML = false;
   //     emit needChangeText(info_body);

        QList < QNetworkReply::RawHeaderPair> temp(reply->rawHeaderPairs());
        /*
         QVariant strJsonText= temp[0];
         for(int i=0;i<temp.size();i++)
         {
             emit resultNotify(temp[i].first + ": "+ temp[i].second);
         }

         QByteArray strJsonText = reply->readAll();
         emit resultNotify(strJsonText);
         */
        QVariant requestType = reply->header(QNetworkRequest::ContentTypeHeader);
 //       emit needChangeText(requestType.toByteArray());

         QByteArray datarequest;
         datarequest.append("HTTP/1.1 200 OK\r\n");
         for(int i=0;i<temp.size();i++)
         {
             if(temp[i].first == "Vary" && temp[i].second.contains("Accept-Encoding"))
             {
           //     qDebug()<<temp[i].first;
            //    qDebug()<<temp[i].second;
                 IS_GZIP = true;
                qDebug()<<"SET IS_GZIP";
             }
             if(temp[i].first == "Content-Type" && temp[i].second.contains("text/html"))
             {
                qDebug()<<temp[i];
                 IS_HTML = true;
                qDebug()<<"SET IS_HTML";
             }
             datarequest.append(temp[i].first);
             datarequest.append(": ");
             datarequest.append(temp[i].second);
             datarequest.append("\r\n");
         }
         datarequest.append("\r\n");
 //        emit needChangeText(info_body);
/*
        QByteArray  tempouta = uncompressGZip(info_body);
        emit needChangeText(tempouta);
*/
         if(IS_HTML)
         {
     //        emit needChangeText("检测到文本内容:");
              CNLPIR *CNPLR_FENCI;
              KS_HANDLE KS_FENCI;
             if(FENCI_open)
             {
                 if(FENCI_select == 0)
                 {
                     KS_FENCI = KS_NewInstance();
                 }
                 else
                 {
                     CNPLR_FENCI = GetActiveInstance();
                     CNPLR_FENCI->SetAvailable(false);
                 }

             }
/*
QTextCodec *tc = QTextCodec::codecForName("GB2312");
QString tmpQStr = tc->toUnicode(info_body);

QString tmpQStrx = QString::fromLatin1(tmpQStr.toLatin1());
 qDebug()<<tmpQStrx;
*/
/*
             mutex.lock();
             QFile   aFile("things.txt");
             qDebug()<<"试图打开：";
            qDebug()<< aFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);

             aFile.write(tmpQStr,info_body.length());  //写入文件
             aFile.close();
             mutex.unlock();
             */

         //    QByteArray  tempout = NLPIR_ParagraphProcess(info_body);
        //      emit needChangeText("=====================");
           //  QString tempx = QTextCodec::codecForName("GBK")->toUnicode(info_body);
        //     qDebug()<<info_body.toStdString().c_str();
       //       emit needChangeText("=======================");   
    //          emit needChangeText(info_body);
             //        QString tempx = QTextCodec::codecForName("GBK")->toUnicode(temp2);
             //      qDebug()<<tempout;
             //     emit needChangeText(tempout);
             QByteArray  tempout,tempoutword;

             if(IS_GZIP)
             {
       //          emit needChangeText("解压缩：");
             //    tempout = uncompressGZip(info_body);
                 QString htmlbody = uncompressGZip(info_body);
                 htmlbody.replace(QRegularExpression("(?is)<.*?>"),"");
                 htmlbody.replace("\t","");
                 htmlbody.replace("\n","");
                 htmlbody.replace("&nbsp","");
                 htmlbody.replace("\r","");
       //          qDebug()<<htmlbody;
           //     emit needChangeText(tempout);
                 datarequest.append(info_body);
                if(FENCI_open)
                {
                    if(FENCI_select == 0)
                    {
                        if(KS_OUTDetails == true)
                        {
                            emit needChangeText("KS_OUTDetails == true");
                            switch(KS_SCANMODE)
                            {
                            case 0:
                                tempoutword = KS_ScanDetail(htmlbody.toUtf8(),0,KS_FENCI);
                                emit needChangeText("KS_SCANMODE =0");
                                break;
                            case 1:
                                tempoutword = KS_ScanDetail(htmlbody.toUtf8(),1,KS_FENCI);
                                break;
                            case 2:
                                tempoutword = KS_ScanDetail(htmlbody.toUtf8(),2,KS_FENCI);
                                break;
                            case 3:
                                tempoutword = KS_ScanDetail(htmlbody.toUtf8(),3,KS_FENCI);
                                break;
                            default:
                                break;
                            }
                        }
                        else
                        {
                            tempoutword = KS_Scan(htmlbody.toUtf8(),KS_FENCI);
                            qDebug()<<tempoutword;
                        }
                    }
                    else
                    {
                        LOGmutex.lock();
                        NLPLog.write(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz ddd").toUtf8());
                        NLPLog.write("\n");
                        NLPLog.write(NLPIR_ParagraphProcess(htmlbody.toUtf8(),0));
                        LOGmutex.unlock();
                        emit needChangeText("精细分词成功，结果已经写入日志");
                    }
                }
             }
             else
             {
        //         emit needChangeText("不用压缩：");
                 datarequest.append(info_body);
          //        emit needChangeText(info_body);
                 QString htmlbody = info_body;
                 htmlbody.replace(QRegularExpression("(?is)<.*?>")," ");
                 htmlbody.replace("\t"," ");
                 htmlbody.replace("\n"," ");
                 htmlbody.replace("&nbsp;"," ");
                 htmlbody.replace("\r"," ");
                 if(FENCI_open)
                 {
                     if(FENCI_select == 0)
                     {
                         if(KS_OUTDetails == true)
                         {
                             switch(KS_SCANMODE)
                             {
                             case 0:
                                 tempoutword = KS_ScanDetail(htmlbody.toUtf8(),0,KS_FENCI);
                                 break;
                             case 1:
                                 tempoutword = KS_ScanDetail(htmlbody.toUtf8(),1,KS_FENCI);
                                 break;
                             case 2:
                                 tempoutword = KS_ScanDetail(htmlbody.toUtf8(),2,KS_FENCI);
                                 break;
                             case 3:
                                 tempoutword = KS_ScanDetail(htmlbody.toUtf8(),3,KS_FENCI);
                                 break;
                             default:
                                 break;
                             }
                         }
                         else
                         {
                             tempoutword = KS_Scan(htmlbody.toUtf8(),KS_FENCI);
                             qDebug()<<tempoutword;
                         }
                     }
                     else
                     {
                         LOGmutex.lock();
                         NLPLog.write(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz ddd").toUtf8());
                         NLPLog.write("\n");
                         NLPLog.write(NLPIR_ParagraphProcess(htmlbody.toUtf8(),0));
                         LOGmutex.unlock();
                         emit needChangeText("精细分词成功，结果已经写入日志");
                     }
                 }
             }
             if(!tempoutword.isEmpty())
             {
                 INTFENC++;
                 emit needChangeText("发现敏感词：");
                 emit needChangeText(tempoutword);
                 if(LOG_OPEN)
                 {
                     LOGmutex.lock();

                     LOGfile.write(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz ddd").toUtf8());
                     LOGfile.write(" : ");
                     LOGfile.write(tempoutword);
                     LOGfile.write(" \n ");

                     LOGmutex.unlock();
                 }
                 if(REFUSEHTML)
                 {
                     ClientSocket.close();
                     if(FENCI_open)  //结束分词线程
                     {
                         if(FENCI_select == 0)
                         {
                             KS_DeleteInstance(KS_FENCI);
                         }
                         else
                         {
                             CNPLR_FENCI->SetAvailable(true);
                         }
                     }
                     break;
                 }

                 reply->deleteLater();
             }

             if(FENCI_open)  //结束分词线程
             {
                 if(FENCI_select == 0)
                 {
                     KS_DeleteInstance(KS_FENCI);
                 }
                 else
                 {
                     CNPLR_FENCI->SetAvailable(true);
                 }
             }
         }
         else
         {  //如果不是HTML
            datarequest.append(info_body);
         }
         /*
         emit needChangeText("-----返回信息-----");
         emit needChangeText(datarequest);
         emit needChangeText("-----END-----");
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
*/

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
   //     emit needChangeText("Reply Error:");
   //     int tempxx=reply->error();
  //      emit needChangeText(QString::number(tempxx).toUtf8());
        qDebug()<<reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
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
  //  emit needChangeText("-----HTTP头开始------");
 //   emit needChangeText(insidething[0].toUtf8() + " "+ insidething[1].toUtf8() +" " + insidething[2].toUtf8());
    httpHeader->method = insidething[0] ;
    httpHeader->url = insidething[1];
    qDebug()<<insidething[1];
    finalRequest->setUrl(insidething[1]);
    index+=2;
 //   emit needChangeText("-----HTTP头结束------");
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
      //       if(insidething[0] == "Accept-Encoding")
       //        {
      //           cmd1 = "Accept-Encoding";
      //           cmd2 = "deflate";
       //      }
    //    emit needChangeText(cmd1.toUtf8() + ":" + cmd2.toUtf8());
        finalRequest->setRawHeader(cmd1.toUtf8(),cmd2.toUtf8());
   //     emit needChangeText("------------------");
    }
    index++;
 //   emit needChangeText("-----以下是BODY部分--------");
    for(;index< list.length();index++)
    {
        httpHeader->body +=list[index].toUtf8();
    //    emit needChangeText( httpHeader->body.toUtf8());
    }
}

QByteArray ServerThread::uncompressGZip(const QByteArray &data)
{
    if (data.size() <= 4)
    {
        qWarning("Data Error");
        return QByteArray();
    }
    QByteArray result;
    int ret;
    z_stream strm;
    static const int CHUNK_SIZE = 1024;
    char out[CHUNK_SIZE];

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = data.size();
    strm.next_in = (Bytef*)(data.data());
    ret = inflateInit2(&strm, 15 +  32);
    if (ret != Z_OK)
        return QByteArray();
    // 解压缩：
    do
    {
        strm.avail_out = CHUNK_SIZE;
        strm.next_out = (Bytef*)(out);
        ret = inflate(&strm, Z_NO_FLUSH);
        Q_ASSERT(ret != Z_STREAM_ERROR);
        switch (ret) {
        case Z_NEED_DICT:
        case Z_DATA_ERROR:
        case Z_MEM_ERROR:
            (void)inflateEnd(&strm);
            return QByteArray();
        }
        result.append(out, CHUNK_SIZE - strm.avail_out);
    } while (strm.avail_out == 0);
    // 删除缓存
   inflateEnd(&strm);
   return result;
}
