#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "myproxyserver.h"
#include <QMutex>
#include <zlib.h>
#include <QtWidgets>
#include <QFile>
#include <QRegExp>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    myProxyServer server;
    QLabel *perFENCI;
    QLabel *logShow;
    bool _InitKSService();
    bool _InitServer();
    void _InitUI();
    void _IniLog();
    void _CloseLog();

    void _StopServer();

    QStandardItemModel  *theModel;//数据模型
    QItemSelectionModel *theSelection;//Item选择模型

    void  iniModel();//从StringList初始化数据模型


private slots:
    void ChangeUIText(QByteArray cmd);
    void ChangeUIOptions(int select , QByteArray cmd);
    void on_action_clearinfotext_triggered();
    void on_action_start_triggered();
    void on_action_stopServer_triggered();
    void on_horizontalSlider_FENCI_select_sliderMoved(int position);
    void on_horizontalSlider_FENCI_select_valueChanged(int value);
    void on_checkBox_FENCI_isopen_stateChanged(int arg1);


       void on_pushButton_KS_load_clicked();
       void on_pushButton_KS_addLine_clicked();
       void on_pushButton_KS_delLine_clicked();
       void on_pushButton_KS_inLine_clicked();
       void on_pushButton_KS_save_clicked();
       void on_checkBox_KS_Details_stateChanged(int arg1);
       void on_radioButton_refuseHTML_clicked(bool checked);
       void on_radioButton_onlyLOG_clicked(bool checked);
       void on_radioButton_KSMODE_0_clicked(bool checked);
       void on_radioButton_KSMODE_1_clicked(bool checked);
       void on_radioButton_KSMODE_2_clicked(bool checked);
       void on_radioButton_KSMODE_3_clicked(bool checked);
       void on_checkBox_KS_savehistory_stateChanged(int arg1);
       void on_pushButton_NLPIR_import_clicked();
       void on_action_reboot_triggered();
       void on_pushButton_clearNLPIRlog_clicked();
       void on_pushButton_openNLPIRlog_clicked();
       void on_pushButton_NLPIR_help_clicked();
       void on_action_mini_triggered();
       void on_action_about_triggered();

};

extern QMutex LOGmutex;
extern QFile LOGfile;
extern QFile NLPLog;
extern bool LOG_OPEN;
extern bool FENCI_open;
extern int FENCI_select;
extern bool KS_OUTDetails;
extern bool REFUSEHTML;
extern int KS_SCANMODE;
extern int INTCON;
extern int INTFENC;

#endif // MAINWINDOW_H
