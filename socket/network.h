#ifndef NETWORK_H
#define NETWORK_H

#include <QByteArray>
#include <QtNetwork>
#include <QMessageBox>
#include <QMainWindow>
#include "my.h"

class Network: public QObject
{
    Q_OBJECT
public:
    explicit Network(QObject *parent = 0);
    bool Connect();  //连接到网络
    void DisConnect();  //断开网络连接
    bool SendData( QByteArray data );  //发送数据 （要发送的数据存在 “data” 里）
    bool isNetworkOn();  //网络连接状态 （“已连接” 、 “未连接”）
    void getMainWindow(QMainWindow* window);  //获取主窗口指针

private:
    void GetData(QByteArray data);//抓包函数
    void getData(QByteArray data);  //抓包函数  //现用
    void getBuff(char* buff, int length);  //抓包函数 wifi定位用
    char getpont(char* buff, int length,                    //或得包头包尾
                          int *pont_head, int *pont_head_s,
                          int *pont_tial, int *pont_tial_s);
    void add_msg(char* buff, int length);//将接收到的信息，加入缓存区
    void del_msg(int tail);
    int max(int a, int b);

    QString HostName;   //主机名称
    quint16 TcpPort;    //端口号
    QTcpSocket* tcpSocket;  //
    bool isNetworkon;    //当前网络状态
    QMainWindow* mainwindow;
    QByteArray buff;  //抓包之后的数据缓存

    struct send_info sen_buf[ESSID_MAX_COUNT];
    struct socket_info socket_info;
    char* recv_info;
    int recv_end;
    int count,i;

signals:
    DataReceived(QByteArray data);//接收到数据并处理之后发送信号
    c_DataReceived(char* data, int length);//接收到数据并处理之后发送信号
    DataEmpty();//发送的数据为空时 发送信号
    netWorkerror();   //当前网络连接出现错误时发出信号

private slots:
    void ReadMessage();//数据接收槽函数
    void NetworkDataUpDate(QString* hostname , quint16* port);  //网络连接相关参数发生改变
    void displayError(QAbstractSocket::SocketError);  //当前网络连接出现错误时 显示 操作

protected:

};




#endif
