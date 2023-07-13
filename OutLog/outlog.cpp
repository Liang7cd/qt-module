#include "outlog.h"
#include <QString>
#include <QLoggingCategory>
#include <QIODevice>
#include <QDir>
#include <QDebug>
#include <QDateTime>
#include <QRegExp>
#include <QMessageBox>

QString OutLog::m_logFileName;
QMutex OutLog::m_mutex;
QFile OutLog::m_logFile;
QTextStream OutLog::m_textStream;

OutLog::OutLog(QString versionInfo, QString ftyMacAddr, QObject *parent) : QObject(parent),
    m_versionInfo(versionInfo),
    m_ftyMacAddr(ftyMacAddr)
{
    m_logDirPath = QString(u8"%1/Log/").arg(QApplication::applicationDirPath());
    QDir dir(m_logDirPath);
    if(!dir.exists()) {
        dir.mkpath(m_logDirPath);
    }
    qInfo("m_logDirPath:[%s]", qUtf8Printable(m_logDirPath));

    m_logFileName = m_logDirPath + "out.log";
    m_logInfoFileName = m_logDirPath + "log_file_info.txt";
    m_logIndexFileName = m_logDirPath + "log_file_index.txt";
    m_logNumber = "000000";
    m_logDateTime = "00000000-000000";

    connect(&m_processCurl, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processCurlReturn(int,QProcess::ExitStatus)));
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
    m_mutex.lock();
    m_logFile.setFileName(m_logFileName);
    m_logFile.open(QFileDevice::WriteOnly | QFileDevice::Append);
    m_textStream.setDevice(&m_logFile);
    m_textStream << data << endl;
    m_logFile.close();
    m_mutex.unlock();
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

void OutLog::logInit()
{
    QFile logInfoFile(m_logInfoFileName);
    if(logInfoFile.exists()) {
        logInfoFile.open(QFileDevice::ReadOnly);
        QString logInfoStr = QString(logInfoFile.readAll());
        logInfoFile.close();

        QStringList logInfoStrList = logInfoStr.split(" ");
        m_logNumber = logInfoStrList.at(0);
        m_logDateTime = logInfoStrList.at(1);
        qInfo("logInfoStr:[%s], m_logNumber:[%s], m_logDateTime:[%s]", qUtf8Printable(logInfoStr),
               qUtf8Printable(m_logNumber), qUtf8Printable(m_logDateTime));

        if(QFileInfo::exists(m_logFileName)) {
            this->logSave();
        }
    } else {
        qInfo("log info file is not exist.");
    }

    m_logNumber = QString("%1").arg(m_logNumber.toUInt()+1, 6, 10, QChar('0'));
    m_logDateTime = QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss");
    qInfo("m_logNumber:[%s], m_logDateTime:[%s]", qUtf8Printable(m_logNumber), qUtf8Printable(m_logDateTime));
    if (!logInfoFile.open(QFileDevice::WriteOnly | QFileDevice::Truncate)) {
        qCritical(u8"file open failed! name:[%s]", qUtf8Printable(logInfoFile.fileName()));
    }
    logInfoFile.write(qUtf8Printable(m_logNumber + " " + m_logDateTime));
    logInfoFile.close();

    m_logFile.setFileName(m_logFileName);
    m_logFile.open(QFileDevice::WriteOnly | QFileDevice::Truncate);
    m_textStream.setDevice(&m_logFile);
    m_textStream << "** Factory Log Start. Version:[" + m_versionInfo + "] **\r\n" << endl;
    m_logFile.close();

    //安装上述自定义函数
    qInstallMessageHandler(LogMessageOutput);
    qDebug(u8"logFileName:%s", qUtf8Printable(m_logFileName));
}

int OutLog::logSave()
{
    qDebug("m_logNumber:[%s], m_logDateTime:[%s]", qUtf8Printable(m_logNumber), qUtf8Printable(m_logDateTime));
    QDateTime dateTime = QDateTime::fromString(m_logDateTime, "yyyyMMdd-hhmmss");
    QString logBackupPath  = QString(u8"%1/Backup/%2/").arg(m_logDirPath, dateTime.date().toString("yyyy-MM-dd"));
    QDir logBackupDir(logBackupPath);
    if(!logBackupDir.exists())
        logBackupDir.mkpath(logBackupPath);
    QString backupFileName = logBackupPath + m_logNumber + '-' + m_logDateTime + ".log";
    qDebug("backupFileName:[%s]", qUtf8Printable(backupFileName));

    //停止日志再输出到日志
    qInstallMessageHandler(0);

    m_logFile.setFileName(m_logFileName);
    m_logFile.open(QFileDevice::WriteOnly | QFileDevice::Append);
    m_textStream.setDevice(&m_logFile);
    m_textStream << "\r\n** Factory Log End. **" << endl;
    m_logFile.close();

    //重命名并移动日志文件到备份目录
    QFile::rename(m_logFileName, backupFileName);
    uploadLog(backupFileName);

    QString indexInfoStr = m_logNumber + " " + backupFileName + "\r\n";
    QFile logIndexFile(m_logIndexFileName);
    logIndexFile.open(QFileDevice::WriteOnly | QFileDevice::Append);
    logIndexFile.write(qUtf8Printable(indexInfoStr));
    logIndexFile.close();
    return 0;
}

int OutLog::uploadLog(QString fileName)
{
#if 0
    QStringList argu;
    //argu.append("http://192.168.50.57:8700/iot-cloud-api/app/attachment/uploadLog");
    argu.append("-s");
    argu.append("-F");
    argu.append(QString("attachment=@%1").arg(fileName).toUtf8());
    argu.append("-F");
    argu.append(QString("directory=%1").arg(m_ftyMacAddr).toUtf8().replace(":", ""));
    qDebug() << argu.join(",");

    m_processCurl.start("curl", argu);
    if(!m_processCurl.waitForStarted()) {
        qDebug()<<"启动失败 error:"<<m_processCurl.errorString();
    }
    m_processCurl.waitForFinished(-1);
    m_processCurl.close();
#endif
    return 0;
}

void OutLog::processCurlReturn(int exitCode,QProcess::ExitStatus exitStatus)
{
    if(exitCode) {
        qCritical()<<"return error, exitCode " << exitCode << " " << exitStatus;
        QMessageBox::critical(NULL, u8"日志上传失败", QString(u8"curl 调用失败！exitCode:[%1]").arg(exitCode).toUtf8());
        return;
    }

    QString result = QString::fromUtf8(m_processCurl.readAll());
    qDebug("result:[%s]", qUtf8Printable(result));
    if(!result.contains("\"success\":true")) {
        qWarning("return result false!");
        QMessageBox::warning(NULL, u8"日志上传异常", QString(u8"result:[%1]").arg(result).toUtf8());
        return;
    }
    return;
}
