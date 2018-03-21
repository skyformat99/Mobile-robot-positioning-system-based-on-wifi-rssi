#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
/*
        Q:过程噪声，Q增大，动态响应变快，收敛稳定性变坏
        R:测量噪声，R增大，动态响应变慢，收敛稳定性变好
*/
    Init_KalmanInfo(&m_Kalmanx, (double)(1.0/10000.0), (double)(1/100.0));
    Init_KalmanInfo(&m_Kalmany, (double)(1.0/10000.0), (double)(1/100.0));

    isArm_AcquisitionOn = false;
    isPhone_AcquisitionOn = false;
    isKey_4 = false; isKey_5 = false; isKey_6 = false; isKey_8 = false;
    isKey_Q = false;
    mPainter = new mCurvePaint(this);
    mPainter->setTitlecolor(Qt::white);
    mPainter->setAxespen(QPen(Qt::white,1,Qt::SolidLine));
    mPainter->setX_axes(60);
    mPainter->setY_axes(40);
    mPainter->setCurve1(false);
    mPainter->setCurve2(false);
    mPainter->setPointon(true);
    mPainter->setAxes(20,30,ui->paintwidget->width()-10,
                      ui->paintwidget->height()-20);
    mPainter->setRuler(false);
    ui->paintwidget->installEventFilter(this);
    ui->paintwidget->setBackgroundRole(QPalette::Shadow);
    ui->paintwidget->setAutoFillBackground(true);

    datafile = new dataAcquisition(this);

    tcpserver = new TcpServer(this);

    engine = new Matlablib(this);
    engine->getMainWindow(this);
    engine->engineOpen();
    engine->engineInit();
    //ui控件设置

{
    QPalette pa;
    pa.setColor(QPalette::WindowText,Qt::white);
    ui->dBm1_phone->setPalette(pa);
    ui->dBm2_phone->setPalette(pa);
    ui->dBm3_phone->setPalette(pa);
    ui->dBm4_phone->setPalette(pa);
    ui->dBm5_phone->setPalette(pa);
    ui->dBm6_phone->setPalette(pa);
    ui->dBm7_phone->setPalette(pa);
    ui->dBm8_phone->setPalette(pa);

    ui->dBm1->setPalette(pa);
    ui->dBm2->setPalette(pa);
    ui->dBm3->setPalette(pa);
    ui->dBm4->setPalette(pa);
    ui->dBm5->setPalette(pa);
    ui->dBm6->setPalette(pa);
    ui->dBm7->setPalette(pa);
    ui->dBm8->setPalette(pa);

    ui->name_dBm1->setPalette(pa);
    ui->name_dBm2->setPalette(pa);
    ui->name_dBm3->setPalette(pa);
    ui->name_dBm4->setPalette(pa);
    ui->name_dBm5->setPalette(pa);
    ui->name_dBm6->setPalette(pa);
    ui->name_dBm7->setPalette(pa);
    ui->name_dBm8->setPalette(pa);

    ui->arm_coor_x->setPalette(pa);
    ui->arm_coor_y->setPalette(pa);
    ui->arm_coor_z->setPalette(pa);

    ui->phone_coor_x->setPalette(pa);
    ui->phone_coor_y->setPalette(pa);
    ui->phone_coor_z->setPalette(pa);

    ui->name_dBm1->setText(QString(QLatin1String((char*)opt_buff[0])));
    ui->name_dBm2->setText(QString(QLatin1String((char*)opt_buff[1])));
    ui->name_dBm3->setText(QString(QLatin1String((char*)opt_buff[2])));
    ui->name_dBm4->setText(QString(QLatin1String((char*)opt_buff[3])));
    ui->name_dBm5->setText(QString(QLatin1String((char*)opt_buff[4])));
    ui->name_dBm6->setText(QString(QLatin1String((char*)opt_buff[5])));
    ui->name_dBm7->setText(QString(QLatin1String((char*)opt_buff[6])));
    ui->name_dBm8->setText(QString(QLatin1String((char*)opt_buff[7])));
}
   //信号槽关联
    connect(tcpserver,SIGNAL(arm_DataReceived(int *, int)),
            this,SLOT(arm_recv_msg_show(int *, int)));
    connect(tcpserver,SIGNAL(arm_DataReceived(double *, int)),
            this,SLOT(arm_recv_msg_cal(double *, int)));
    connect(tcpserver,SIGNAL(phone_DataReceived(double*, int)),
            this,SLOT(phone_recv_msg_cal(double*, int)));
    connect(tcpserver,SIGNAL(arm_DataReceived_cmd(char*,int)),
            this,SLOT(arm_recv_msg_cmd(char*, int)));
    connect(tcpserver,SIGNAL(arm_netWorkerror()),
            this,SLOT(arm_net_down()));
    connect(tcpserver,SIGNAL(arm_network_connect()),
            this,SLOT(arm_net_connect()));
}

