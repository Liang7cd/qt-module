#include "outlog.h"
#include <QString>
#include <QLoggingCategory>
#include <QIODevice>
#include <QDir>
#include <QDebug>
#include <QDateTime>
#include <QRegExp>

QString OutLog::logFileName;
QMutex OutLog::mutex;
QFile OutLog::file;
QTextStream OutLog::textStream;

OutLog::OutLog(QObject *parent) : QObject(parent)
{
}

void OutLog::LogMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString text;
    switch(type)
    {
    case QtDebugMsg:
        text = QString(u8"[Debug]");
        break;
    case QtInfoMsg:
        text = QString(u8"[Info]");
        break;
    case QtWarningMsg:
        text = QString(u8"[Warning]");
        break;
    case QtCriticalMsg:
        text = QString(u8"[Critical]");
        break;
    case QtFatalMsg:
        text = QString(u8"[Fatal]");
    }
    text.append(QString(u8"[%1]").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")));
    //提取文件名
    QStringList list = QString(context.file).split("\\");
    text.append(QString(u8"[%1]").arg(list.at(list.count()-1)));
    //提取函数名
    QString function = QString(context.function);
    int bpos = function.indexOf("(");//从前面开始查找
    int epos = function.leftRef(bpos).lastIndexOf(" ")+1;//从后面开始查找
    QString functionName = function.mid(epos, bpos-epos);
    text.append(QString(u8"[%1][%2]").arg(functionName).arg(context.line));
    text +=  QString(u8":{%1}").arg(msg);

    QByteArray data = text.toUtf8();
    mutex.lock();
    file.setFileName(logFileName);
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    textStream.setDevice(&file);
    textStream << data << endl;
    file.close();
    mutex.unlock();
    switch(type)
    {
    case QtDebugMsg:
        qDebug(u8"%s", data.constData());
        break;
    case QtInfoMsg:
        qInfo(u8"%s", data.constData());
        break;
    case QtWarningMsg:
        qWarning(u8"%s", data.constData());
        break;
    case QtCriticalMsg:
        qCritical(u8"%s", data.constData());
        break;
    case QtFatalMsg:
        qFatal(u8"%s", data.constData());
    }
}

void OutLog::logInit(QString versionInfo)
{
    qDebug("logInit");
    QString logDirectory  = QString(u8"%1/Log/%2/").arg(QApplication::applicationDirPath(), QDateTime::currentDateTime().date().toString("yyyy-MM-dd"));
    QDir dir(logDirectory);
    if(!dir.exists())
        dir.mkpath(logDirectory);
    logFileName = logDirectory + (QDateTime::currentDateTime().toString("yyyy-MM-dd-hhmmss")+".log");

    file.setFileName(logFileName);
    file.open(QIODevice::Truncate | QIODevice::WriteOnly);
    textStream.setDevice(&file);
    textStream << "** FactoryVersion:[" + versionInfo + "] **\r\n" << endl;
    file.close();

    //安装上述自定义函数
    qInstallMessageHandler(LogMessageOutput);
    qDebug(u8"logFileName:%s", qUtf8Printable(logFileName));
}
