#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QTimer>

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

void MainWindow::closeInputDialog()
{
    qDebug("Debug[%s/%d]", __FILE__, __LINE__);
    this->loginSuccess(NORMAL_USERS);
}

void MainWindow::loginSuccess(LOGIN_PERMISSIONS loginPermissions)
{
    qDebug("Debug[%s/%d] loginPermissions:[%d]", __FILE__, __LINE__, loginPermissions);
    m_loginWindow->loginReturn(true, loginPermissions);
    this->show();
}

void MainWindow::loginFailed(QString failInfo)
{
    qDebug("Debug[%s/%d] failInfo:[%s]", __FILE__, __LINE__, qPrintable(failInfo.toUtf8()));
    m_loginWindow->loginReturn(false, NULL_USERS);
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
#if 1
        QMessageBox messageBox(QMessageBox::NoIcon, QString(u8"登录校验").toUtf8(), QString(u8"用户:%1\n密码:%2\n资源代码:%3\n校验是否通过?").arg(loginInfo.name, loginInfo.password, loginExtraData).toUtf8(), QMessageBox::Yes|QMessageBox::No, NULL);
        int iResult = messageBox.exec();
        if (iResult == QMessageBox::Yes) {
            qDebug("clicked on Yes");
            this->loginSuccess(NORMAL_USERS);
        } else {
            qDebug("clicked on No");
            this->loginFailed("clicked on No");
        }
#else
        QTimer::singleShot(5000, this, SLOT(closeInputDialog()));
#endif
    }
    return;
}
