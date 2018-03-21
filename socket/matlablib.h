#ifndef MATLABLIB_H
#define MATLABLIB_H
#include <QByteArray>
#include <QtNetwork>
#include <QMessageBox>
#include <QMainWindow>
#include "engine.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "my.h"

class Matlablib: public QObject
{
    Q_OBJECT
public:
    explicit Matlablib(QObject *parent = 0);
    ~Matlablib();

    bool engineOpen();
    void engineClose();
    void getMainWindow(QMainWindow* window);
    bool isEngine_open();
    bool isEngine_ready();
    void engineInit();
    int engineClaculate(double* val, int length, double *x, double *y);

private:
    QMainWindow* mainwindow;

    Engine *ep;
    bool isEngineopen;
    bool isEngineready;

signals:
    updateCoordinate(double *x, double *y); //当坐标更新时，发布该信号

};

#endif // MATLABLIB_H
