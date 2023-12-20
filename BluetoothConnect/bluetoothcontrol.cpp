#include "bluetoothcontrol.h"
#include "ui_bluetoothcontrol.h"

#include <QInputDialog>

BluetoothControl::BluetoothControl(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BluetoothControl)
{
    ui->setupUi(this);
    this->setWindowTitle("连接设备");

    //设置设备列表的标题
    QListWidgetItem *item = new QListWidgetItem();
    //创建自定义的窗口 然后放入到listwidget中
    BtDevCell *bdc = new BtDevCell();
    //设置item的高
    item->setSizeHint(QSize(ui->lwShow->width(),bdc->height()));
    //设置label的显示
    bdc->btName->setText("名称");
    bdc->btAddr->setText("地址");
    bdc->btRssi->setText("信号强度");

    //把item加入到listwidget
    ui->lwShow->addItem(item);
    //设置item的窗口为自定义的窗口
    ui->lwShow->setItemWidget(item,bdc);

    ui->lbImfor->setText("连接状态：等待连接......");

    //创建通信的socket 普通蓝牙使用
    //QBluetoothSocket *btSocket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol,this);

    //设备列表被点击事件
    connect(ui->lwShow,&QListWidget::itemDoubleClicked,[=](QListWidgetItem *item){
        static bool isFirstPoint = true;
        if(isFirstPoint)
        {
            isFirstPoint = false;
            //创建蓝牙控制器
            this->lowBtControl = QLowEnergyController::createCentral(this->devInfos.at(ui->lwShow->currentRow()-1),this);
            //连接设备
            this->lowBtControl->connectToDevice();

            ui->lbImfor->setText("连接状态：正在连接，请等待......");

            //连接成功后进入
            connect(this->lowBtControl,&QLowEnergyController::connected,[=](){
                qDebug() << "连接成功";
                //搜索服务
                this->lowBtControl->discoverServices();
                ui->lbImfor->setText("连接状态：连接成功，准备连接服务，请等待......");
            });

            //连接失败后进入
            connect(this->lowBtControl,static_cast<void (QLowEnergyController::*)(QLowEnergyController::Error newError)>(&QLowEnergyController::error),
            [=](QLowEnergyController::Error newError){
                qDebug() << newError;
                ui->lbImfor->setText("连接状态：连接失败");
                emit stateChanged("连接状态：连接失败");
            });

//        //连接失败后进入
//        connect(this->lowBtControl,&QLowEnergyController::stateChanged,[=](QLowEnergyController::ControllerState state){
//                qDebug() << state;
//        });

            //搜索到服务后进入
            connect(this->lowBtControl,&QLowEnergyController::serviceDiscovered, [=](QBluetoothUuid serviceUuid) {
                qDebug() << "发现服务" << serviceUuid;

                if(serviceUuid == QBluetoothUuid(quint16(0xffe0))) {  //我们用的服务类型是0xffe0对应的uui
                     //创建服务
                     this->m_service = this->lowBtControl->createServiceObject(QBluetoothUuid(serviceUuid),this);
                    this->serviceUUID = serviceUuid;

                     if(m_service==NULL)
                     {
                         qDebug() << "服务建立失败!";
                         ui->lbImfor->setText("连接状态：服务建立失败！请重试");
                     }
                     else
                     {
                         qDebug() << "服务建立成功!";
                         ui->lbImfor->setText("连接状态：服务建立成功！可以返回");

                         // 服务对象创建成功后，监听服务状态变化，如果状态变成已发现，则进行后续服务下特征对象获取
                         connect(this->m_service,&QLowEnergyService::stateChanged, this, [this]() {
                             qDebug() << "service state change" << m_service->state();

                             //发现服务, 建立characteristic对象实例
                             //获取特征对象
                             QList<QLowEnergyCharacteristic> list=this->m_service->characteristics();

                             if(m_service->state() == QLowEnergyService::ServiceDiscovered) {
                                 //获取特征对象
                                 QList<QLowEnergyCharacteristic> list= this->m_service->characteristics();

                                 for(int i=0;i<list.count();i++)
                                 {
                                     this->m_characteristic = list.at(i);
                                     qDebug() << this->m_characteristic.properties();

                                     /*如果QLowEnergyCharacteristic对象有效，则返回true，否则返回false*/
                                     if(m_characteristic.isValid() && (this->m_characteristic.properties() & 0x10))
                                     {
                                         qDebug() << "-------------";
                                         //描述符定义特征如何由特定客户端配置。
                                         QLowEnergyDescriptor m_notificationDesc = m_characteristic.descriptor(QBluetoothUuid::DescriptorType::ClientCharacteristicConfiguration);
                                         if(m_notificationDesc.isValid())
                                         {
                                             //写描述符
                                             m_service->writeDescriptor(m_notificationDesc, QByteArray::fromHex("0100"));
                                             //m_service->writeDescriptor(m_notificationDesc, QByteArray::fromHex("FEE1"));
                                         }
                                     }
                                 }
                             }
                         });

                         //service错误监听
                         connect(m_service,static_cast<void (QLowEnergyService::*)(QLowEnergyService::ServiceError error)>(&QLowEnergyService::error),
                         [=](QLowEnergyService::ServiceError newErrorr){
                             if(QLowEnergyService::NoError == newErrorr)
                             {
                                   qDebug() <<"没有发生错误。";
                                   ui->lbImfor->setText("连接状态：没有发生错误");
                             }
                             if(QLowEnergyService::OperationError==newErrorr)
                             {
                                  qDebug() <<"错误: 当服务没有准备好时尝试进行操作!";
                                  ui->lbImfor->setText("连接状态：错误:当服务没有准备好时尝试进行操作");
                             }
                             if(QLowEnergyService::CharacteristicReadError==newErrorr)
                             {
                                  qDebug() <<"尝试读取特征值失败!";
                                  ui->lbImfor->setText("连接状态：尝试读取特征值失败");
                             }
                             if(QLowEnergyService::CharacteristicWriteError==newErrorr)
                             {
                                  qDebug() <<"尝试为特性写入新值失败!";
                                  ui->lbImfor->setText("连接状态：尝试为特性写入新值失败");
                             }
                             if(QLowEnergyService::DescriptorReadError==newErrorr)
                             {
                                  qDebug() <<"尝试读取描述符值失败!";
                                  ui->lbImfor->setText("连接状态：尝试读取描述符值失败");
                             }
                             if(QLowEnergyService::DescriptorWriteError==newErrorr)
                             {
                                  qDebug() <<" 尝试向描述符写入新值失败!";
                                  ui->lbImfor->setText("连接状态：尝试向描述符写入新值失败");
                             }
                             if(QLowEnergyService::UnknownError==newErrorr)
                             {
                                  qDebug() <<"与服务交互时发生未知错误!";
                                  ui->lbImfor->setText("连接状态：与服务交互时发生未知错误！请重新连接");
                             }

                         });

//                         //通知主窗口可以连接service了
//                         emit toConectCharacteristic(m_service);

                         // 触发服务详情发现函数 ，不要忘记调用
                         m_service->discoverDetails();

                     }

                 }
            });
        }
        else
        {
            this->m_service = this->lowBtControl->createServiceObject(QBluetoothUuid(this->serviceUUID),this);

            if(m_service==NULL)
            {
                qDebug() << "服务建立失败!";
                ui->lbImfor->setText("连接状态：服务建立失败！请重试");
            }
            else
            {
                qDebug() << "服务建立成功!";
                ui->lbImfor->setText("连接状态：服务建立成功！可以返回");

                // 服务对象创建成功后，监听服务状态变化，如果状态变成已发现，则进行后续服务下特征对象获取
                connect(this->m_service,&QLowEnergyService::stateChanged, this, [this]() {
                    qDebug() << "service state change" << m_service->state();

                    //发现服务, 建立characteristic对象实例
                    //获取特征对象
                    QList<QLowEnergyCharacteristic> list=this->m_service->characteristics();

                    if(m_service->state() == QLowEnergyService::ServiceDiscovered) {
                        //获取特征对象
                        QList<QLowEnergyCharacteristic> list= this->m_service->characteristics();

                        for(int i=0;i<list.count();i++)
                        {
                            this->m_characteristic = list.at(i);
                            qDebug() << this->m_characteristic.properties();

                            /*如果QLowEnergyCharacteristic对象有效，则返回true，否则返回false*/
                            if(m_characteristic.isValid() && (this->m_characteristic.properties() & 0x10))
                            {
                                qDebug() << "-------------";
                                //描述符定义特征如何由特定客户端配置。
                                QLowEnergyDescriptor m_notificationDesc = m_characteristic.descriptor(QBluetoothUuid::DescriptorType::ClientCharacteristicConfiguration);
                                if(m_notificationDesc.isValid())
                                {
                                    //写描述符
                                    m_service->writeDescriptor(m_notificationDesc, QByteArray::fromHex("0100"));
                                    //m_service->writeDescriptor(m_notificationDesc, QByteArray::fromHex("FEE1"));
                                }
                            }
                        }
                    }
                });

                //通知主窗口可以连接service了
                emit toConectCharacteristic(m_service);

                // 触发服务详情发现函数 ，不要忘记调用
                m_service->discoverDetails();
            }
        }
    });

}

