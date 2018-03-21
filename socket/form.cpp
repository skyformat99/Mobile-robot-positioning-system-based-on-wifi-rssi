#include "form.h"
#include "ui_form.h"

Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);
    showflag = true;
    ui->textEdit->setTextColor(Qt::white);
    ui->textEdit->setFontPointSize(16);
    ui->textEdit->setReadOnly(true);
}

Form::~Form()
{
    delete ui;
}

void Form::recvMsgfromMain(QString str)
{
    ui->textEdit->append(str);
    //ui->textEdit->setText("ddkf: 78\ndgfl 88\nfslfk 90\n");
}

void Form::recvMsg_val_fromMain(double *val, int length)
{
    if(showflag == true)
    {
        memcpy(value, val, length);
        print_state();
    }
}

void Form::print_state()
{
    ui->textEdit->setText("Single Level:(dBm)    坐标：    ");
    ui->textEdit->append("TP-LINK_208_1   "+QString::number(value[0])
            +"      X:"+QString::number(value[8]));
    ui->textEdit->append("TP-LINK_208_2   "+QString::number(value[1])
            +"      Y:"+QString::number(value[9]));
    ui->textEdit->append("TP-W            "+QString::number(value[2]));
    ui->textEdit->append("HiWiFi_3D93C2   "+QString::number(value[3]));
    ui->textEdit->append("202             "+QString::number(value[4]));
    ui->textEdit->append("210             "+QString::number(value[5]));
    ui->textEdit->append("TP-LINK_885C    "+QString::number(value[6]));
    ui->textEdit->append("TP-LINK_7FEBEC  "+QString::number(value[7]));
}

void Form::keyPressEvent(QKeyEvent  *event)
{
    if(ui->lineEdit->hasFocus() || ui->textEdit->hasFocus() || this->hasFocus())
        if(event->key()==Qt::Key_Enter || event->key()==Qt::Key_Return )
        {

            if(strcmp(ui->lineEdit->text().toLatin1().data(),"state") == 0)
                showflag = true;
            else
            {
                emit sendMsgtoMain(ui->lineEdit->text());
                showflag = false;
            }
            ui->lineEdit->clear();
        }
}
