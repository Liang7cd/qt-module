#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QKeyEvent>
#include <QWidget>

#include "QProgressIndicator.h"

//登录用户类型
enum LOGIN_PERMISSIONS {NULL_USERS, ADMIN_USERS, TEST_USERS, NORMAL_USERS};

typedef struct LoginInfo {
    QString name;
    QString password;
} LoginInfo;
Q_DECLARE_METATYPE(LoginInfo);//因为要用信号传递，注册数据类型

typedef struct AutoLogin {
    int userID;
    bool enable;
} AutoLogin;

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

    int startLogin();
    int logout();
    void loginReturn(bool login_ok, LOGIN_PERMISSIONS loginPermissions);
    LOGIN_PERMISSIONS m_loginPermissions;

signals:
    void loginInfoVerification(LoginInfo loginInfo, QString loginExtraData);

private slots:
    void on_pushButton_login_clicked();

    void on_pushButton_clear_clicked();

    void on_checkBox_save_stateChanged(int arg1);

    void on_checkBox_auto_stateChanged(int arg1);

    void keyPressEvent(QKeyEvent *event);

private:
    Ui::LoginWindow *ui;

    void interfaceFormatInit();
    int loginInfoSave();
    int getLoginSaveInfo();
    QString encryptionStr(QString str);

    int m_userID;
    QString m_loginInfoFileName;
    LoginInfo m_loginInfo;
    QString m_loginExtraData;
    AutoLogin m_autoLogin;
    QProgressIndicator *m_progressIndicator;
};

#endif // LOGINWINDOW_H
