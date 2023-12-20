#ifndef BLUETOOTHCONTROL_H
#define BLUETOOTHCONTROL_H

#include <QMainWindow>
#include "QLowEnergyController"
#include "QBluetoothLocalDevice"
#include "QBluetoothDeviceDiscoveryAgent"
#include "QBluetoothSocket"
#include "QDebug"
#include "QListWidgetItem"
#include "QLabel"
#include "QMessageBox"
#include "QLowEnergyDescriptor"
#include "QLowEnergyService"
#include "QLowEnergyCharacteristic"
#include "QBluetoothUuid"
#include "QTimer"
#include "QFile"
#include "QThread"

#include "btdevcell.h"

namespace Ui {
class BluetoothControl;
}

class BluetoothControl : public QMainWindow
{
    Q_OBJECT

public:
    explicit BluetoothControl(QWidget *parent = nullptr);
    ~BluetoothControl();

    QList<QBluetoothDeviceInfo> devInfos;
    QList<QBluetoothUuid> btUuids;
    QBluetoothUuid serviceUUID;

    QLowEnergyController *lowBtControl;
    QLowEnergyService * m_service;
    QLowEnergyCharacteristic m_characteristic;

private:
    Ui::BluetoothControl *ui;

signals:
    void toConectCharacteristic(QLowEnergyService * m_service);
    void stateChanged(QString newState);

private slots:
    void on_btnSerachDevice_clicked();
    void DiscoveryDevice(QBluetoothDeviceInfo devInfo);
    void on_pushButton_find_clicked();
};

#endif // BLUETOOTHCONTROL_H
