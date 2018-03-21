#include "tcpserver.h"
#include <QStringList>

TcpServer::TcpServer(QObject *parent):
    QObject(parent)
{

    clientConnection = new QTcpSocket(this);
    client1 = new QTcpSocket(this);  //arm端
    client2 = new QTcpSocket(this);  //手机端


    server = new QTcpServer();
    server->listen(QHostAddress::Any, 8080);
    connect(server, SIGNAL(newConnection()), this, SLOT(acceptConnection()));

    connect(server,SIGNAL(acceptError(QAbstractSocket::SocketError)),
            this,SLOT(displayError(QAbstractSocket::SocketError)));

    isIP1connect = false;
    isIP2connect = false;

    ip1 = QHostAddress("192.168.0.103").toIPv4Address();
    ip2 = QHostAddress("192.168.0.102").toIPv4Address();

    recv_end = 0;
    memset(recv_info, 0, RECV_BUFF_SIZE);

    need_sen_buf_init();

}

TcpServer::~TcpServer()
{
    delete server;
    delete clientConnection;
    delete client1;
    delete client2;
}

int TcpServer::max(int buff[], int length)
{
    int i=0,max=0;
    max = buff[0];
    for(i=1; i<length; i++)
    {
        if(buff[i] > max)
            max = buff[i];
    }
    return max;
}

int TcpServer::max(int a, int b)
{
    if(a > b)
        return a;
    else if(a < b)
        return b;
    else
        return a;
}

int TcpServer::sum(int buff[], int length)
{
    int sum,i;
    sum = buff[0];
    for(i=1; i<length; i++)
    {
        sum += buff[i];
    }
    return sum;
}

void TcpServer::acceptConnection()
{
    qDebug("new connect");
    clientConnection = server->nextPendingConnection();

    if(ip1 == clientConnection->peerAddress().toIPv4Address())
    {
        client1 = clientConnection;
        isIP1connect = true;
        connect(client1, SIGNAL(readyRead()), this, SLOT(readClient()));
        connect(client1,SIGNAL(error(QAbstractSocket::SocketError)),
                this,SLOT(client1_displayError(QAbstractSocket::SocketError)));
        connect(client1,SIGNAL(disconnected()),
                        client1,SLOT(deleteLater()));
        emit arm_network_connect();
    }
    if(ip2 == clientConnection->peerAddress().toIPv4Address())
    {
        client2 = clientConnection;
        isIP2connect = true;
        connect(client2, SIGNAL(readyRead()), this, SLOT(readClient()));
        connect(client2,SIGNAL(error(QAbstractSocket::SocketError)),
                this,SLOT(client2_displayError(QAbstractSocket::SocketError)));
        connect(client2,SIGNAL(disconnected()),
                        client2,SLOT(deleteLater()));
    }
}

void TcpServer::readClient()
{
    /* ARm 端数据接收 */
    if(isIP1connect == true)
    if(ip1 == client1->peerAddress().toIPv4Address())
    {
        QByteArray tep = client1->readAll();
#ifdef DEBUG_SOCKET
        qDebug("recv_msg length:%d",tep.length());
#endif
        int length = tep.length();
        char* socket_snd_buf = tep.data();
        add_msg(socket_snd_buf, length);
        getBuff(recv_info, recv_end);
    }

    /* 手机 端数据接收 */
    if(isIP2connect == true)
    if(ip2 == client2->peerAddress().toIPv4Address())
    {
#ifdef DEBUG_SOCKET
        qDebug("message from: 192.168.0.136");
#endif
        QString str = client2->readAll();

#ifdef DEBUG_SOCKET
        qDebug()<<"data :"<<str.toUtf8();
#endif
        splitData(str);
    }
}

/* 开发板 端数据处理 */
void TcpServer::add_msg(char* buff, int length)
{
    if(recv_end + length > RECV_BUFF_SIZE)
    {
        qDebug("**************************缓冲区已满");
        return;
    }
    else
    {
        memcpy(recv_info + recv_end, buff, length);
        recv_end += length;
    }
}

void TcpServer::del_msg(int tail)
{
    if(tail == recv_end)
    {
        memset(recv_info, 0, RECV_BUFF_SIZE);
        recv_end = 0;
    }
    else if(recv_end > tail)
    {
        memcpy(recv_info, recv_info+tail, recv_end-tail);
        recv_end = recv_end - tail + 1;
    }
    else
        qDebug("***************************del_msg:发生了非常奇怪的事情");
}

