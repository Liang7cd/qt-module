#include "loginwindow.h"
#include "ui_loginwindow.h"

#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QByteArray>
#include <QInputDialog>

#include <QtXml>
#include <QDomDocument>
#include <QFileInfo>

LoginWindow::LoginWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginWindow)
{
    //注册数据类型“LoginInfo”，使得能通过signal/slot来传递
    qRegisterMetaType<LoginInfo>("LoginInfo");
    ui->setupUi(this);

    QString xmlDirectory;
    xmlDirectory = QString(u8"%1/xml/").arg(QApplication::applicationDirPath());
    QDir dir(xmlDirectory);
    if(!dir.exists()) {
        dir.mkpath(xmlDirectory);
    }
    m_loginInfoFileName = xmlDirectory + "LoginInfo.xml";
    qDebug("m_loginInfoFileName:%s",qPrintable(m_loginInfoFileName));
    if(QFileInfo::exists(m_loginInfoFileName)) {
        this->getLoginSaveInfo();
    }
    m_loginPermissions = NULL_USERS;
    this->interfaceFormatInit();
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

int LoginWindow::startLogin()
{
    qDebug("");
    if(m_autoLogin.enable)
    {
        if(m_userID == m_autoLogin.userID)
        {
            if(m_loginInfo.name.isEmpty() || m_loginInfo.password.isEmpty()) {
                qWarning("auto login failed! The name and password cannot be empty.");
            }else{
                qInfo(u8"auto login. ID:[%d], Name:[%s], Password:[%s], ExtraData:[%s]", m_userID, qPrintable(m_loginInfo.name.toUtf8()),
                       qPrintable(m_loginInfo.password.toUtf8()), qPrintable(m_loginExtraData.toUtf8()));
                emit loginInfoVerification(m_loginInfo, m_loginExtraData);
                return 0;
            }
        }else{
            qWarning("auto login failed! userID not found!");
        }
    }
    this->show();
    return 0;
}

int LoginWindow::logout()
{
    m_loginPermissions = NULL_USERS;
    this->show();
    return 0;
}

void LoginWindow::loginReturn(int login_ok, LOGIN_PERMISSIONS loginPermissions)
{
    qDebug("login_ok:[%d], loginPermissions:[%d]", login_ok, loginPermissions);
    if(login_ok) {
        m_loginPermissions = loginPermissions;
        this->hide();
    }else{
        QMessageBox::critical(NULL, u8"登录失败", u8"登录信息校验失败！");
    }
    return;
}

void LoginWindow::interfaceFormatInit()
{
    //设置各个标签的位置
    ui->label_title->setGeometry(140,25,160,25);
    ui->groupBox->setGeometry(50,60,300,125);
    ui->label_name->setGeometry(25,25,100,25);
    ui->lineEdit_name->setGeometry(125,25,150,25);
    ui->label_password->setGeometry(25,75,100,25);
    ui->lineEdit_password->setGeometry(125,75,150,25);
    ui->checkBox_auto->setGeometry(85,200,100,25);
    ui->checkBox_save->setGeometry(215,200,100,25);
    ui->pushButton_clear->setGeometry(50,235,120,30);
    ui->pushButton_login->setGeometry(230,235,120,30);
    //密码输入的时候显示圆点
    //ui->lineEdit_password->setEchoMode(QLineEdit::Password);

    //调整字体的大小
    QFont font;
    font.setPointSize(16);
    font.setFamily(("wenquanyi"));
    font.setBold(false);
    ui->label_title->setFont(font);
    font.setPointSize(12);
    ui->label_name->setFont(font);
    ui->label_password->setFont(font);

    if(!m_loginInfo.name.isEmpty() && !m_loginInfo.password.isEmpty()) {
        ui->lineEdit_name->setText(m_loginInfo.name.toUtf8());
        ui->lineEdit_password->setText(m_loginInfo.password.toUtf8());
        ui->checkBox_auto->setChecked(m_autoLogin.enable);
        ui->checkBox_save->setChecked(true);
    }
}

QString LoginWindow::encryptionStr(QString str)
{
    std::wstring wString = str.toStdWString();
    static QByteArray key = "DZW_IPC@";
    for (unsigned int i = 0; i < wString.size(); i++)
    {
        wString[i] = wString[i] ^ key[i % key.size()];
    }
    return QString::fromStdWString(wString).toUtf8();
}

int LoginWindow::loginInfoSave()
{
    // 打开或者创建一个XML文件
    QFile xmlFile(m_loginInfoFileName);
    // 文件存在则打开并清空，否则创建一个文件
    if (!xmlFile.open(QFileDevice::ReadWrite | QFileDevice::Truncate)) {
        qCritical(u8"file open failed! name:[%s]", qPrintable(xmlFile.fileName().toUtf8()));
        return -1;
    }

    QDomDocument doc;
    QDomProcessingInstruction instruction;
    instruction = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(instruction);

    QDomElement login_info = doc.createElement("LoginInfo");
    doc.appendChild(login_info);

    QDomElement user_list = doc.createElement("UserList");
    login_info.appendChild(user_list);

    QDomElement user = doc.createElement("User");
    user.setAttribute("ID", "1");
    user.setAttribute("Name", m_loginInfo.name);
    //QString enc_password = encryptionStr(m_loginInfo.password);
    //user.setAttribute("Password", enc_password);
    user.setAttribute("Password", m_loginInfo.password);
    user.setAttribute("ExtraData", m_loginExtraData);
    user_list.appendChild(user);

    QDomElement auto_login = doc.createElement("AutoLogin");
    login_info.appendChild(auto_login);

    QDomElement user_ID = doc.createElement("UserID");
    QDomText userIDText = doc.createTextNode(QString::number(m_autoLogin.userID).toUtf8());
    user_ID.appendChild(userIDText);
    auto_login.appendChild(user_ID);

    QDomElement enable = doc.createElement("Enable");
    QDomText enableText = doc.createTextNode(m_autoLogin.enable?"1":"0");
    enable.appendChild(enableText);
    auto_login.appendChild(enable);

    QTextStream stream(&xmlFile);
    doc.save(stream, 4);
    xmlFile.close();
    return 0;
}

int LoginWindow::getLoginSaveInfo()
{
    QFile xmlFile;
    xmlFile.setFileName(m_loginInfoFileName);
    if (!xmlFile.open(QFileDevice::ReadOnly)) {
        qCritical(u8"file open failed! name:[%s]", qPrintable(xmlFile.fileName().toUtf8()));
        return -1;
    }

    QString nodename;
    QXmlStreamReader xmlreader(&xmlFile);
    while(!xmlreader.atEnd())
    {
        if (xmlreader.readNext() == QXmlStreamReader::EndDocument) {
            qDebug("parse the end of the xml file.");
            break;
        }
        xmlreader.readNextStartElement();
        nodename = xmlreader.name().toString();
        while(!(nodename == "LoginInfo" && xmlreader.isEndElement()) && !xmlreader.hasError())
        {
            xmlreader.readNextStartElement();
            nodename = xmlreader.name().toString();
            if(nodename == "UserList" && xmlreader.isStartElement())
            {
                while(!(nodename == "UserList" && xmlreader.isEndElement()) && !xmlreader.hasError())
                {
                    xmlreader.readNextStartElement();
                    nodename = xmlreader.name().toString();
                    if(nodename == "User" && xmlreader.isStartElement())
                    {
                        m_userID = xmlreader.attributes().value("ID").toString().toInt();
                        m_loginInfo.name = xmlreader.attributes().value("Name").toString();
                        //QString enc_password = xmlreader.attributes().value("Password").toString();
                        //m_loginInfo.password = encryptionStr(enc_password);
                        m_loginInfo.password = xmlreader.attributes().value("Password").toString();
                        m_loginExtraData = xmlreader.attributes().value("ExtraData").toString();
                        qDebug(u8"ID:[%d], Name:[%s], Password:[%s], ExtraData:[%s]", m_userID, qPrintable(m_loginInfo.name.toUtf8()),
                               qPrintable(m_loginInfo.password.toUtf8()), qPrintable(m_loginExtraData.toUtf8()));
                    }
                }
            }
            else if(nodename == "AutoLogin" && xmlreader.isStartElement())
            {
                while(!(nodename == "AutoLogin" && xmlreader.isEndElement()) && !xmlreader.hasError())
                {
                    xmlreader.readNextStartElement();
                    nodename = xmlreader.name().toString();
                    if(nodename == "UserID" && xmlreader.isStartElement())
                    {
                        m_autoLogin.userID = xmlreader.readElementText().toInt();
                        qDebug(u8"userID:[%d]", m_autoLogin.userID);
                    }
                    else if(nodename == "Enable" && xmlreader.isStartElement())
                    {
                        m_autoLogin.enable = xmlreader.readElementText().toInt()?true:false;
                        qDebug(u8"Enable:[%d]", xmlreader.readElementText().toInt());
                    }
                }
            }
        }
    }
    if(xmlreader.hasError()) {
        qWarning("errorInfo:[%s], line:[%lld], column:[%lld], offset:[%lld]", qPrintable(xmlreader.errorString()), xmlreader.lineNumber(), xmlreader.columnNumber(), xmlreader.characterOffset());
    }
    xmlFile.close();
    return 0;
}

void LoginWindow::on_pushButton_login_clicked()
{
    if(ui->lineEdit_name->text().isEmpty() || ui->lineEdit_password->text().isEmpty()) {
        QMessageBox::critical(NULL, u8"输入错误", u8"用户名和密码不能为空");
        return;
    }

    bool bRet = false;
    m_loginExtraData = QInputDialog::getText(this, u8"信息获取", u8"请输入资源代码", QLineEdit::Normal, m_loginExtraData, &bRet);
    if (bRet) {
        m_loginInfo.name = ui->lineEdit_name->text();
        m_loginInfo.password = ui->lineEdit_password->text();

        if(ui->checkBox_save->isChecked()) {
            if(ui->checkBox_auto->isChecked()) {
                m_autoLogin.userID = 1;
                m_autoLogin.enable = true;
            }else{
                m_autoLogin.userID = 0;
                m_autoLogin.enable = false;
            }
            if(0!=loginInfoSave()) {
                qCritical("login info save failed!");
            }
        }
        emit loginInfoVerification(m_loginInfo, m_loginExtraData);
    }
}

void LoginWindow::on_pushButton_clear_clicked()
{
    ui->lineEdit_name->clear();
    ui->lineEdit_password->clear();
}

void LoginWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
        this->on_pushButton_login_clicked();
    }
}

void LoginWindow::on_checkBox_save_stateChanged(int arg1)
{
    if(!arg1) {
        ui->checkBox_auto->setChecked(false);
    }
}

void LoginWindow::on_checkBox_auto_stateChanged(int arg1)
{
    if(arg1) {
        ui->checkBox_save->setChecked(true);
    }
}

