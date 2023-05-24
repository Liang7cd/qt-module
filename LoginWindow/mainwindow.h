#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "loginwindow.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_logout_clicked();

    void closeInputDialog();
    void loginInfoVerification(LoginInfo loginInfo, QString loginExtraData);

private:
    Ui::MainWindow *ui;

    LoginWindow * m_loginWindow;
    void loginSuccess(LOGIN_PERMISSIONS loginPermissions);
    void loginFailed(QString failInfo);
};
#endif // MAINWINDOW_H
