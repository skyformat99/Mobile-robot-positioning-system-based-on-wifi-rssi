#include "network.h"

Network::Network(QObject *parent):
    QObject(parent)
{
    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(ReadMessage()));
    connect(tcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),
            this,SLOT(displayError(QAbstractSocket::SocketError)));
    HostName = "192.168.0.145";
    TcpPort = 8080;
    isNetworkon = false;
    recv_end = 0;
    recv_info = new char[RECV_BUFF_SIZE];
    memset(recv_info, 0, RECV_BUFF_SIZE);
}

int Network::max(int a, int b)
{
    if(a > b)
        return a;
    else if(a < b)
        return b;
    else
        return a;
}

bool Network::Connect()
{
    tcpSocket->connectToHost( HostName , TcpPort );
    if(tcpSocket->waitForConnected(100))
    {
        isNetworkon = true;
        buff.clear();
        return true;
    }
    else
    {
        isNetworkon = false;
        buff.clear();
        return false;
    }
}

void Network::DisConnect()
{
    tcpSocket->abort();
    buff.clear();
    isNetworkon = false;
}

void Network::ReadMessage()
{
    QByteArray tep= tcpSocket->readAll();
    int length = tep.length();
    char* socket_snd_buf = new char[length];
    socket_snd_buf = tep.data();
    add_msg(socket_snd_buf, length);

    getBuff(recv_info, recv_end);
    //emit c_DataReceived(socket_snd_buf,length);
    delete [] socket_snd_buf;
}

void Network::add_msg(char* buff, int length)
{
    if(recv_end + length > RECV_BUFF_SIZE)
    {
        qDebug("**************************缓冲区已满");
        return;
    }
    memcpy(recv_info + recv_end, buff, length);
    recv_end += length;
}

void Network::del_msg(int tail)
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

char Network::getpont(char* buff, int length,
                      int* pont_head, int* pont_head_s,
                      int* pont_tial, int* pont_tial_s)
{
    int i = 0;
    *pont_head = -1;
    *pont_head_s = -1;
    *pont_tial = -1;
    *pont_tial_s = -1;
    for(i = 0; i < length-3; i++)
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
    if(*pont_head > -1 || *pont_head_s >-1 || *pont_tial > -1 || *pont_tial_s > -1)
        return 1;
    else
        return 0;
}

void Network::getBuff(char* buff, int length)
{
    int p_head, p_head2, p_tail, p_tail2;
    int leng=0;
    qDebug("data length:%d",length);
    char ret = getpont(buff, length, &p_head, &p_head2, &p_tail, &p_tail2);
    if(ret == 1)
    {
        if(p_tail > p_head && p_head > -1) //正好包含一组数据 并且数据非空
        {
            leng = p_tail - p_head - 4;
            if(leng > 0)
            {
                char* socket_snd_buf = new char[leng];
                socket_snd_buf = buff + p_head + 4;
                emit c_DataReceived(socket_snd_buf,leng);
                delete [] socket_snd_buf;
            }
            else
                qDebug("data is empty!\n");
        }
        if(p_tail2 > p_head2 && p_head2 > -1) //正好包含一组数据 并且数据非空
        {
            leng = p_tail2 - p_head2 -4;
            if(leng > 0)
            {
                char* socket_snd_buf = new char[leng];
                socket_snd_buf = buff + p_head2 + 4;
                emit c_DataReceived(socket_snd_buf,leng);
                delete [] socket_snd_buf;
            }
            else
                qDebug("data is empty!\n");
        }
        qDebug("there is a p_x come in %d %d %d %d",
               p_head, p_tail, p_head2, p_tail2);
        if(p_tail2 > -1 && p_tail > -1)
            del_msg(p_tail2 + 4);
        else if(p_tail > -1 && p_tail2 == -1)
            del_msg(p_tail + 4);
        else if(p_tail == -1 && p_tail2 == -1 && p_head > -1 && p_head2 > -1)
            del_msg(max(p_head, p_head2));
    }
    else
        qDebug("no data!\n");
}

void Network::getData(QByteArray data)
{
    char i=0,k=0,m=0;
    static char j=0,n=0;
    unsigned char* s = (unsigned char*)data.data();
    qDebug("%d ",data.length());
    if(0xff == *s && j == 0)
    {
        for(i=0;i<data.length();i++)
        {
            if(0xff != *(s+i))
                buff[k++] =*(s+i);
            else if(0xff == *(s+i) && k != 0)
            {
                qDebug("emit !");
                emit DataReceived(buff);
                buff.clear();
                k=0;
                m++;
                if(m == 2)
                {
                    m=0;
                    break;
                }
            }
            if(0xff != *(s+data.length()-1))
            {
                j=1;
                n=k;
            }
            else
            {
                j=0;
                n=0;
            }
        }
    }
    if(j==1 && 0xff != *s)
    {
        for(i=0;i<data.length();i++)
        {
            if(0xff != *(s+i))
            buff[n++]= *(s+i);
            else
            {
                j=0;
                n=0;
                emit DataReceived(buff);
                buff.clear();
                qDebug("emit2 !");
                break;
            }
        }
    }
}

void Network::GetData(QByteArray data)
{
    char i=0,k=0;
    static char j=0,n=0;
    unsigned char* s = (unsigned char*)data.data();
    static QByteArray buff;
    if(0xff == *s && j == 0)
    {
        for(i=0;i<=data.length();i++)
        {
            if(0xff != *(s+i))
            buff[k++] =*(s+i);
            else
            {
                j++;
                if(j==4)
                {
                    j=0;
                    emit DataReceived(buff);
                    break;
                }
                else
                {
                    n=k+1;
                }
            }

        }
    }
    if(j!=0 && 0xff!=*s)
    {
        for(i=0;i<=data.length();i++)
        {
            if(0xff != *(s+i))
            buff[n++]= *(s+i);
            else
            {
                j=0;
                n=0;
                emit DataReceived(buff);
                break;
            }
        }
    }
}

bool Network::SendData(QByteArray data)
{
    char* a;
    a=data.data();
    if(data.length()>0)
    {
        tcpSocket->write(a,data.length());
        if(tcpSocket->waitForBytesWritten())
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
        emit DataEmpty();  //如果数据为空，则发出信号
        return false;
    }
}

void Network::NetworkDataUpDate(QString* hostname , quint16* port)
{
    HostName = *hostname;
    TcpPort = *port;
    qDebug("NetworkDataUpDate");
  //  qDebug("%s %d",QByteArray(HostName.toLatin1()).toHex() , TcpPort);
}

bool Network::isNetworkOn()
{
    return isNetworkon;
}

void Network::getMainWindow(QMainWindow* window)
{
    mainwindow = window;
}

void Network::displayError(QAbstractSocket::SocketError)
{
    qDebug() << tcpSocket->errorString(); //输出错误信息
    isNetworkon = false;
    buff.clear();
    QMessageBox::information(mainwindow,
                             tr("warning"),
                             tr("An unexpected network error occurred !"));
    emit netWorkerror();
}
