#include "mthread.h"
#include "mserver.h"
#include "msocket.h"

MThread::MThread(QObject *parent)
{
    m_server=static_cast<MServer*>(parent);
    sockethelper=nullptr;
    ThreadLoad = 0;
}

MThread::~MThread()
{
    if(sockethelper!=nullptr)
    {
        sockethelper->disconnect();
        delete sockethelper;//释放sockethelper
    }
}

void MThread::run()
{
    //在线程内创建对象，槽函数在这个线程中执行
    sockethelper=new SocketHelper(m_server);
    connect(sockethelper,&SocketHelper::Create,sockethelper,&SocketHelper::CreateSocket);
    connect(sockethelper,&SocketHelper::AddList,m_server,&MServer::AddInf);
    connect(sockethelper,&SocketHelper::RemoveList,m_server,&MServer::RemoveInf);

    exec();
}

SocketHelper::SocketHelper(QObject *parent)
{
    m_server=static_cast<MServer*>(parent);
}

void SocketHelper::CreateSocket(qintptr socketDescriptor,int index)
{
    qDebug()<<"子线程:"<<QThread::currentThreadId();

    MSocket* tcpsocket = new MSocket(m_server);
    tcpsocket->sockethelper = this;
    // 初始化socket
    tcpsocket->setSocketDescriptor(socketDescriptor);
    // 发送到UI线程记录信息
    emit AddList(tcpsocket,index);
    // 非UI线程时
    if( index!= -1)
    {
        // 负载+1
        m_server->list_thread[index]->ThreadLoad+=1;//负载+1
        // 关联释放socket,非UI线程需要阻塞
        connect(tcpsocket , &MSocket::DeleteSocket , tcpsocket, &MSocket::deal_disconnect,Qt::ConnectionType::BlockingQueuedConnection);
    }
    else
    {
        connect(tcpsocket , &MSocket::DeleteSocket , tcpsocket, &MSocket::deal_disconnect,Qt::ConnectionType::AutoConnection);
    }

    // 发送消息
    connect(tcpsocket,&MSocket::WriteMessage,tcpsocket,&MSocket::deal_write);
    // 关联接收数据
    connect(tcpsocket , &MSocket::readyRead , tcpsocket , &MSocket::deal_readyRead);
    // 关联断开连接时的处理槽
    connect(tcpsocket , &MSocket::disconnected , tcpsocket, &MSocket::deal_disconnect);

    QString ip = tcpsocket->peerAddress().toString();
    quint16 port = tcpsocket->peerPort();
    QString message = QString("[%1:%2] 已连接").arg(ip).arg(port);
    // 发送到UI线程显示
    emit tcpsocket->AddMessage(message);
}