BluetoothControl::~BluetoothControl()
{
    delete ui;
}

void BluetoothControl::on_btnSerachDevice_clicked()
{
    qDebug() << "开始搜索设备";

    //如果已经搜索过一次了，那么设备列表可能就大于1，那么就清空设备列表
    //但是标题也会被清除，所以重新构建
    if(ui->lwShow->count()>1)
    {
        ui->lwShow->clear();
        this->devInfos.clear();

        //设置设备列表的标题
        QListWidgetItem *item = new QListWidgetItem();
        //创建自定义的窗口 然后放入到listwidget中
        BtDevCell *bdc = new BtDevCell();
        //设置item的高
        item->setSizeHint(QSize(ui->lwShow->width(),bdc->height()));
        //设置label的显示
        bdc->btName->setText("名称");
        bdc->btAddr->setText("地址");
        bdc->btRssi->setText("信号强度");

        //把item加入到listwidget
        ui->lwShow->addItem(item);
        //设置item的窗口为自定义的窗口
        ui->lwShow->setItemWidget(item,bdc);
    }

    //创建设备发现对象
    QBluetoothDeviceDiscoveryAgent *discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    discoveryAgent->setLowEnergyDiscoveryTimeout(5000);

    //搜索到设备后进入
    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &BluetoothControl::DiscoveryDevice, Qt::QueuedConnection);
    //搜索设备发生错误
    connect(discoveryAgent, static_cast<void (QBluetoothDeviceDiscoveryAgent::*)(QBluetoothDeviceDiscoveryAgent::Error error)>(&QBluetoothDeviceDiscoveryAgent::error),
    [=](QBluetoothDeviceDiscoveryAgent::Error error){
        qDebug() << error;
        //警告对话框
        QMessageBox::warning(this,"警告","搜索蓝牙设备发生错误，请检查蓝牙是否开启！");
    });

    //搜索设备完成
    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished, [=](){
        qDebug() << "搜索完成";
        QMessageBox::information(this, tr("info"), QString(u8"搜索完成！num:[%1]").arg(devInfos.size()).toUtf8());
        ui->lbImfor->setText("搜索完成！");
    });

    //开始搜索设备
    discoveryAgent->start();
    ui->lbImfor->setText("连接状态：正在搜索设备......");
}

