#-------------------------------------------------
#
# Project created by QtCreator 2017-11-13T14:54:26
#
#-------------------------------------------------

QT       += core gui


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = socket
TEMPLATE = app


INCLUDEPATH += $$quote(D:\Program Files (x86)\matlab2014a\extern\include)
LIBS += $$quote(D:\Program Files (x86)\matlab2014a\extern\lib\win64\microsoft\libeng.lib)
LIBS += $$quote(D:\Program Files (x86)\matlab2014a\extern\lib\win64\microsoft\libmat.lib)
LIBS += $$quote(D:\Program Files (x86)\matlab2014a\extern\lib\win64\microsoft\libmx.lib)
LIBS += $$quote(D:\Program Files (x86)\matlab2014a\extern\lib\win64\microsoft\libmex.lib)


SOURCES += main.cpp\
        mainwindow.cpp \
    tcpserver.cpp \
    matlablib.cpp \
    mcurvepaint.cpp \
    dataacquisition.cpp \
    Kaiman.cpp \
    form.cpp

HEADERS  += mainwindow.h \
    tcpserver.h \
    my.h \
    matlablib.h \
    mcurvepaint.h \
    dataacquisition.h \
    Kalman.h \
    form.h

QT += network


FORMS    += mainwindow.ui \
    form.ui

RESOURCES += \
    resource.qrc


