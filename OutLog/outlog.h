#ifndef OUTLOG_H
#define OUTLOG_H

#include <QObject>
#include <QFile>
#include <QMutex>
#include <QApplication>
#include <QProcess>

class OutLog : public QObject
{
    Q_OBJECT
public:
    explicit OutLog(QString versionInfo, QString ftyMacAddr, QObject *parent = NULL);

public:
    /** 用作监听qDebug输出 */
    static void LogMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);
    /** 日志类初始化接口 */
    void logInit();
    int logSave();

private:
    int uploadLog(QString fileName);

public:
    static QString m_logFileName;
    QString m_logDirPath;

private slots:
    void processCurlReturn(int exitCode, QProcess::ExitStatus exitStatus);

private:
    static QMutex m_mutex;
    static QFile m_logFile;
    static QTextStream m_textStream;

    const QString m_versionInfo;
    const QString m_ftyMacAddr;

    QString m_logInfoFileName;
    QString m_logIndexFileName;
    QString m_logNumber;
    QString m_logDateTime;

    QProcess m_processCurl;
};

#endif // OUTLOG_H