void BluetoothControl::on_pushButton_find_clicked()
{
    bool bRet = false;
    QString findMacAddr;
    findMacAddr = QInputDialog::getText(this, u8"查找", u8"查找的MAC地址：", QLineEdit::Normal, findMacAddr, &bRet);
    if (bRet) {
        qDebug("findMacAddr:[%s]", qUtf8Printable(findMacAddr));

        for(int i=0; i<devInfos.size(); i++) {
            QBluetoothDeviceInfo devInfo = devInfos[i];
            if(findMacAddr == devInfo.address().toString())
            {
                qDebug("rssi:[%d]", devInfo.rssi());
            }
        }
    }
}

void BluetoothControl::DiscoveryDevice(QBluetoothDeviceInfo devInfo)
{
    qDebug() << "搜索到设备";
    //名称不为空且是低功耗蓝牙
    if(devInfo.name() != "" && (devInfo.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration))
    {
        QListWidgetItem *item = new QListWidgetItem();
        BtDevCell *bdc = new BtDevCell();

        item->setSizeHint(QSize(ui->lwShow->width(),bdc->height()));

        bdc->btName->setText(devInfo.name());
        bdc->btAddr->setText(devInfo.address().toString());
        bdc->btRssi->setText(QString::number(devInfo.rssi()));

        ui->lwShow->addItem(item);
        ui->lwShow->setItemWidget(item,bdc);

        devInfos.append(devInfo);
    }
}
