#ifndef RENTDATABASE_H
#define RENTDATABASE_H

#include <QObject>
#include <QFile>

typedef struct RentInfo {
    QString Date;
    QString ElectricMeter;
    QString WaterMeter;
    QString RoomPrice;
    QString ElectricBill;
    QString WaterBill;
    QString TotalRent;
} RentInfo;

class RentDataBase : public QObject
{
    Q_OBJECT
public:
    explicit RentDataBase(QObject *parent = NULL);
    ~RentDataBase();

    int DataBaseInit(QString& user);
    int DataBaseWrite(RentInfo rentInfo);

private:
    QString m_rentInfoFileName;
    QFile m_rentInfoFile;

    QList<RentInfo> m_rentInfo;
};

#endif // RENTDATABASE_H
