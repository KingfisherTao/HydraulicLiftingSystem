#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include "myhelper.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_status(new QLabel)
{
    ui->setupUi(this);

    m_udpSocket = nullptr;

    initUI();
    initConnect();
}

MainWindow::~MainWindow()
{
    delete ui;

    if(m_udpSocket!=nullptr)
    {
        m_udpSocket->abort();
        delete m_udpSocket;
    }
}

void MainWindow::initUI()
{
    setWindowTitle(AppName + QString("    本地 IP: %1 监听端口: %2").arg(myHelper::GetLocalIP()).arg(CONST_LOCAL_PORT));
    ui->statusbar->addWidget(m_status);
    setWindowFlags(Qt::WindowCloseButtonHint);
    setFixedSize(1035, 632);
    ui->Lab_IP->setText(QString("目标 IP :  %1:%2").arg(CONST_TARGET_IP).arg(CONST_TARGET_PORT));
}

void MainWindow::initConnect()
{

}

// udp接收消息
void MainWindow::UdpReadData()
{
    // 获取发来的数据
    QByteArray ba;
    while (m_udpSocket->hasPendingDatagrams())
    {
        // 等待接收的字节数
        int sizeLen=static_cast<int>(m_udpSocket->pendingDatagramSize());
        ba.resize(sizeLen);
        // 读取
        m_udpSocket->readDatagram(ba.data(),ba.size());
    }

    QString data;
    // hex
    myHelper::ByteToHexString(data,ba);

    // 显示
    //qDebug() << ba.length() << data;
    QStringList _list;
    QByteArray _ba = ba.mid(4, 20);

    for (int i = 0; i < _ba.length(); i++)
    {
        unsigned char n =static_cast<unsigned char>((_ba.at(i)));
        _list.append(QByteArray::number(n, 16).toUpper());
    }
    QString _strState = QString("%1%2%3%4").arg(_list[0]).arg(_list[1]).arg(_list[2]).arg(_list[3]);
    QString _strLift = QString("%1%2%3%4").arg(_list[4]).arg(_list[5]).arg(_list[6]).arg(_list[7]);
    QString _strPitch = QString("%1%2%3%4").arg(_list[8]).arg(_list[9]).arg(_list[10]).arg(_list[11]);
    QString _strRota = QString("%1%2%3%4").arg(_list[12]).arg(_list[13]).arg(_list[14]).arg(_list[15]);

    //qDebug() << "_strState = " << _strState <<"_strLift = " << _strLift << " _strPitch = " << _strPitch << " _strRota = " << _strRota;

    auto _statLift = myHelper::Hex2Bin(_list[0]);
    auto _statPitch = myHelper::Hex2Bin(_list[1]);
    auto _statRota = myHelper::Hex2Bin(_list[2]);
    auto _statSystem = myHelper::Hex2Bin(_list[3]);

    //qDebug() << "升降状态: " << _statLift;
    //qDebug() << "俯仰状态: " << _statPitch;
    //qDebug() << "旋转状态: " << _statRota;
    //qDebug() << "系统状态: " << _statSystem;

    //ui->LE_State->setText(QString::number(qint32(_strState.toUInt(nullptr,16))));
    // 升降状态
    ui->LE_UpperLimit->setText(_statLift.at(7));
    ui->LE_LowerLimit->setText(_statLift.at(6));
    ui->LE_InverterFaults->setText(_statLift.at(5));


    // 俯仰状态
    ui->LE_TiltUp->setText(_statPitch.at(7));
    ui->LE_TiltDown->setText(_statPitch.at(6));
    ui->LE_Lockpin->setText(_statPitch.at(5));
    ui->LE_ErrorPitchPointMove->setText(_statPitch.at(4));
    ui->LE_ErrorPitchLocation->setText(_statPitch.at(3));

    // 旋转状态
    ui->LE_TurnLift->setText(_statRota.at(7));
    ui->LE_TurnRight->setText(_statRota.at(6));
    ui->LE_ErrorRotaPointMove->setText(_statRota.at(5));
    ui->LE_ErrorRotaLocation->setText(_statRota.at(4));

    // 系统状态
    ui->LE_OnSiteStop->setText(_statSystem.at(7));
    ui->LE_MachineStop->setText(_statSystem.at(6));

    ui->LE_StateLift->setText(QString::number(qint32(_strLift.toUInt(nullptr,16))));
    ui->LE_StatePitch->setText(QString::number(qint32(_strPitch.toUInt(nullptr,16))));
    ui->LE_StateRota->setText(QString::number(qint32(_strRota.toUInt(nullptr,16))));
}

