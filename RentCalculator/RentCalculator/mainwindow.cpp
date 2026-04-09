#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_rentDataBase(nullptr)
{
    ui->setupUi(this);

    QString rentInfo;
    m_rentDataBase = new RentDataBase(this);
    m_rentDataBase->DataBaseInit(rentInfo);

    ui->rentInfoList->setText(rentInfo);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
    ProductionInfo productionInfo;
    productionInfo.guid = guid;
    productionInfo.cmei = cmei;
    productionInfo.type = QString("GUIDWrite");
    if(1==is_ok) {
        productionInfo.status = QString("1");
        productionInfo.details = QString("guid write ok.");
        ui->pushButton_guidWriteAuto->setEnabled(false);
        m_currentCamera->m_ctei = cmei;
    } else {
        productionInfo.status = QString("0");
        productionInfo.details = QString("guid write fail!");
    }
    m_productionDataBase->DataBaseWrite(productionInfo);
 */
