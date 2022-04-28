#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "myproxyserver.h"
#include "KeyScanAPI.h"
#include "NLPIR.h"
#include <QtNetwork>
#include <QRegularExpression>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _InitUI();
    _InitKSService();
    _IniLog();
}

MainWindow::~MainWindow()
{
    delete ui;
    KS_Exit();
    NLPIR_Exit();
}

void MainWindow::_InitUI()
{
    //全局变量第一次初始化：
     FENCI_open = ui->checkBox_FENCI_isopen->isChecked();
     FENCI_select = 0;
     KS_OUTDetails = ui->checkBox_KS_Details->isChecked();
     REFUSEHTML = ui->radioButton_refuseHTML->isChecked();
     KS_SCANMODE = 0;
     LOG_OPEN = ui->checkBox_KS_savehistory->isChecked();
     INTCON = 0;
     INTFENC = 0;

    //END

     this->setFixedSize(this->geometry().size());
     this->setWindowIcon(QIcon(":/image/QResource/icon.svg"));
     this->setWindowTitle("基于http代理的内容过滤分析");
     this->setWindowFlags(Qt::WindowCloseButtonHint);

    QString scrollstyle="QScrollBar:vertical"
                           "{"
                           "background-color: #FFFFFF;"
                           "color: #000000;"
                           "}";
    ui->text_output->verticalScrollBar()->setStyleSheet(scrollstyle);
    ui->statusbar->setSizeGripEnabled(false);
    ///获取本机所有可用IP地址
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    for(auto address:list)
        if (address.protocol() == QAbstractSocket::IPv4Protocol)
            ui->comboBox_serverIP->addItem(address.toString());
    ui->comboBox_serverIP->setCurrentIndex(ui->comboBox_serverIP->findText("127.0.0.1"));

//     ui->statusbar->showMessage(tr("临时信息!"),2000);

    ui->action_stopServer->setDisabled(true);

    perFENCI = new QLabel("快速过滤模式", this);
    ui->statusbar->addPermanentWidget(perFENCI);
    ui->statusbar->setMinimumWidth(180);

    logShow = new QLabel("",this);
    ui->statusbar->addPermanentWidget(logShow);
    ui->statusbar->setMinimumWidth(180);

    //KS:
    ui->pushButton_KS_addLine->setDisabled(true);
    ui->pushButton_KS_delLine->setDisabled(true);
    ui->pushButton_KS_inLine->setDisabled(true);
    ui->pushButton_KS_save->setDisabled(true);
    ui->pushButton_KS_load->setEnabled(true);

    ui->tabWidgetOPTIONS->setCurrentIndex(0);

    ui->tableView_KS->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QTimer *pTimer=new QTimer();
        connect(pTimer, &QTimer::timeout, [=](){
            ui->lcdNumber_connect->display(INTCON);
        });
        connect(pTimer, &QTimer::timeout, [=](){
            ui->lcdNumber_FENCI->display(INTFENC);
        });
     pTimer->start(500);

}

void MainWindow::ChangeUIText(QByteArray cmd)
{
    ui->text_output->append(cmd.toStdString().c_str());
}

bool MainWindow::_InitKSService()
{
    qint64 res1 = KS_Init(0,UTF8_CODE);
    ui->text_output->append("KS初始化状态:"+QString::number(res1));
//     ui->text_output->append(KS_GetLastErrorMsg());
    qint64 res2 = NLPIR_Init(0,UTF8_CODE);
    ui->text_output->append("NLPIR初始化状态:"+QString::number(res2));
    if(res1 != 1)
    {
         ui->text_output->append(KS_GetLastErrorMsg());
         return false;
    }
    if(res2 != 1)
    {
        ui->text_output->append(NLPIR_GetLastErrorMsg());
        return false;
    }
    KS_ImportUserDict("keylist.txt",true);
    return true;
}