MainWindow::~MainWindow()
{
    engine->engineClose();
    delete tcpserver;
    delete engine;
    delete ui;
}

void MainWindow::phone_recv_msg_cal(double *level, int length)
{

    //更新ap信号强度
    phone_update_lable(level, length);

    //坐标点计算相关，传入matlab引擎
    double x, y;
    engine->engineClaculate((double*)level, length, (double*)&x, (double*)&y);
    ui->phone_coor_x->setNum(x);
    ui->phone_coor_y->setNum(y);
    phone_coor_x = x;
    phone_coor_y = y;
    ui->paintwidget->update();


    //数据采集相关
    if(isPhone_AcquisitionOn)
    {
        acq_data(level, length);
    }
}

void MainWindow::update_lable(int *socket_snd_buf, int length)
{
    length = length;
    ui->dBm1->setNum(*(socket_snd_buf+0));
    ui->dBm2->setNum(*(socket_snd_buf+1));
    ui->dBm3->setNum(*(socket_snd_buf+2));
    ui->dBm4->setNum(*(socket_snd_buf+3));
    ui->dBm5->setNum(*(socket_snd_buf+4));
    ui->dBm6->setNum(*(socket_snd_buf+5));
    ui->dBm7->setNum(*(socket_snd_buf+6));
    ui->dBm8->setNum(*(socket_snd_buf+7));
}

void MainWindow::phone_update_lable(double *socket_snd_buf, int length)
{
    length = length;
    ui->dBm1_phone->setNum((int)*(socket_snd_buf+2));
    ui->dBm2_phone->setNum((int)*(socket_snd_buf+3));
    ui->dBm3_phone->setNum((int)*(socket_snd_buf+4));
    ui->dBm4_phone->setNum((int)*(socket_snd_buf+5));
    ui->dBm5_phone->setNum((int)*(socket_snd_buf+6));
    ui->dBm6_phone->setNum((int)*(socket_snd_buf+7));
    ui->dBm7_phone->setNum((int)*(socket_snd_buf+8));
    ui->dBm8_phone->setNum((int)*(socket_snd_buf+9));
}

void MainWindow::arm_recv_msg_show(int *level, int length)
{
    update_lable(level,length);

}

void MainWindow::arm_recv_msg_cal(double *level, int length)
{
    //坐标点计算相关，传入matlab引擎
    double x, y;
    engine->engineClaculate((double*)level, length, (double*)&x, (double*)&y);
    x=x/100;
    y=y/100;
    qDebug("之前:x=%0.2f  y=%0.2f",x,y);
    phone_coor_x = x;
    phone_coor_y = y;
    //x = KalmanFilter(x, (double)(1.0/10000.0), (double)(1.0/10.0));
    //y = KalmanFilter(y, (double)(1.0/10.0), (double)(1.0/10000.0));

    x = m_KalmanFilter(&m_Kalmanx, x);
    y = m_KalmanFilter(&m_Kalmany, y);
    qDebug("之后：x=%0.2f  y=%0.2f",x,y);
    ui->arm_coor_x->setNum(x);
    ui->arm_coor_y->setNum(y);
    arm_coor_x = x;
    arm_coor_y = y;
    ui->paintwidget->update();

    double val[10];
    memcpy(val, (double*)&(*(level+2)), 8*sizeof(double));
    val[8] = x;
    val[9] = y;
    emit sendMsgtoForm(val,10*sizeof(double));
//数据采集相关
    if(isArm_AcquisitionOn)
    {
        acq_data(level, length);
    }
}