char TcpServer::getpont(char* buff, int length,
                      int* pont_head, int* pont_head_s,
                      int* pont_tial, int* pont_tial_s)
{
    int i = 0;
    *pont_head = -1;
    *pont_head_s = -1;
    *pont_tial = -1;
    *pont_tial_s = -1;
    for(i = 0; i < length-3; i++)   //小于 也就是要判断到length-4个数据。
    {
        if(*(buff+i) == 26 && *(buff+i+1) == 42
                && *(buff+i+2) == 58 && *(buff+i+3) == 74)
        {
            if(*pont_head == -1)
                *pont_head = i;
            else if(*pont_head != -1 && *pont_head_s == -1)
                *pont_head_s = i;
            else
                qDebug("***********************************注意： 超过三个数据头！");
        }
        if(*(buff+i) == 0x1f && *(buff+i+1) == 0x2f
                && *(buff+i+2) == 0x3f && *(buff+i+3) == 0x4f)
        {
            if(*pont_tial == -1)
                *pont_tial = i;
            else if(*pont_tial != -1 && *pont_tial_s == -1)
                *pont_tial_s = i;
            else
                qDebug("***********************************注意： 超过三个数据尾！");
        }
    }
#ifdef DEBUG_SOCKET
    if(*pont_head > -1 && *pont_head == 0)
        qDebug("pont_head: \n%d %d %d %d\n%d %d %d %d",*(buff+*pont_head),*(buff+*pont_head+1),
               *(buff+*pont_head+2),*(buff+*pont_head+3),*(buff+*pont_head+4),
               *(buff+*pont_head+5),*(buff+*pont_head+6),*(buff+*pont_head+7));

    if(*pont_tial > -1 && *pont_tial > 0)
        qDebug("pont_tial: \n%d %d %d %d\n%d %d %d %d\n%d %d %d %d"
               ,*(buff+*pont_tial-4),*(buff+*pont_tial-3),*(buff+*pont_tial-2),*(buff+*pont_tial-1),
               *(buff+*pont_tial),*(buff+*pont_tial+1),*(buff+*pont_tial+2),*(buff+*pont_tial+3),
               *(buff+*pont_tial+4),*(buff+*pont_tial+5),*(buff+*pont_tial+6),*(buff+*pont_tial+7));

    if(*pont_head_s > -1 && *pont_head_s > 0)
     qDebug("pont_head_s: \n%d %d %d %d\n%d %d %d %d\n%d %d %d %d"
            ,*(buff+*pont_head_s-4),*(buff+*pont_head_s-3),*(buff+*pont_head_s-2),*(buff+*pont_head_s-1),
            *(buff+*pont_head_s),*(buff+*pont_head_s+1),*(buff+*pont_head_s+2),*(buff+*pont_head_s+3),
            *(buff+*pont_head_s+4),*(buff+*pont_head_s+5),*(buff+*pont_head_s+6),*(buff+*pont_head_s+7));

    if(*pont_tial_s > -1 && *pont_tial_s > 0)
      qDebug("pont_tial_s: \n%d %d %d %d\n%d %d %d %d\n%d %d %d %d"
             ,*(buff+*pont_tial_s-4),*(buff+*pont_tial_s-3),*(buff+*pont_tial_s-2),*(buff+*pont_tial_s-1),
             *(buff+*pont_tial_s),*(buff+*pont_tial_s+1),*(buff+*pont_tial_s+2),*(buff+*pont_tial_s+3),
             *(buff+*pont_tial_s+4),*(buff+*pont_tial_s+5),*(buff+*pont_tial_s+6),*(buff+*pont_tial_s+7));
#endif

    if(*pont_head > -1 || *pont_head_s >-1 || *pont_tial > -1 || *pont_tial_s > -1)
        return 1;
    else
        return 0;
}