bool MainWindow::_InitServer()
{
    connect(&server,&myProxyServer::changeUITextSignal,this,&MainWindow::ChangeUIText);
    connect(&server,&myProxyServer::ChangeUIOptionsSignal,this,&MainWindow::ChangeUIOptions);
    INTCON = 0;
    INTFENC = 0;
    const QHostAddress addr(ui->comboBox_serverIP->currentText());
    const quint16 port = ui->spinBox_serverPort->value();
    if (!server.listen(addr,port)) {
            QMessageBox::critical(this, tr("服务器启动失败"),
                                  tr("无法启动服务器: %1.")
                                  .arg(server.errorString()));
            close();
            return false;
        }
    ui->text_output->append("服务器启动成功");


    if(FENCI_select == 0)
    {
       KS_ImportUserDict("keylist.txt",true);
    }
    else
    {

    }
    return true;
}


void MainWindow::on_action_clearinfotext_triggered()
{
    ui->text_output->clear();
}


void MainWindow::on_action_start_triggered()
{
    _IniLog();
    _InitServer();

    ui->action_start->setDisabled(true);
    ui->action_stopServer->setEnabled(true);

  //PAGE 1:
    ui->comboBox_serverIP->setDisabled(true);
    ui->spinBox_serverPort->setDisabled(true);
    ui->checkBox_FENCI_isopen->setDisabled(true);
    ui->horizontalSlider_FENCI_select->setDisabled(true);
    ui->radioButton_refuseHTML->setDisabled(true);
    ui->radioButton_onlyLOG->setDisabled(true);

   //PAGE 2:
    ui->pushButton_KS_addLine->setDisabled(true);
    ui->pushButton_KS_delLine->setDisabled(true);
    ui->pushButton_KS_inLine->setDisabled(true);
    ui->pushButton_KS_save->setDisabled(true);
    ui->pushButton_KS_load->setDisabled(true);
    ui->checkBox_KS_Details->setDisabled(true);
    ui->checkBox_KS_savehistory->setDisabled(true);
    ui->checkBox_KS_Details->setDisabled(true);
    ui->checkBox_KS_savehistory->setDisabled(true);
    ui->radioButton_KSMODE_0->setDisabled(true);
    ui->radioButton_KSMODE_1->setDisabled(true);
    ui->radioButton_KSMODE_2->setDisabled(true);
    ui->radioButton_KSMODE_3->setDisabled(true);


    //PAGE3:
    ui->pushButton_NLPIR_import->setDisabled(true);
    ui->pushButton_clearNLPIRlog->setDisabled(true);
    ui->pushButton_openNLPIRlog->setDisabled(true);
}

void MainWindow::ChangeUIOptions(int select, QByteArray cmd)
{

}

void MainWindow::on_action_stopServer_triggered()
{
    _StopServer();

    INTCON = 0;

    ui->action_start->setEnabled(true);
    ui->action_stopServer->setDisabled(true);

    //PAGE 1:
    ui->comboBox_serverIP->setEnabled(true);
    ui->spinBox_serverPort->setEnabled(true);
    ui->checkBox_FENCI_isopen->setEnabled(true);
    ui->horizontalSlider_FENCI_select->setEnabled(true);
    ui->radioButton_refuseHTML->setEnabled(true);
    ui->radioButton_onlyLOG->setEnabled(true);

    //PAGE2：
    ui->pushButton_KS_addLine->setDisabled(true);
    ui->pushButton_KS_delLine->setDisabled(true);
    ui->pushButton_KS_inLine->setDisabled(true);
    ui->pushButton_KS_save->setDisabled(true);
    ui->pushButton_KS_load->setEnabled(true);
    ui->checkBox_KS_Details->setEnabled(true);
    ui->checkBox_KS_savehistory->setEnabled(true);
    ui->radioButton_KSMODE_0->setEnabled(true);
    ui->radioButton_KSMODE_1->setEnabled(true);
    ui->radioButton_KSMODE_2->setEnabled(true);
    ui->radioButton_KSMODE_3->setEnabled(true);


    //PAGE3:
    ui->pushButton_NLPIR_import->setEnabled(true);
    ui->pushButton_clearNLPIRlog->setEnabled(true);
    ui->pushButton_openNLPIRlog->setEnabled(true);

    _CloseLog();
}

