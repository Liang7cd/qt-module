#ifndef OUTLOG_H
#define OUTLOG_H

#include <QObject>
#include <QFile>
#include <QMutex>
#include <QApplication>

class OutLog : public QObject
{
    Q_OBJECT
public:
    explicit OutLog(QObject *parent = NULL);

public:
    /** 日志类初始化接口 */
    static void logInit(QString versionInfo);
    /** 用作监听qDebug输出 */
    static void LogMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

private:
    static QString logFileName;
    static QMutex mutex;
    static QFile file;
    static QTextStream textStream;
};

#endif // OUTLOG_H
