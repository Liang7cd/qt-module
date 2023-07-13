#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDateTime>
#include "outlog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QString versionInfo, QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    const QString m_versionInfo;
};
#endif // MAINWINDOW_H
