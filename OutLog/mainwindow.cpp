#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(const QString versionInfo, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_versionInfo(versionInfo)
{
    ui->setupUi(this);
    this->setWindowTitle("Version (" + m_versionInfo + ")");
    //初始化日志记录功能
    OutLog::logInit(versionInfo);
}

MainWindow::~MainWindow()
{
    delete ui;
}

