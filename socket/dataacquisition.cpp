#include "dataacquisition.h"

dataAcquisition::dataAcquisition(QObject *parent) : QObject(parent)
{

    qDebug("dataAcquisition start");
}

void dataAcquisition::saveData(double *data, int length, QString name)
{
    double Data[10];
    memcpy(Data, data, length);
    file.setFileName(name);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) //以文本文式写入
    {
        QTextStream out(&file);
        out<<Data[0]<<" "<<Data[1]<<" "<<Data[2]<<" "<<Data[3]<<" "<<
                      Data[4]<<" "<<Data[5]<<" "<<Data[6]<<" "<<Data[7]
                   <<" "<<Data[8]<<" "<<Data[9]<<endl;
        file.close();
    }
    else
    {
#ifdef DEBUG_ACQ
        qDebug("saveData:打开文件失败");
#endif
    }
}
