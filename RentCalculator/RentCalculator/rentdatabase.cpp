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

int RentDataBase::DataBaseInit(QString& user)
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
    user.append("年月  房租  电费  水费\n");
    m_rentInfoFile.open(QFileDevice::ReadOnly | QFileDevice::Text);
    QTextStream stream(&m_rentInfoFile);
    qDebug() << "11111";
    while (!stream.atEnd()) {
        qDebug() << "222222";
        RentInfo rentInfo;
        QString line = stream.readLine();
        qDebug() << line;

        // 处理行数据
        QStringList list = QString(line).split(",");
        for (int i = 0; i < list.size(); ++i) {
            QString cell = list.at(i);
            qDebug() << i << ":" << cell;

            int bpos = cell.indexOf("[");//从前面开始查找
            int epos = cell.leftRef(bpos).lastIndexOf("]")+1;//从后面开始查找

            if (cell.contains("Date")) {
                qDebug() << "包含字符串 'Date'";
                rentInfo.Date = cell.mid(epos, bpos-epos);
            } else if (cell.contains("ElectricMeter")) {
                qDebug() << "包含字符串 'ElectricMeter'";
                rentInfo.ElectricMeter = cell.mid(epos, bpos-epos);
            } else if (cell.contains("WaterMeter")) {
                qDebug() << "包含字符串 'WaterMeter'";
                rentInfo.WaterMeter = cell.mid(epos, bpos-epos);
            } else if (cell.contains("RoomPrice")) {
                qDebug() << "包含字符串 'RoomPrice'";
                rentInfo.RoomPrice = cell.mid(epos, bpos-epos);
            } else if (cell.contains("ElectricBill")) {
                qDebug() << "包含字符串 'ElectricBill'";
                rentInfo.ElectricBill = cell.mid(epos, bpos-epos);
            } else if (cell.contains("WaterBill")) {
                qDebug() << "包含字符串 'WaterBill'";
                rentInfo.WaterBill = cell.mid(epos, bpos-epos);
            } else if (cell.contains("TotalRent")) {
                qDebug() << "包含字符串 'TotalRent'";
                rentInfo.TotalRent = cell.mid(epos, bpos-epos);
            }
        }
        m_rentInfo.append(rentInfo);
        QString txt = QString("%1-%2-%3-%4-%5-%6-%7\n").arg(rentInfo.Date, rentInfo.ElectricMeter, rentInfo.WaterMeter,
                rentInfo.RoomPrice, rentInfo.ElectricBill, rentInfo.WaterBill, rentInfo.TotalRent);
        user.append(txt);
    }
    m_rentInfoFile.close();
    return 0;
}

int RentDataBase::DataBaseWrite(RentInfo rentInfo)
{
    qDebug("");
    QStringList qstrList;
    qstrList.append(QString(u8"Date:[%1]").arg(rentInfo.Date));
    qstrList.append(QString(u8"ElectricMeter:[%1]").arg(rentInfo.ElectricMeter));
    qstrList.append(QString(u8"WaterMeter:[%1]").arg(rentInfo.WaterMeter));
    qstrList.append(QString(u8"RoomPrice:[%1]").arg(rentInfo.RoomPrice));
    qstrList.append(QString(u8"ElectricBill:[%1]").arg(rentInfo.ElectricBill));
    qstrList.append(QString(u8"WaterBill:[%1]").arg(rentInfo.WaterBill));
    qstrList.append(QString(u8"TotalRent:[%1]").arg(rentInfo.TotalRent));

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
