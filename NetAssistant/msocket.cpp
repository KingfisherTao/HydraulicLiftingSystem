#include "msocket.h"
#include "mserver.h"
#include "myhelper.h"

MSocket::MSocket(QObject *parent)
{
    m_tcpServer=static_cast<MServer*>(parent);
}

MSocket::~MSocket()
{
    qDebug()<<"释放Socket,所在线程："<<QThread::currentThreadId();
}

void MSocket::deal_readyRead()
{
    MSocket* tcpsocket=static_cast<MSocket*>(sender());
    // 获取客户端IP地址,端口号
    QString ip = tcpsocket->peerAddress().toString();
    quint16 port = tcpsocket->peerPort();
    // 获取客户端发来的数据
    QByteArray ba = tcpsocket->readAll();
    QString data;
    // hex接收
    myHelper::ByteToHexString(data,ba);

    // 数据打包
    qDebug()<<"读取数据线程："<<QThread::currentThreadId();
    data = QString("[%1|%2:%3]：%4").arg(QTime::currentTime().toString("hh:mm:ss.zzz")).arg(ip).arg(port).arg(data);
    // 发送到UI线程显示
    emit AddMessage(data);
}

void MSocket::deal_write(QByteArray ba)
{
    write(ba);
}

void MSocket::deal_disconnect()
{
    MSocket* tcpsocket=static_cast<MSocket*>(sender());
    // 断开socket
    tcpsocket->abort();
    // 消息提示断开
    QString ip = tcpsocket->peerAddress().toString();
    quint16 port = tcpsocket->peerPort();
    QString message = QString("[%1:%2] 已断开").arg(ip).arg(port);
    // 发送到UI线程显示
    emit AddMessage(message);
    // 断开所有信号连接
    tcpsocket->disconnect();
    // 发送到UI线程移除信息
    emit sockethelper->RemoveList(tcpsocket);
    // 释放
    tcpsocket->deleteLater();
}
