#ifndef RENTDATABASE_H
#define RENTDATABASE_H

#include <QObject>
#include <QFile>

typedef struct RentInfo {
    QString date;
    QString rent;
    QString electric;
    QString water;
} RentInfo;

class RentDataBase : public QObject
{
    Q_OBJECT
public:
    explicit RentDataBase(QObject *parent = NULL);
    ~RentDataBase();

    int DataBaseInit(QString user);
    int DataBaseWrite(RentInfo rentInfo);

private:
    QString m_rentInfoFileName;
    QFile m_rentInfoFile;
};

#endif // RENTDATABASE_H