void MainWindow::_StopServer()
{
    if(server.isListening())
    {
        server.close();
        server.closeAllThread();
    }
    else
    {

    }
    ui->text_output->append("服务器关闭");
}

void MainWindow::on_horizontalSlider_FENCI_select_sliderMoved(int position)
{
    if(position < 50)
    {
        ui->horizontalSlider_FENCI_select->setValue(0);
    }
    else
    {
        ui->horizontalSlider_FENCI_select->setValue(99);
    }
}


void MainWindow::on_horizontalSlider_FENCI_select_valueChanged(int value)
{
    if(value < 50)
    {
        FENCI_select =0;
        perFENCI->setText("快速过滤模式");
    }
    else
    {
        FENCI_select =1;
        perFENCI->setText("精细分词模式");
    }
}


void MainWindow::on_checkBox_FENCI_isopen_stateChanged(int arg1)
{
    if(arg1 == Qt::Checked)
    {
        FENCI_open = true;
    }
    else
    {
        FENCI_open = false;
    }
}

void MainWindow::iniModel()
{
    theModel = new QStandardItemModel(2,3,this); //数据模型
    theSelection = new QItemSelectionModel(theModel);//Item选择模型
    ui->tableView_KS->setModel(theModel); //设置数据模型
    ui->tableView_KS->setSelectionModel(theSelection);//设置选择模型
    ui->tableView_KS->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->tableView_KS->setSelectionBehavior(QAbstractItemView::SelectItems);

    QFile aFile("keylist.txt");
    /*    QString curPath=QCoreApplication::applicationDirPath(); //获取应用程序的路径
//调用打开文件对话框打开一个文件
    QString aFileName=QFileDialog::getOpenFileName(this,"打开一个文件",curPath,
                 "井数据文件(*.txt);;所有文件(*.*)");
    if (aFileName.isEmpty()).....
     QFile aFile(aFileName);  //以文件方式读出
    */
    QStringList fFileContent;//文件内容字符串列表
   if (aFile.open(QIODevice::ReadOnly | QIODevice::Text))
   {
       QTextStream aStream(&aFile); //用文本流读取文件
       while (!aStream.atEnd())
       {
           QString str=aStream.readLine();//读取文件的一行
           fFileContent.append(str); //添加到 StringList
       }
       QStringList aFileContent = fFileContent;
       aFile.close();//关闭文件
       int rowCnt=aFileContent.count(); //文本行数，第1行是标题
       theModel->setRowCount(rowCnt); //实际数据行数

   //设置表头
       QString header=aFileContent.at(0);

       QStringList headerList;
       headerList.append("词条");
       headerList.append("词类");
       headerList.append("权重");
       theModel->setHorizontalHeaderLabels(headerList);

       int j;
       QStandardItem   *aItem;

       for (int i=0;i<rowCnt;i++)
       {
           QString aLineText=aFileContent.at(i); //获取 数据区 的一行
   //一个或多个空格、TAB等分隔符隔开的字符串， 分解为一个StringList
           QStringList tmpList=aLineText.split(QRegularExpression("\\s+"),Qt::SkipEmptyParts);
           for (j=0;j<3;j++)
           {
               aItem=new QStandardItem(tmpList.at(j));//创建item
               theModel->setItem(i,j,aItem); //为模型的某个行列位置设置Item
           }
        }
    }
   else
   {
       QMessageBox::information(this,"错误","打开词库失败",QMessageBox::Ok,QMessageBox::NoButton);
   }
}
void MainWindow::on_pushButton_KS_load_clicked()
{
        iniModel();
        ui->pushButton_KS_addLine->setEnabled(true);
        ui->pushButton_KS_delLine->setEnabled(true);
        ui->pushButton_KS_inLine->setEnabled(true);
        ui->pushButton_KS_save->setEnabled(true);
        ui->pushButton_KS_load->setDisabled(true);
        ui->action_start->setDisabled(true);

        ui->tabWidgetOPTIONS->setTabEnabled(0,false);
        ui->tabWidgetOPTIONS->setTabEnabled(2,false);
        ui->tabWidgetOPTIONS->setTabEnabled(3,false);
}


