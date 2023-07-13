#include "mainwindow.h"

#include <QApplication>

static const QString buildVersionInfo()
{
    //Tag可选类型（alpha,beta,RC,release）
    const QString versionTag = "release";
    const QString versionNum = "2.1.0";

    QString buildTime;
    buildTime += __DATE__;
    buildTime += __TIME__;
    //当日期小于10时，日期格式为("MMM  d yyyy")(Jun  1 2023)，当日期大于等于10时，日期格式为(MMM dd yyyy)(Jun 25 2023)
    QDateTime buildDateTime;
    if(buildTime.contains("  ")) {
        buildDateTime = QLocale(QLocale::English).toDateTime(buildTime, "MMM  d yyyyhh:mm:ss");
    } else {
        buildDateTime = QLocale(QLocale::English).toDateTime(buildTime, "MMM dd yyyyhh:mm:ss");
    }
    return QString(u8"%1_%2_%3").arg(versionTag, versionNum, buildDateTime.toString("yyyyMMddhhmmss"));
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString versionInfo = buildVersionInfo();
    MainWindow w(versionInfo);
    w.show();
    return a.exec();
}
