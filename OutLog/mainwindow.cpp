#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCloseEvent>
#include <QNetworkInterface>

MainWindow::MainWindow(const QString versionInfo, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_versionInfo(versionInfo)
{
    ui->setupUi(this);
    this->setWindowTitle("Version (" + m_versionInfo + ")");

    m_ftyMacAddr = getHostMacAddress();

    //初始化日志记录功能
    m_outLog = new OutLog(m_versionInfo, m_ftyMacAddr);
    m_outLog->logInit();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    qDebug("");
    //窗口关闭时保存日志
    m_outLog->logSave();
    event->accept();
}

QString MainWindow::getHostMacAddress()
{
    //获取所有网络接口列表
    QList<QNetworkInterface> nets = QNetworkInterface::allInterfaces();
    //qDebug() << nets;
    int nCnt = nets.count();
    QString strMacAddr = "";
    for(int i=0; i<nCnt; i++)
    {
        //如果此网络接口被激活并且正在运行并且不是回环地址，且该mac的ip地址不能是回环地址并且是ipv4地址，则就是我们需要找的Mac地址
        if(nets[i].flags().testFlag(QNetworkInterface::IsUp) &&
           nets[i].flags().testFlag(QNetworkInterface::IsRunning) &&
           !nets[i].flags().testFlag(QNetworkInterface::IsLoopBack))
        {
            for(int j=0; j<nets[i].addressEntries().size(); j++)
            {
                //该mac的ip地址不能为172.0.0.1，且需是ipv4地址
                if(nets[i].addressEntries().at(j).ip()!=QHostAddress::LocalHost &&
                   nets[i].addressEntries().at(j).ip().protocol()== QAbstractSocket::IPv4Protocol)
                {
                    strMacAddr = nets[i].hardwareAddress();
                }
            }
        }
    }
    qInfo("strMacAddr:[%s]", qUtf8Printable(strMacAddr));
    return strMacAddr;
}

