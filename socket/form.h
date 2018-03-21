#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <QEvent>
#include <QKeyEvent>
#include <QColor>
#include <QFont>
#include <QString>
#include "my.h"

namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = 0);
    ~Form();

private:
    Ui::Form *ui;
    void keyPressEvent(QKeyEvent  *event);           //按键按下事件
    void print_state();

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
    double value[10];
    bool showflag;

private slots:
    void recvMsgfromMain(QString str);
    void recvMsg_val_fromMain(double *val, int length);

signals:
    sendMsgtoMain(QString str);

};

#endif // FORM_H