void TcpServer::getBuff(char* buff, int length)
{
    int p_head, p_head2, p_tail, p_tail2;
    int leng=0;
#ifdef DEBUG_SOCKET
    qDebug("data length:%d",length);
#endif
    char ret = getpont(buff, length, &p_head, &p_head2, &p_tail, &p_tail2);
    if(ret == 1)
    {
        if(p_tail > p_head && p_head > -1) //正好包含一组数据 并且数据非空
        {
            leng = p_tail - p_head - 4;
            if(leng > 0)
            {
                char* socket_snd_buf = buff + p_head + 4;
                opt_data(socket_snd_buf, leng);
               // emit c_DataReceived(socket_snd_buf,leng);
            }
            else
                qDebug("data is empty!\n");
        }
        if(p_tail2 > p_head2 && p_head2 > -1) //正好包含一组数据 并且数据非空
        {
            leng = p_tail2 - p_head2 -4;
            if(leng > 0)
            {
                char* socket_snd_buf = buff + p_head2 + 4;
                opt_data(socket_snd_buf, leng);
               // emit c_DataReceived(socket_snd_buf,leng);
            }
            else
                qDebug("data is empty!\n");
        }
#ifdef DEBUG_SOCKET
        qDebug("there is a p_x come in %d %d %d %d",
               p_head, p_tail, p_head2, p_tail2);
#endif
        if(p_tail2 > -1 && p_tail > -1)
            del_msg(p_tail2 + 4);
        else if(p_tail > -1 && p_tail2 == -1)
            del_msg(p_tail + 4);
        else if(p_tail == -1 && p_tail2 == -1 && p_head > -1 && p_head2 > -1)
            del_msg(max(p_head, p_head2));
    }
    else
    {
        qDebug("no data!\n");
    }
}


void TcpServer::opt_data(char* socket_snd_buf, int length)
{
    double level[10] = {0,0};
    int levelint[8];
    int count, i=0, j=0;
    length = length; //防止报参数未用警告
    char* socket_snd_buff = socket_snd_buf;
    struct socket_info socket_info;
    struct socket_cmd_info cmd_info;
    struct send_info sen_buf[ESSID_MAX_COUNT];

    memset(&socket_info, 0, sizeof(socket_info));
    if(socket_snd_buff != NULL && socket_snd_buff != 0)
    {
        memcpy(&socket_info, socket_snd_buff, sizeof(socket_info));
        memcpy(&cmd_info, socket_snd_buff, sizeof(cmd_info));
    }

    if(strcmp(socket_info.head, "essid") == 0
            && *(&socket_info.buff) != NULL && &socket_info.count != NULL
            && *(&socket_info.buff) != 0 && &socket_info.count != 0)
    {
        count = socket_info.count;
        memcpy(&sen_buf, socket_info.buff, sizeof(socket_info.buff));
        if(count > ESSID_MAX_COUNT) count =0;
        for(i = 0; i < count; i++)
        {
            for(j=0; j<NEEDED_ESSID_COUNT; j++)
            {
                if(strcmp(sen_buf[i].data, need_sen_buf[j].data) == 0)
                    need_sen_buf[j].level = sen_buf[i].level;
            }
        }
        for(i=0; i<NEEDED_ESSID_COUNT; i++)
        {
            level[i+2] = need_sen_buf[i].level; //传入matlab引擎
            levelint[i] = need_sen_buf[i].level; //用于数据采集、数据显示

#ifdef DEBUG_SOCKET
            qDebug("data[%d].data = %s, data[%d].level %d", i, need_sen_buf[i].data, i, need_sen_buf[i].level);
#endif
            if(need_sen_buf[i].level == 0)
                qDebug("************************need_sen_buf[%d] has no scan result!",i);
        }
        if(phone_optData(level, 10*sizeof(double)))
        {
            emit arm_DataReceived(level, 10*sizeof(double));
            emit arm_DataReceived(levelint, 8*sizeof(int));
        }
    }

    if(strcmp(cmd_info.head, "cmd") == 0
            && *(&cmd_info.buff) != NULL
            && *(&cmd_info.buff) != 0)
    {
        char* data = cmd_info.buff;
        emit arm_DataReceived_cmd(data, sizeof(cmd_info.buff));
    }
}

void TcpServer::need_sen_buf_init()
{
    int i=0;
    for(i=0; i<NEEDED_ESSID_COUNT; i++)
    {
        memcpy(need_sen_buf[i].data, opt_buff[i], sizeof(opt_buff[i]));
        need_sen_buf[i].level = 0;
    }
#ifdef DEBUG_SOCKET
    for(i=0; i<NEEDED_ESSID_COUNT; i++)
    {
        qDebug("data:%s  level: %d",need_sen_buf[i].data, need_sen_buf[i].level);
    }
#endif
}

/* 手机端 数据处理 */
void TcpServer::splitData(QString str)
{
    QStringList strlist = str.split(",");
    if(strlist.size() != 8)
    {
#ifdef DEBUG_ACQ
        qDebug("splitData:接收数据出错，返回");
#endif
        return;
    }
    QString str1 = strlist[0];
    QString str2 = strlist[1];
    QString str3 = strlist[2];
    QString str4 = strlist[3];
    QString str5 = strlist[4];
    QString str6 = strlist[5];
    QString str7 = strlist[6];
    QString str8 = strlist[7];

    double level[10]={0,0};
    bool ok;
    level[2] = str1.toDouble(&ok);
    level[3] = str2.toDouble(&ok);
    level[4] = str3.toDouble(&ok);
    level[5] = str4.toDouble(&ok);
    level[6] = str5.toDouble(&ok);
    level[7] = str6.toDouble(&ok);
    level[8] = str7.toDouble(&ok);
    level[9] = str8.toDouble(&ok);

    if(phone_optData(level, 10*sizeof(double)))
        emit phone_DataReceived(level, 10*sizeof(double));
}

