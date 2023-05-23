#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    qDebug("Debug[%s/%d]", __FILE__, __LINE__);
    ui->setupUi(this);

    m_loginWindow = new LoginWindow;
    connect(m_loginWindow, SIGNAL(loginInfoVerification(LoginInfo, QString)), this, SLOT(loginInfoVerification(LoginInfo, QString)), Qt::QueuedConnection);
    m_loginWindow->startLogin();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_logout_clicked()
{
    m_loginWindow->logout();
    this->hide();
}

void MainWindow::loginSuccess(LOGIN_PERMISSIONS loginPermissions)
{
    qDebug("Debug[%s/%d] loginPermissions:[%d]", __FILE__, __LINE__, loginPermissions);
    m_loginWindow->loginReturn(1, loginPermissions);
    this->show();
}

void MainWindow::loginInfoVerification(LoginInfo loginInfo, QString loginExtraData)
{
    qInfo(u8"Name:[%s], Password:[%s], ExtraData:[%s]", qPrintable(loginInfo.name.toUtf8()),
           qPrintable(loginInfo.password.toUtf8()), qPrintable(loginExtraData.toUtf8()));

    if(loginInfo.name == "root" && loginInfo.password == "root") {
        this->loginSuccess(ADMIN_USERS);
    } else if(loginInfo.name == "test" && loginInfo.password == "test") {
        this->loginSuccess(TEST_USERS);
    } else {
        this->loginSuccess(NORMAL_USERS);
    }
    return;
}
