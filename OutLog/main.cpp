#include "mainwindow.h"

#include <QApplication>
#include <QDir>
#include <QLockFile>
#include <QMessageBox>
#include <QDebug>

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

//通过文件锁的方式实现程序只能打开一次的限制（0：限制当前目录的程序，1：所有目录下的同名程序）
static const QString getLockFilePath(int flag)
{
    QString lockFilePath;
    // 获取应用程序的名称
    QString appName = QApplication::applicationName();
    if(!flag) {
        // 构建文件锁的路径（锁文件的位置在“程序当前目录\.OutLog.lock”）
        lockFilePath = QApplication::applicationDirPath() + "/." + appName + ".lock";
    } else {
        // 获取用户的主目录
        QString homePath = QDir::homePath();
        // 构建文件锁的路径（锁文件的位置在“C:\Users\用户名\.OutLog.lock”）
        lockFilePath = homePath + QDir::separator() + "." + appName + ".lock";
    }
    return lockFilePath;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString versionInfo = buildVersionInfo();

    QString lockFilePath = getLockFilePath(0);
    qDebug() << lockFilePath;
    // 创建文件锁
    QLockFile lockFile(lockFilePath);
    // 尝试加锁（100毫秒超时）
    if (!lockFile.tryLock(100))
    {
#if 1
        QMessageBox::critical(nullptr, u8"启动失败", u8"应用程序正在运行！");
        return -1;
#else
        //ChatGPT：如果应用程序崩溃或意外退出，文件锁可能不会被正确释放（但实际测试发现程序奔溃并不会导致死锁，保留下列代码以待观察）
        QMessageBox messageBox(QMessageBox::NoIcon, QString(u8"启动异常").toUtf8(), QString(u8"应用程序正在运行，是否仍然运行?").toUtf8(), QMessageBox::Yes|QMessageBox::No, NULL);
        int iResult = messageBox.exec();
        if (iResult == QMessageBox::Yes) {
            qDebug("clicked on Yes");
            // 如果锁文件存在，则尝试删除
            if (QFile::exists(lockFilePath))
            {
                QFile::remove(lockFilePath);
                //当前程序重新上锁
                if (!lockFile.tryLock(100)) {
                    QMessageBox::critical(nullptr, u8"启动失败", u8"未知异常，请联系开发人员！");
                    return -2;
                }
            }
        } else {
            qDebug("clicked on No");
            return -1;
        }
#endif
    }

    MainWindow w(versionInfo);
    w.show();
    return a.exec();
}
