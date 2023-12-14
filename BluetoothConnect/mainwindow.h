#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "bluetoothconnect.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    BluetoothConnect *btConnect;/* 蓝牙连接对象 */
    QByteArray revData = ""; /* 收到的字符串暂存 收到\n之后会更新 */

    void ConnectCharacteristic(QLowEnergyService * m_service); /* 接收蓝牙消息的函数 */
    void StateChanged(QString newState);/* 连接状态更改处理的函数 */

private slots:
    void on_pbDetect_clicked();

    void on_pbSend_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
