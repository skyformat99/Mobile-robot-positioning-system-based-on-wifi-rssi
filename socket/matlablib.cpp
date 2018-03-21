#include "matlablib.h"

Matlablib::Matlablib(QObject *parent):
    QObject(parent)
{
    isEngineopen = false;
    isEngineready = false;
    ep = NULL;
}

Matlablib::~Matlablib()
{
    if(isEngineopen == true)
    {
        engClose(ep);
        ep = NULL;
    }
}

bool Matlablib::engineOpen()
{
    if(isEngineopen == false)
    {
        if (!(ep = engOpen(NULL)))
        {
            isEngineopen = false;
                qDebug("Can't start MATLAB engine");
            return false;
        }
        else
        {
            isEngineopen = true;
            qDebug("MATLAB engine started");
            return true;
        }
    }
    else
    {
        qDebug("engine already open");
        return true;
    }
}

void Matlablib::engineClose()
{
    engClose(ep);
    isEngineopen = false;
    isEngineready = false;
    ep = NULL;
}

bool Matlablib::isEngine_open()
{
    return isEngineopen;
}

bool Matlablib::isEngine_ready()
{
    return isEngineready;
}

void Matlablib::getMainWindow(QMainWindow* window)
{
    mainwindow = window;
}

void Matlablib::engineInit()
{
    if(isEngineopen == true)
    {
        engEvalString(ep, "clear all;");
        engEvalString(ep, "addpath(genpath('E:\\my_qtprogram\\socket\\matlab'))");
        engEvalString(ep, "load('file');");
        engEvalString(ep, "elm_train('file',0,20,'hardlim');");
        isEngineready = true;
        qDebug("MATLAB engine init");
    }
    else
    {
        QMessageBox::information(mainwindow,
             tr("warning"),
             tr("engine is not open, plese open it before do something else!"));
        qDebug("engine is not open, plese open it before do something else!");
    }
}

int Matlablib::engineClaculate(double* val, int length, double *x, double *y)
{
    if(length/sizeof(double) != 10)
    {
        qDebug("data error, return!");
        *x=-1;
        *y=-1;
        return -1;
    }
    else
    {
        mxArray *X = NULL,*Y = NULL;
        mxArray *T = NULL;
        T = mxCreateDoubleMatrix(1, 10, mxREAL);
#ifdef DEBUG_MATLAB
        int i;
        for(i=0; i<10; i++)
        {
            qDebug("data[%d]=%0.2f",i,*(val+i));
        }
#endif
        memcpy((void *)mxGetPr(T), (void *)val, length);
        engPutVariable(ep, "T", T);
        engEvalString(ep, "[x y]=elm_predict(T)");
        X = engGetVariable(ep, "x");
        Y = engGetVariable(ep, "y");
        *x = *mxGetPr(X);
        *y = *mxGetPr(Y);
#ifdef DEBUG_MATLAB
        qDebug("x=%0.2f   y=%0.2f",*x,*y);
#endif
        mxDestroyArray(X);
        mxDestroyArray(Y);
        mxDestroyArray(T);
        emit updateCoordinate(x, y);
        return 0;
    }
}


