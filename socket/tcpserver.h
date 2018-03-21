#ifndef TCPSERVER_H
#define TCPSERVER_H
#include <QByteArray>
#include <QtNetwork>
#include <QMessageBox>
#include <QMainWindow>
#include <QString>
#include <QStringList>
#include "my.h"


class TcpServer: public QObject
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = 0);
    ~TcpServer();
    bool sendData(char *data, int length);
    bool isArm_connect();
    bool sendPackage(char *data, int length, char *head);

private:
    void getBuff(char* buff, int length);  //抓包函数 wifi定位用
    char getpont(char* buff, int length,                    //或得包头包尾
                          int *pont_head, int *pont_head_s,
                          int *pont_tial, int *pont_tial_s);
    void add_msg(char* buff, int length);//将接收到的信息，加入缓存区
    void del_msg(int tail);
    int max(int buff[], int length);
    int max(int a, int b);
    int sum(int buff[], int length);
    void need_sen_buf_init();
    void opt_data(char* socket_snd_buf, int length);
    void splitData(QString str);
    bool phone_optData(double *level, int length);
    bool isPhone_zero(double *level, int length);
    bool isPhone_equal(double *level, double *level_last,
                       int length);
    int package(char *data, int len, char *head,
                             char cmd_snd[], int *length);


    QTcpServer *server;
    QTcpSocket *clientConnection;
    QTcpSocket* client1;
    QTcpSocket* client2;
    bool isIP1connect,isIP2connect;
    char recv_info[RECV_BUFF_SIZE];
    int recv_end;
    quint32 ip1,ip2;
    struct send_info need_sen_buf[NEEDED_ESSID_COUNT];
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
    arm_DataReceived(int* data, int length);//接收到数据并处理之后发送信号
    arm_DataReceived(double* data, int length);//接收到数据并处理之后发送信号
    arm_DataReceived_cmd(char* data, int length);
    phone_DataReceived(double* data, int length);//接收到数据并处理之后发送信号
    arm_netWorkerror();
    arm_network_connect();

private slots:
    void acceptConnection();
    void readClient();
    void displayError(QAbstractSocket::SocketError);  //当前网络连接出现错误时 显示 操作
    void client1_displayError(QAbstractSocket::SocketError);
    void client2_displayError(QAbstractSocket::SocketError);

protected:

};

#endif // TCPSERVER_H