void MainWindow::on_pushButton_KS_addLine_clicked()
{
    //在表格最后添加行
        QList<QStandardItem*>    aItemList; //容器类
        QStandardItem   *aItem;
        for(int i=0;i<3;i++)
        {
            aItem=new QStandardItem(""); //创建Item
            aItemList<<aItem;   //添加到容器
        }

        theModel->insertRow(theModel->rowCount(),aItemList); //插入一行，需要每个Cell的Item
        QModelIndex curIndex=theModel->index(theModel->rowCount()-1,0);//创建最后一行的ModelIndex
        theSelection->clearSelection();//清空选择项
        theSelection->setCurrentIndex(curIndex,QItemSelectionModel::Select);//设置刚插入的行为当前选择行
}


void MainWindow::on_pushButton_KS_delLine_clicked()
{
    //删除行
        QModelIndex curIndex=theSelection->currentIndex();//获取当前选择单元格的模型索引

        if (curIndex.row()==theModel->rowCount()-1)//最后一行
            theModel->removeRow(curIndex.row()); //删除最后一行
        else
        {
            theModel->removeRow(curIndex.row());//删除一行，并重新设置当前选择行
            theSelection->setCurrentIndex(curIndex,QItemSelectionModel::Select);
        }
}


void MainWindow::on_pushButton_KS_inLine_clicked()
{
    //插入行
        QList<QStandardItem*>    aItemList;  //QStandardItem的列表类
        QStandardItem   *aItem;
        for(int i=0;i<3;i++)
        {
            aItem=new QStandardItem(""); //新建一个QStandardItem
            aItemList<<aItem;//添加到列表类
        }

        QModelIndex curIndex=theSelection->currentIndex(); //获取当前选中项的模型索引
        theModel->insertRow(curIndex.row(),aItemList);  //在当前行的前面插入一行
        theSelection->clearSelection();//清除已有选择
        theSelection->setCurrentIndex(curIndex,QItemSelectionModel::Select);
}


void MainWindow::on_pushButton_KS_save_clicked()
{
    QFile aFile("keylist.txt");
    if (!(aFile.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate)))
    {
    }
    QTextStream aStream(&aFile); //用文本流读取文件

    QStandardItem   *aItem;
    int i,j;
    QString str;

//获取数据区文字
    for ( i=0;i<theModel->rowCount();i++)
    {
        str="";
        for( j=0;j<theModel->columnCount();j++)
        {
            aItem=theModel->item(i,j);
            str=str+aItem->text()+QString::asprintf(" ");
        }
         aStream<<str<<"\n";
    }
    aFile.close();
    QMessageBox::information(this,"提示","词典保存成功",QMessageBox::Ok,QMessageBox::NoButton);


    theModel->clear();

    ui->pushButton_KS_addLine->setDisabled(true);
    ui->pushButton_KS_delLine->setDisabled(true);
    ui->pushButton_KS_inLine->setDisabled(true);
    ui->pushButton_KS_save->setDisabled(true);
    ui->pushButton_KS_load->setEnabled(true);

    ui->action_start->setEnabled(true);

    ui->tabWidgetOPTIONS->setTabEnabled(0,true);
    ui->tabWidgetOPTIONS->setTabEnabled(2,true);
    ui->tabWidgetOPTIONS->setTabEnabled(3,true);
}


void MainWindow::on_checkBox_KS_Details_stateChanged(int arg1)
{
    if(arg1 == Qt::Checked)
        KS_OUTDetails = true;
    else
        KS_OUTDetails = false;
}


void MainWindow::on_radioButton_refuseHTML_clicked(bool checked)
{
    if(checked){
        ui->radioButton_onlyLOG->setChecked(false);
        REFUSEHTML = true;
    }
}


void MainWindow::on_radioButton_onlyLOG_clicked(bool checked)
{
    if(checked){
        ui->radioButton_refuseHTML->setChecked(false);
        REFUSEHTML = false;
    }
}


void MainWindow::on_radioButton_KSMODE_0_clicked(bool checked)
{
    if(checked)
    {
        KS_SCANMODE = 0;
        ui->radioButton_KSMODE_1->setChecked(false);
        ui->radioButton_KSMODE_2->setChecked(false);
        ui->radioButton_KSMODE_3->setChecked(false);
    }
}