void MainWindow::on_Btn_Bind_clicked()
{
    // 解绑udp
    if(m_udpSocket!=nullptr)
    {
        // 断开信号槽
        m_udpSocket->disconnect();
        // 关闭udp
        m_udpSocket->abort();
        // 释放
        m_udpSocket->deleteLater();
        m_udpSocket=nullptr;
        // 更新UI
        showStatusMessage(QString("<font color=red>UDP已解绑 %1 : %2</font>").arg(CONST_TARGET_IP).arg(CONST_TARGET_PORT));

        ui->Btn_Bind->setText("绑定");
        ui->Btn_Bind->setIcon(QIcon(":/Resource/Img/start36x36.png"));
    }
    // 绑定端口
    else
    {
        // 获取本地端口
        quint16 port = CONST_LOCAL_PORT;
        bool ok;
        // 获取本地Ip
        QHostAddress localip(myHelper::GetLocalIP());
        localip.toIPv4Address(&ok);
        m_udpSocket=new QUdpSocket(this);
        // 绑定端口
        ok = m_udpSocket->bind(localip,port,QAbstractSocket::ShareAddress);
        // 连接接收信号槽
        connect(m_udpSocket,SIGNAL(readyRead()),this,SLOT(UdpReadData()));
        // 更新UI
        showStatusMessage(QString("<font color=forestgreen>UDP已绑定 %1 : %2</font>").arg(CONST_TARGET_IP).arg(CONST_TARGET_PORT));
        ui->Btn_Bind->setText("解绑");
        ui->Btn_Bind->setIcon(QIcon(":/Resource/Img/stop36x36.png"));
    }
}

