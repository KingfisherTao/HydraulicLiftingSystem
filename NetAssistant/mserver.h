#ifndef MSERVER_H
#define MSERVER_H
#include <QTcpServer>
#include "mthread.h"

class MServer;

class SocketInformation
{
public:
    MSocket* socket;// socket指针
    QString str_inf;// ip端口字符串
    int threadIndex;// 所在线程ID
};

class MServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit MServer(QObject *parent = nullptr);
    ~MServer();
    // 设置线程数
    void SetThread(int num);
    // 获取当前最少负载的线程ID
    int GetMinLoadThread();

    // socket创建辅助对象
    SocketHelper* sockethelper;
    // 线程列表
    QList<MThread*> list_thread;
    // socket信息列表
    QList<SocketInformation> list_information;
private:
    void incomingConnection(qintptr socketDescriptor);
public slots:
    // 添加信息
    void AddInf(MSocket* socket,int index);
    // 移除信息
    void RemoveInf(MSocket* socket);
};
#endif // MSERVER_H
