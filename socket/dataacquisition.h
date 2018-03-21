#ifndef DATAACQUISITION_H
#define DATAACQUISITION_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include "my.h"
class dataAcquisition : public QObject
{
    Q_OBJECT
public:
    explicit dataAcquisition(QObject *parent = 0);

    void saveData(double *data, int length, QString name);

private:
    QFile file;

signals:



public slots:

};

#endif // DATAACQUISITION_H
