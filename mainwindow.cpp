#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "myproxyserver.h"
#include <QtWidgets>
#include "KeyScanAPI.h"
#include "NLPIR.h"


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
    _InitKSService();
  //  _InitZLIB();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ChangeUIText(QByteArray cmd)
{
    ui->text_output->append(cmd.toStdString().c_str());
}

void MainWindow::_InitKSService()
{
    qint64 res1 = KS_Init(0,UTF8_CODE);
    ui->text_output->append("KS初始化状态:"+QString::number(res1));
//     ui->text_output->append(KS_GetLastErrorMsg());
    qint64 res2 = NLPIR_Init(0,UNKNOWN_CODE);
    ui->text_output->append("NLPIR初始化状态:"+QString::number(res2));

 //   qDebug()<<KS_ImportUserDict("./keylist.txt");

  //  QString cmd = "六合彩你玩过吗？";
  //  QString result = KS_Scan(cmd.toStdString().c_str());
//    ui->text_output->append(result);

 //   KS_ExportDict("testKEYShow.txt");
}


void MainWindow::_InitZLIB()
{
    QByteArray src;
       src.append("中华人民共和国,china mobile,123456");
/*
       unsigned long len_compress;
       unsigned long len_uncompress;

       unsigned char *buffer_compress  = new unsigned char[1000];

       unsigned char *buffer_uncompress = new unsigned char[1000];


        qDebug() << compress(buffer_compress,&len_compress,(Bytef*)src.data(), src.length());
        qDebug() << uncompress(buffer_uncompress,&len_uncompress,buffer_compress,len_compress);



       qDebug() << "qt zlib:";
       qDebug()  << "src:\n" << src.data() << ",length:" << src.size() ;

       qDebug()  << "compress:\n" << buffer_compress << ",length:" << len_compress ;
       qDebug()  << "uncompress:\n" << buffer_uncompress << ",length:" << len_uncompress;
*/

      QByteArray cmd1 =  Compress(src);
      qDebug()<<cmd1;
      QByteArray cmd2 =  UnCompress(cmd1);
      QString xx = cmd2;
      qDebug()<<xx;
}

QByteArray MainWindow::Compress(QByteArray postBody)
{
    QByteArray outBuf;
    z_stream c_stream;

    int GZIP_ENCODING = 16;
    if (!postBody.isEmpty())
    {
        c_stream.zalloc = (alloc_func)0;
        c_stream.zfree = (free_func)0;
        c_stream.opaque = (voidpf)0;
        c_stream.next_in = (Bytef *)postBody.data();
        c_stream.avail_in = postBody.size();
        if (deflateInit2(&c_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
            MAX_WBITS + GZIP_ENCODING, 8, Z_DEFAULT_STRATEGY) != Z_OK) return QByteArray();
        for (;;) {
            char destBuf[4096] = { 0 };
            c_stream.next_out = (Bytef *)destBuf;
            c_stream.avail_out = 4096;
            int err = deflate(&c_stream, Z_FINISH);
            outBuf.append(destBuf, 4096 - c_stream.avail_out);
            if (err == Z_STREAM_END || err != Z_OK)
            {
                break;
            }
        }
        deflateEnd(&c_stream);
    }
    return outBuf;
}


QByteArray MainWindow::UnCompress(QByteArray src)
{
    QByteArray outBuffer;
    z_stream strm;
    strm.zalloc = NULL;
    strm.zfree = NULL;
    strm.opaque = NULL;

    strm.avail_in = src.size();
    strm.next_in = (Bytef *)src.data();

    int err = -1;// ret = -1;
    err = inflateInit2(&strm, MAX_WBITS + 16);
    if (err == Z_OK) {
        while (true)
        {
            char buffer[4096] = { 0 };
            strm.avail_out = 4096;
            strm.next_out = (Bytef *)buffer;
            int code = inflate(&strm, Z_FINISH);
            outBuffer.append(buffer, 4096 - strm.avail_out);
            if (Z_STREAM_END == code)// || Z_OK != code) 防止截取半头
            {
                break;
            }
        }
    }
    inflateEnd(&strm);
    return outBuffer;
}
