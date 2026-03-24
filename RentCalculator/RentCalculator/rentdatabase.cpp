#include "rentdatabase.h"

#include <QApplication>
#include <QDateTime>
#include <QDir>
#include <QDebug>

RentDataBase::RentDataBase(QObject *parent) : QObject(parent)
{
    QString dataDirectory;
    dataDirectory = QString(u8"%1/Data/").arg(QApplication::applicationDirPath());
    QDir dir(dataDirectory);
    if(!dir.exists()) {
        dir.mkpath(dataDirectory);
    }
    m_rentInfoFileName = dataDirectory + "RentInfo.csv";
}

RentDataBase::~RentDataBase()
{

}

int RentDataBase::DataBaseInit(QString user)
{
    qDebug("m_rentInfoFileName:[%s]",qUtf8Printable(m_rentInfoFileName));
    m_rentInfoFile.setFileName(m_rentInfoFileName);
    /*
    if(QFileInfo(m_rentInfoFileName).isFile()) {
        m_rentInfoFile.open(QFileDevice::WriteOnly | QFileDevice::Append);
    }else{
        m_rentInfoFile.open(QFileDevice::Truncate | QFileDevice::WriteOnly);
    }
    */
    m_rentInfoFile.open(QFileDevice::WriteOnly | QFileDevice::Append);

    m_rentInfoFile.close();
    return 0;
}

int RentDataBase::DataBaseWrite(RentInfo rentInfo)
{
    qDebug("");
    QStringList qstrList;
    qstrList.append(QString(u8"Date:[%1]").arg(rentInfo.date));
    qstrList.append(QString(u8"Water:[%1]").arg(rentInfo.water));
    qstrList.append(QString(u8"Electric:[%1]").arg(rentInfo.electric));
    qstrList.append(QString(u8"Rent:[%1]").arg(rentInfo.rent));

    if(m_rentInfoFile.open(QFileDevice::WriteOnly | QFileDevice::Append))
    {
        QByteArray infoStr = QString(u8"%1\r\n").arg(qstrList.join(",")).toUtf8();
        qDebug(infoStr.data());
        m_rentInfoFile.write(infoStr);
        m_rentInfoFile.close();
    }else{
        qFatal("open file missing log!") ;
        qDebug() << qstrList;
    }
    return 0;
}
