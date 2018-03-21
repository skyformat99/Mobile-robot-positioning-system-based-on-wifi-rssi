#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QPen>
#include <QPainterPath>
#include <QEvent>
#include <QKeyEvent>
#include "network.h"
#include "tcpserver.h"
#include "matlablib.h"
#include "mcurvepaint.h"
#include "dataacquisition.h"
#include "my.h"
#include "Kalman.h"
#include <iostream>


using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_bt_connect_clicked();

    void on_bt_disconnect_clicked();

    void arm_recv_msg_show(int* level, int length);
    void arm_recv_msg_cal(double* level, int length);
    void arm_recv_msg_cmd(char *data, int length);
    void phone_recv_msg_cal(double* level, int length);

    void recvMsgfromForm(QString str);

    void arm_net_down();
    void arm_net_connect();

    void on_bt_send_clicked();

    void on_acq_bt_clicked();

    void on_acq_bt_arm_clicked();

    void on_change_bt_clicked();

private:
    Ui::MainWindow *ui;

    bool eventFilter(QObject *obj, QEvent *event);   //事件过滤器

   // void opt_data(char* socket_snd_buf, int length);
    //void need_sen_buf_init();
    void update_lable(int *socket_snd_buf, int length);
    void phone_update_lable(double *socket_snd_buf, int length);
    void keyPressEvent(QKeyEvent  *event);           //按键按下事件
    void keyReleaseEvent(QKeyEvent  *event);         //按键松开事件
    void acq_data(double *level, int length);
    int package(char *data, int len, char *head,
                             char cmd_snd[], int *length);
    char sendtoUart(char *cmd, short speed, short angle);
    int carContorl(bool isk4, bool isk5, bool isk6, bool isk8, bool iskQ);

    mCurvePaint* mPainter;  //曲线绘制类 对象
    Network* network;
    TcpServer* tcpserver;
    Matlablib *engine;
    dataAcquisition * datafile;
    double arm_coor_x,arm_coor_y,phone_coor_x,phone_coor_y;
    bool isPhone_AcquisitionOn,isArm_AcquisitionOn;
    bool isKey_8, isKey_5, isKey_4, isKey_6, isKey_Q;
    KalmanInfo m_Kalmanx, m_Kalmany;

   // struct send_info sen_buf[ESSID_MAX_COUNT];
   // struct socket_info socket_info;
    char opt_buff[8][IW_ESSID_MAX_SIZE+1]={
        "TP-LINK_208_1",
        "TP-LINK_208_2",
        "TP-W",
        "HiWiFi_3D93C2",
        "202",
        "210",
        "TP-LINK_885C",
        "TP-LINK_7FEBEC"
    };


signals:
    sendMsgtoForm(QString str);
    sendMsgtoForm(double *val, int length);
};

#endif // MAINWINDOW_H
