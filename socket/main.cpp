#include "mainwindow.h"
#include "form.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    Form f;
    f.show();
    QObject::connect(&w,SIGNAL(sendMsgtoForm(QString)),&f,SLOT(recvMsgfromMain(QString)));
    QObject::connect(&f,SIGNAL(sendMsgtoMain(QString)),&w,SLOT(recvMsgfromForm(QString)));
    QObject::connect(&w,SIGNAL(sendMsgtoForm(double*, int)),
                     &f,SLOT(recvMsg_val_fromMain(double*, int)));

    return a.exec();


}

