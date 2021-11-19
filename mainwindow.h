#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>
#include <QTimer>
#include <QUdpSocket>

#include "NetAssistant/mserver.h"
#include "NetAssistant/msocket.h"

#define CONST_TARGET_IP "192.168.1.100"
#define CONST_TARGET_PORT 2000
#define CONST_LOCAL_PORT 2001
#define AppName "升降系统控制程序"

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
    void UdpReadData();

    void on_Btn_Bind_clicked();
    void on_Btn_Start_clicked();

private:
    void initUI();
    void initConnect();
    void showStatusMessage(const QString &message);
    void updateInfo(QStringList info1, QStringList info2);

    Ui::MainWindow *ui;
    QUdpSocket* m_udpSocket;

    QLabel *m_status = nullptr;         // 窗口状态栏
};
#endif // MAINWINDOW_H