void MainWindow::on_Btn_Start_clicked()
{
    qDebug() << "Start";
    QByteArray byteArray;
    // 控制双子
    // 高度控制 0000 0/1 0/1 0/1 0/1
    //              复位控制 升降确认 点动下降 点动上升
    QString _strLiftCtrl = QString("0000%1%2%3%4").arg(ui->CB_LiftResetCtrl->isChecked()).arg(ui->CB_LiftConfirm->isChecked())
            .arg(ui->CB_PMDown->isChecked()).arg(ui->CB_PMUp->isChecked());
    QString _strHexLiftCtrl = QString("%1").arg(_strLiftCtrl.toInt(nullptr, 2),2,16,QLatin1Char('0')).toUpper();
    qDebug() << "高度控制 : "<<_strLiftCtrl << " >> " << _strHexLiftCtrl;

    // 俯仰控制 000 0/1 0/1 0/1 0/1 0/1
    //              回原点控 复位控制 定位确认 点动下俯 点动上俯
    QString _strPitchCtrl = QString("000%1%2%3%4%5").arg(ui->CB_PitchReturnToOrigin->isChecked()).arg(ui->CB_PitchResetCtrl->isChecked())
            .arg(ui->CB_PitchPositionConfirm->isChecked()).arg(ui->CB_PMLookDown->isChecked())
            .arg(ui->CB_PMLookUp->isChecked());
    QString _strHexPitchCtrl = QString("%1").arg(_strPitchCtrl.toInt(nullptr, 2),2,16,QLatin1Char('0')).toUpper();
    qDebug() << "俯仰控制 : "<<_strPitchCtrl << " >> " << _strHexPitchCtrl;

    // 旋转控制 000 0/1 0/1 0/1 0/1 0/1
    //              回原点控 复位控制 定位确认 点动反行 点动正行
    QString _strRotaCtrl = QString("000%1%2%3%4%5").arg(ui->CB_RotaReturnToOrigin->isChecked()).arg(ui->CB_RotaResetCtrl->isChecked())
            .arg(ui->CB_RotaPositionConfirm->isChecked()).arg(ui->CB_PMReversal->isChecked())
            .arg(ui->CB_PMForward->isChecked());
    QString _strHexRotaCtrl = QString("%1").arg(_strRotaCtrl.toInt(nullptr, 2),2,16,QLatin1Char('0')).toUpper();
    qDebug() << "旋转控制 : "<<_strRotaCtrl << " >> " << _strHexRotaCtrl;

    // 状态控制 0000000 0/1
    //                  急停输入
    QString _strStatCtrl = QString("0000000%1").arg(ui->CB_StopInput->isChecked());
    QString _strHexStatCtrl = QString("%1").arg(_strStatCtrl.toInt(nullptr, 2),2,16,QLatin1Char('0')).toUpper();
    qDebug() << "状态控制 : "<<_strStatCtrl << " >> " << _strHexStatCtrl;

    // 升降点动速度
    QString _strLiftPointMoveSpeed = myHelper::Dec2Hex(ui->LE_LiftPMSpeed->text());
    qDebug () << "升降点动速度 : " <<_strLiftPointMoveSpeed;
    // 俯仰点动速度 取值
    QString _strPitchPointMoveSpeed = myHelper::Dec2Hex(ui->LE_PitchPMSpeed->text());
    qDebug () << "俯仰点动速度 : " <<_strPitchPointMoveSpeed;
    // 旋转点动速度 取值
    QString _strRotaPointMoveSpeed = myHelper::Dec2Hex(ui->LE_RotaPMSpeed->text());
    qDebug () << "旋转点动速度 : "<<_strRotaPointMoveSpeed;
    // 高度位置 取值
    QString _strLift = myHelper::Dec2Hex(ui->LE_Lift->text());
    qDebug () << "高度位置 : "<< _strLift;
    // 俯仰位置 取值
    QString _strPitch = myHelper::Dec2Hex(ui->LE_Pitch->text());
    qDebug () << "俯仰位置 : "<< _strPitch;
    // 旋转位置 取值
    QString _strRota = myHelper::Dec2Hex(ui->LE_Rota->text());
    qDebug () << "旋转位置 : " <<_strRota;
    // 俯仰定位速度 取值
    QString _strPitchPositionSpeed = myHelper::Dec2Hex(ui->LE_PitchPositionSpeed->text());
    qDebug () << "俯仰定位速度 : " <<_strPitchPositionSpeed;
    // 旋转定位速度 取值
    QString _strRotaPositionSpeed = myHelper::Dec2Hex(ui->LE_RotaPositionSpeed->text());
    qDebug () << "旋转定位速度 : "<<_strRotaPositionSpeed;

    // hex字符串转字节
    QString sendData;
    sendData.append(_strHexLiftCtrl);
    sendData.append(_strHexPitchCtrl);
    sendData.append(_strHexRotaCtrl);
    sendData.append(_strHexStatCtrl);
    sendData.append(_strLiftPointMoveSpeed);
    sendData.append(_strPitchPointMoveSpeed);
    sendData.append(_strRotaPointMoveSpeed);
    sendData.append(_strLift);
    sendData.append(_strPitch);
    sendData.append(_strRota);
    sendData.append(_strPitchPositionSpeed);
    sendData.append(_strRotaPositionSpeed);
    qDebug() << sendData.length() << " " << sendData;

    if(!myHelper::HexStringToByte(sendData,byteArray))
    {
        QMessageBox::information(this,"提示","输入的十六进制字符串有误，请重新输入");
        return;
    }

    // 单播
    // 获取远程Ip
    QHostAddress remoteip(CONST_TARGET_IP);
    // 获取远程端口
    quint16 port=CONST_TARGET_PORT;
    remoteip.toIPv4Address();

    if (m_udpSocket)
        // 发送
        m_udpSocket->writeDatagram(byteArray,remoteip,port);
    else
        qDebug() << "udpSocket is Null";
}

void MainWindow::showStatusMessage(const QString &message)
{
    m_status->setText(message);
}
