#ifndef MSOCKET_H
#define MSOCKET_H
#include <QTcpSocket>
#include "mthread.h"

class MServer;

class MSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit MSocket(QObject *parent);
    ~MSocket();
    MServer* m_tcpServer;
    SocketHelper* sockethelper;
public slots:
    // 读取数据槽函数
    void deal_readyRead();
    // 断开连接槽函数
    void deal_disconnect();
    // 写入数据槽函数
    void deal_write(QByteArray ba);
signals:
    // 发送给UI显示
    void AddMessage(QString data);
    // UI发送过来数据
    void WriteMessage(QByteArray ba);
    // 主动关闭socket
    void DeleteSocket();
};

#endif // MSOCKET_H