void MainWindow::on_radioButton_KSMODE_1_clicked(bool checked)
{
    if(checked)
    {
        KS_SCANMODE = 1;
        ui->radioButton_KSMODE_0->setChecked(false);
        ui->radioButton_KSMODE_2->setChecked(false);
        ui->radioButton_KSMODE_3->setChecked(false);
    }
}


void MainWindow::on_radioButton_KSMODE_2_clicked(bool checked)
{
    if(checked)
    {
        KS_SCANMODE = 2;
        ui->radioButton_KSMODE_0->setChecked(false);
        ui->radioButton_KSMODE_1->setChecked(false);
        ui->radioButton_KSMODE_3->setChecked(false);
    }
}


void MainWindow::on_radioButton_KSMODE_3_clicked(bool checked)
{
    if(checked)
    {
        KS_SCANMODE = 3;
        ui->radioButton_KSMODE_0->setChecked(false);
        ui->radioButton_KSMODE_1->setChecked(false);
        ui->radioButton_KSMODE_2->setChecked(false);
    }
}


void MainWindow::on_checkBox_KS_savehistory_stateChanged(int arg1)
{
   if(arg1 == Qt::Checked)
   {
       LOG_OPEN = true;
       logShow->setText("日志正在写入文本文件");
   }
   else
   {
       LOG_OPEN = false;
       logShow->setText("");
   }
}

void MainWindow::_IniLog()
{
    if (!LOGfile.isOpen() && !LOGfile.open(QIODevice::Append | QIODevice::Text) )
    {
        ui->text_output->append("KS日志文件无法写入");
        ui->checkBox_KS_savehistory->setCheckable(false);
    }
    else
    {
         qDebug()<<"KS日志加载成功";
    }
    if(!NLPLog.isOpen() &&  !NLPLog.open(QIODevice::Append | QIODevice::Text))
    {
        ui->text_output->append("NIPLR日志文件无法写入");
    }
    else
         qDebug()<<"NIPLR日志加载成功";
}

void MainWindow::_CloseLog()
{
    if(NLPLog.isOpen())
        NLPLog.close();
    if(LOGfile.isOpen())
        LOGfile.close();
}

void MainWindow::on_pushButton_NLPIR_import_clicked()
{
    QString curPath = QDir::currentPath();
    QString dlgTitle = "打开一个文件";
    QString filter = "文本文件(*.txt);;所有文件(*.*)";
    QString aFileName = QFileDialog::getOpenFileName(this,dlgTitle,curPath,filter);
    if(aFileName.isEmpty())
        return;
    NLPIR_ImportUserDict(aFileName.toUtf8(),false);
    QMessageBox::information(this,"提示","词典导入成功",QMessageBox::Ok,QMessageBox::NoButton);
}


void MainWindow::on_action_reboot_triggered()
{
    _StopServer();
    _InitServer();
}


void MainWindow::on_pushButton_clearNLPIRlog_clicked()
{
    if(NLPLog.isOpen())
        NLPLog.close();
    NLPLog.open(QIODevice::WriteOnly | QIODevice::Text);
    QMessageBox::information(this,"提示","日志已经清空",QMessageBox::Ok,QMessageBox::NoButton);
    NLPLog.close();
}


void MainWindow::on_pushButton_openNLPIRlog_clicked()
{
     if(NLPLog.isOpen())
         NLPLog.close();
    QDesktopServices::openUrl(QUrl("NLPIRlog.txt"));
}


void MainWindow::on_pushButton_NLPIR_help_clicked()
{
    QMessageBox::information(this,"帮助","精细分词系统具有强大分词功能，由于结果数据量大，只采用文本日志输出",QMessageBox::Ok,QMessageBox::NoButton);
}


void MainWindow::on_action_mini_triggered()
{
    this->showMinimized();
}


void MainWindow::on_action_about_triggered()
{
    QMessageBox::information(this,"关于","毕业设计",QMessageBox::Ok,QMessageBox::NoButton);
}