void MainWindow::acq_data(double *level, int length)
{
    bool ok;
    static int count=0;
    static int set=0;
    if(count == 0)
    {
        set = ui->acq_num->text().toInt(&ok,10);
        if(set == 0)
        {
#ifdef DEBUG_ACQ
    qDebug("recv_msg_cal:设定采集次数为零，请重新输入采集次数");
#endif
            isArm_AcquisitionOn = false;
            set = 0;
            count = 0;
            QMessageBox::information(this,
                 tr("warning"),
                 tr("设定采集次数为零，请重新输入采集次数!"));
            return;
        }
        else
        {
            ui->textBr_mess->append("开始保存数据...");
            emit sendMsgtoForm("开始保存数据...");
        }
    }

    *(level+0) = ui->acq_x->text().toDouble(&ok);
    *(level+1) = ui->acq_y->text().toDouble(&ok);
    //传入保存数据
    QString str = ui->acq_filename->text();
    if(str.length() == 0)
    {
#ifdef DEBUG_ACQ
    qDebug("acq_data:保存文件名称为空，请重新输入文件名称");
#endif
    isArm_AcquisitionOn = false;
    set = 0;
    count = 0;
    ui->textBr_mess->append("保存文件名称为空，请重新输入文件名称");
    ui->textBr_mess->append("数据未保存，返回");
    emit sendMsgtoForm("保存文件名称为空，请重新输入文件名称");
    emit sendMsgtoForm("数据未保存，返回");
    QMessageBox::information(this,
         tr("warning"),
         tr("保存文件名称为空，请重新输入文件名称!"));
        return;
    }
    datafile->saveData(level, length, str);
    count++;
    ui->textBr_mess->append("已保存："+QString::number(100*count/set, 10, 4)+"%");
    emit sendMsgtoForm("已保存："+QString::number(100*count/set, 10, 4)+"%");
#ifdef DEBUG_ACQ
    qDebug("recv_msg_cal：count=%d",count);
#endif
    if(count == set)
    {
        count = 0;
        set = 0;
        isArm_AcquisitionOn = false;
        ui->textBr_mess->append("数据采集完成！");
        emit sendMsgtoForm("数据采集完成！");
        QMessageBox::information(this,
             tr("提示"),
             tr("采集完成!"));
    }
}

void MainWindow::arm_recv_msg_cmd(char *data, int length)
{
    ui->textBr_mess->append("  "+QString(QLatin1String(data)));
    emit sendMsgtoForm("  "+QString(QLatin1String(data)));
}

