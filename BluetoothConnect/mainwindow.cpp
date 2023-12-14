#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QTextCodec>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->btConnect = new BluetoothConnect();

    ui->lbImfor->setText("连接状态：未连接");

    /* 监听连接设备窗口发送的连接服务信号 */
    connect(this->btConnect, &BluetoothConnect::toConectCharacteristic, this, &MainWindow::ConnectCharacteristic, Qt::QueuedConnection);
    /* 连接界面和主界面的状态交互 */
    connect(this->btConnect, &BluetoothConnect::stateChanged, this, &MainWindow::StateChanged, Qt::QueuedConnection);
}

MainWindow::~MainWindow()
{
    if(ui->lbImfor->text() == "连接状态：已连接")
    {
        this->btConnect->lowBtControl->disconnectFromDevice();
    }
    delete ui;
}

/**
 * 等待连接完成后发送信号  然后监听特征对象的变化
 * @brief MainWindow::ConnectCharacteristic
 * @param m_service
 */
void MainWindow::ConnectCharacteristic(QLowEnergyService * m_service)
{
    // 通过监听特征对象的变化
    //写的和读取到的都会进入到这个位置
    ui->lbImfor->setText("连接状态：已连接");

    connect(m_service,&QLowEnergyService::characteristicChanged,[=](QLowEnergyCharacteristic c,QByteArray value) {
        this->revData.append(value); //就将本次的字符数组暂存到全局字符数组中
        ui->tbRecvData->append(this->revData);
    });

    //service错误监听
    connect(m_service,static_cast<void (QLowEnergyService::*)(QLowEnergyService::ServiceError error)>(&QLowEnergyService::error),
    [=](QLowEnergyService::ServiceError newErrorr){
         ui->lbImfor->setText("连接状态：连接中断");
    });

    //没有写响应属性的Characteristic不会进入这个连接
    connect(m_service,&QLowEnergyService::characteristicWritten,[=](QLowEnergyCharacteristic c,QByteArray value) {
        qDebug() << "characteristicChanged state change::";
        qDebug() << "value length::" << value.length();
        qDebug() << "value length::" << value;
    });
}

/* 监听蓝牙连接的状态变化 */
void MainWindow::StateChanged(QString newState)
{
    qDebug("newState:[%s]", qUtf8Printable(newState));
    ui->lbImfor->setText(newState);
}

void MainWindow::on_pbDetect_clicked()
{
    this->btConnect->show();
}

void MainWindow::on_pbSend_clicked()
{
    if(ui->lbImfor->text() != "连接状态：已连接")
    {
        QMessageBox::warning(this,"警告","蓝牙还未连接，请先连接后再进行操作！");
        return ;
    }

    QString str = ui->teSendData->toPlainText();
    //将字符串拆分为每20个字符发送一次，超过后会发送失败
    for(int i=0;i<str.length()/20+1;i++)
    {
        QString s = "";
        if((i+1)*20 <= str.length())
        {
            s = str.mid(i*20,20);
        }
        else
        {
            s = str.mid(i*20,-1);
        }
        //       qDebug() << s;

        //以WriteWithoutResponse这种模式发送的数据  长度不能超过20bytes，也就是20个字符
        btConnect->m_service->writeCharacteristic(btConnect->m_characteristic,s.toUtf8(),QLowEnergyService::WriteMode::WriteWithoutResponse);
    }
}