bool TcpServer::phone_optData(double *level, int length)
{
    static double level_last[10];
    static char flag = 0;
    if(flag == 0)
    {
        if(isPhone_zero(level, length))
        {
            memcpy(level_last, level, length);
            flag = 1;
            return true;
        }
        else
            return false;
    }
    else
    {
        if(isPhone_zero(level, length)
               && isPhone_equal(level,level_last,length))
        {
            memcpy(level_last, level, length);
            return true;
        }
        else
            return false;
    }
}

bool TcpServer::isPhone_zero(double *level, int length)
{
    int i=0;
    bool flag = true;
    for(i=2; i<length/(int)sizeof(double); i++)
    {
        if(*(level+i) == 0)
            flag = false;
    }
    return flag;
}

bool TcpServer::isPhone_equal(double *level,
                              double *level_last, int length)
{
    int i=0;
    int flag = 0;
    for(i=2; i<length/(int)sizeof(double); i++)
    {
        if(*(level+i) == *(level_last+i))
            flag++;
    }
    if(flag < length/(int)sizeof(double)-2)
        return true;
    else
        return false;
}

int TcpServer::package(char *data, int len, char *head,
                         char cmd_snd[], int *length)
{
    if(strcmp(head, "uart") == 0)
    {
        struct uart_cmd_info cmd_info;
        char uart_snd_buf[sizeof(cmd_info)];
        if(len > UART_CMD_BUFF_SIZE)
            return -1;
        memset(&cmd_info, 0, sizeof(cmd_info));
          //添加包头
        memcpy(cmd_info.head, "uart", 8);
          //填充数据
        memcpy(cmd_info.buff, data, len);
        memset(uart_snd_buf, 0, sizeof(uart_snd_buf));
        memcpy(uart_snd_buf, &cmd_info, sizeof(cmd_info));
        *length = sizeof(cmd_info);
        memcpy(cmd_snd, uart_snd_buf, *length);
    }
    else if(strcmp(head, "cmd") == 0)
    {
        struct socket_cmd_info cmd_info;
        char socket_snd_buf[sizeof(cmd_info)];
        if(len > SOCKET_CMD_BUFF_SIZE)
            return -1;
        memset(&cmd_info, 0, sizeof(cmd_info));
          //添加包头
        memcpy(cmd_info.head, "cmd", 8);
          //填充数据
        memcpy(cmd_info.buff, data, len);
        memset(socket_snd_buf, 0, sizeof(socket_snd_buf));
        memcpy(socket_snd_buf, &cmd_info, sizeof(cmd_info));
        *length = sizeof(cmd_info);
        memcpy(cmd_snd, socket_snd_buf, *length);
    }
    return 0;
}

bool TcpServer::sendPackage(char *data, int length, char *head)
{
    char send_snd[SOCKET_CMD_BUFF_SIZE];
    int len;
    int ret = package(data, length, head, (char*)send_snd, (int*)&len);
    if(ret == 0)
    {
        if(sendData((char*)send_snd, len))
            return true;
        else
            return false;
    }
    else
        return false;
}

bool TcpServer::sendData(char *data, int length)
{
    if(length>0)
    {
        client1->write(data, length);
        if(client1->waitForBytesWritten())
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
       // emit DataEmpty();  //如果数据为空，则发出信号
        return false;
    }
}

bool TcpServer::isArm_connect()
{
    return isIP1connect;
}

void TcpServer::displayError(QAbstractSocket::SocketError)
{
    qDebug("server error");
    qDebug() << server->errorString(); //输出错误信息
   // emit netWorkerror();
}

void TcpServer::client1_displayError(QAbstractSocket::SocketError)
{
    isIP1connect = false;
    qDebug("client1 error");
    qDebug() << client1->errorString(); //输出错误信息
    client1->close();

    emit arm_netWorkerror();
}

void TcpServer::client2_displayError(QAbstractSocket::SocketError)
{
    isIP2connect = false;
    //client2->close();
   // qDebug("client2 error");
   // qDebug() << client2->errorString(); //输出错误信息

   // emit netWorkerror();
}

