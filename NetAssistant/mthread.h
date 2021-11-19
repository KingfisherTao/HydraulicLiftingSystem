#ifndef MTHREAD_H
#define MTHREAD_H

#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")
#endif

#include <QThread>
#include <QTime>

class MServer;
class MSocket;

// Socket创建辅助类
class SocketHelper:public QObject
{
    Q_OBJECT
public:
    explicit SocketHelper(QObject *parent = nullptr);
    MServer* m_server;
public slots:
    // 创建socket
    void CreateSocket(qintptr socketDescriptor,int index);
signals:
    // 创建
    void Create(qintptr socketDescriptor,int index);
    // 添加信息
    void AddList(MSocket* tcpsocket,int index);
    // 移除信息
    void RemoveList(MSocket* tcpsocket);
};

// 子线程类
class MThread : public QThread
{
    Q_OBJECT
public:
    explicit  MThread(QObject *parent);
    ~MThread() override;
public:
    MServer* m_server;
    SocketHelper* sockethelper;
    int ThreadLoad;
    void run() override;
};

#endif // MTHREAD_H