/*
return  -1 : 发送失败
        -2 : 网络无连接
*/
char MainWindow::sendtoUart(char *cmd, short int speed, short int angle)
{
    if(tcpserver->isArm_connect() == true)
    {
        char cm;
        if(strcmp(cmd, "forward") == 0)
            cm = 0;
        else if(strcmp(cmd, "back") == 0)
            cm = 1;
        else if(strcmp(cmd, "left") == 0)
            cm = 2;
        else if(strcmp(cmd, "right") == 0)
            cm = 3;
        char data[16];
        memset(data, 0, 16);
        data[0] = 0xff;
        data[1] = cm;
        char *ptr = (char*)&speed;
        data[2] = *(ptr+0);
        data[3] = *(ptr+1);
        ptr = (char*)&angle;
        data[4] = *(ptr+0);
        data[5] = *(ptr+1);
        data[6] = 0xff;
        ptr = NULL;
        if(tcpserver->sendPackage((char*)data, 16, "uart"))
        {
            return 0;
        }
        else
        {
           return -1;
        }
    }
    else
    {
        return -2;
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent  *event)
{
    if(event->key() == Qt::Key_Q)
    {
        isKey_Q = false;
        if(event->isAutoRepeat()) return;
    }

    if(event->key() == Qt::Key_8)
    {
        isKey_8 = false;
        if(event->isAutoRepeat()) return;
    }
    if(event->key() == Qt::Key_6)
    {
        isKey_6 = false;
        if(event->isAutoRepeat()) return;
    }
    if(event->key() == Qt::Key_4)
    {
        isKey_4 = false;
        if(event->isAutoRepeat()) return;
    }
    if(event->key() == Qt::Key_5)
    {
        isKey_5 = false;
        if(event->isAutoRepeat()) return;
    }

    int ret = carContorl(isKey_4, isKey_5, isKey_6, isKey_8, isKey_Q);
    if(ret == -1)
        emit sendMsgtoForm("# 发送失败");
    else if(ret == -2)
        emit sendMsgtoForm("# 网络已断开！");
}

void MainWindow::keyPressEvent(QKeyEvent  *event)
{
    if(ui->sendLineEdit->hasFocus())
        if(event->key()==Qt::Key_Enter || event->key()==Qt::Key_Return )
        {
            if(tcpserver->isArm_connect() == true)
            {
                QString str = ui->sendLineEdit->text();
                ui->sendLineEdit->clear();
                QByteArray arry = str.toLatin1();
                char *data = arry.data();
                int length = arry.length();
                if(length == 0)
                {
                    ui->textBr_mess->append("# ");
                    emit sendMsgtoForm("# ");
                }
                else
                {

                    if(tcpserver->sendData(data, length))
                    {
                        ui->textBr_mess->append("# "+str);
                        emit sendMsgtoForm("# "+str);
                    }
                    else
                    {
                        ui->textBr_mess->append("# tcpserver data send failed!");
                        emit sendMsgtoForm("# tcpserver data send failed!");
                    }
                }
            }
            else
            {
                ui->textBr_mess->append("# network is down!");
                emit sendMsgtoForm("# network is down!");
            }
        }

    if(event->key() == Qt::Key_Q)
    {
        isKey_Q = true;
        // if(event->isAutoRepeat())  return;
    }

    if(event->key()==Qt::Key_8)
    {
        isKey_8 = true;
       // if(event->isAutoRepeat())  return;
    }
    if(event->key()==Qt::Key_5)
    {
        isKey_5 = true;
        //if(event->isAutoRepeat()) return;
    }
    if(event->key()==Qt::Key_4)
    {
        isKey_4 = true;
        //if(event->isAutoRepeat()) return;
    }
    if(event->key()==Qt::Key_6)
    {
        isKey_6 = true;
        //if(event->isAutoRepeat()) return;
    }
    int ret = carContorl(isKey_4, isKey_5, isKey_6, isKey_8, isKey_Q);
    if(ret == -1)
        emit sendMsgtoForm("# 发送失败");
    else if(ret == -2)
        emit sendMsgtoForm("# 网络已断开！");
}

int MainWindow::carContorl(bool isk4, bool isk5, bool isk6, bool isk8, bool iskQ)
{
    int ret=0;
    bool isk = false,ok;
    char cmd[8];
    if(isk8 && !isk5)
    {
        isk = true;
        memcpy(cmd, "forward", 8);
    }
    else if(!isk8 && isk5)
    {
        isk = true;
        memcpy(cmd, "back", 5);
    }
    //直行
    if(isk && !isk4 && !isk6)
        ret = sendtoUart(cmd,ui->speed_edit->text().toInt(&ok,10), 0);
    //右转
    else if(isk && !isk4 && isk6 && !iskQ)
        ret = sendtoUart(cmd, ui->speed_edit->text().toInt(&ok,10),
            ui->angle_edit->text().toInt(&ok,10));
    //左转
    else if(isk && isk4 && !isk6 && !iskQ)
        ret = sendtoUart(cmd, ui->speed_edit->text().toInt(&ok,10),
            -1*ui->angle_edit->text().toInt(&ok,10));
    //右漂
    else if(isk && !isk4 && isk6 && iskQ)
        ret = sendtoUart("right", ui->speed_edit->text().toInt(&ok,10),0);
    //左漂
    else if(isk && isk4 && !isk6 && iskQ)
        ret = sendtoUart("left", ui->speed_edit->text().toInt(&ok,10),0);
    else
        ret = sendtoUart("forward", 0,0);
    return ret;
}

void MainWindow::arm_net_down()
{
    ui->textBr_mess->append("# network is down");
    emit sendMsgtoForm("# network is down!");
}
void MainWindow::arm_net_connect()
{
    ui->textBr_mess->append("# network is connect");
    emit sendMsgtoForm("# network is connect!");
}

void MainWindow::on_bt_connect_clicked()
{
    bool ok;
    short int duty = ui->portLineEdit->text().toInt(&ok,10);

    char *ptr = (char*)&duty;
    int i;
    char num[2];
    for(i=0; i<2; i++)
        num[i] = *(ptr+i);
    short int Duty;
    memcpy(&Duty, num, 2);
    qDebug("duty = %d\nDuty = %d\nsizeof(int)=%d\nsizeof(short int)=%d"
           ,duty,Duty,sizeof(int),sizeof(short int));
}

void MainWindow::on_bt_disconnect_clicked()
{

}

void MainWindow::on_bt_send_clicked()
{
    if(tcpserver->isArm_connect() == true)
    {
        QString str = ui->sendLineEdit->text();
        ui->sendLineEdit->clear();
        QByteArray arry = str.toLatin1();
        char *data = arry.data();
        int length = arry.length();
        if(length == 0)
        {
            ui->textBr_mess->append("# ");
            emit sendMsgtoForm("# ");
        }
        else
        {
            if(tcpserver->sendData(data, length))
            {
                ui->textBr_mess->append("# "+str);
                emit sendMsgtoForm("# "+str);
            }
            else
            {
                ui->textBr_mess->append("# tcpserver data send failed!");
                emit sendMsgtoForm("# tcpserver data send failed!");
            }
        }
    }
    else
    {
        ui->textBr_mess->append("# network is down!");
        emit sendMsgtoForm("# network is down!");
    }
}

int MainWindow::package(char *data, int len, char *head,
                         char cmd_snd[], int *length)
{
    if(strcmp(head, "uart_cmd") == 0)
    {
        struct uart_cmd_info cmd_info;
        char uart_snd_buf[sizeof(cmd_info)];
        if(len > UART_CMD_BUFF_SIZE)
            return -1;
        memset(&cmd_info, 0, sizeof(cmd_info));
          //添加包头
        memcpy(cmd_info.head, "uart_cmd", 8);
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

void MainWindow::recvMsgfromForm(QString str)
{
    if(tcpserver->isArm_connect() == true)
    {
        QByteArray arry = str.toLatin1();
        char *data = arry.data();
        int length = arry.length();
        if(length == 0)
        {
            ui->textBr_mess->append("# ");
            emit sendMsgtoForm("# ");
        }
        else
        {
            if(tcpserver->sendPackage(data, length, "cmd"))
            {
                ui->textBr_mess->append("# "+str);
                emit sendMsgtoForm("# "+str);
            }
            else
            {
                ui->textBr_mess->append("# tcpserver data send failed!");
                emit sendMsgtoForm("# tcpserver data send failed!");
            }
        }
    }
    else
    {
        ui->textBr_mess->append("# network is down!");
        emit sendMsgtoForm("# network is down!");
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == ui->paintwidget && event->type() == QEvent::Paint)
    {
        QPainter painter(ui->paintwidget);
        mPainter->setPointcolor(Qt::green);
        mPainter->setPoint(arm_coor_x,arm_coor_y);
        mPainter->setPainter(&painter);
        mPainter->paint();
        mPainter->setPointcolor(Qt::red);
        mPainter->setPoint(phone_coor_x,phone_coor_y);
        mPainter->setPainter(&painter);
        mPainter->paint();
    }
    return QWidget::eventFilter(obj,event);
}

//数据采集相关
void MainWindow::on_acq_bt_clicked()
{
    isPhone_AcquisitionOn = true;
}

void MainWindow::on_acq_bt_arm_clicked()
{
    isArm_AcquisitionOn = true;
}


void MainWindow::on_change_bt_clicked()
{
    bool ok;
    int x = ui->change_x->text().toLatin1().toInt(&ok,10);
    int y = ui->change_y->text().toLatin1().toInt(&ok,10);
    mPainter->setX_axes(x);
    mPainter->setY_axes(y);
    ui->paintwidget->update();
}
