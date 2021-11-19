#include "mserver.h"
#include "msocket.h"

MServer::MServer(QObject *parent) :
    QTcpServer(parent)
{        
    // 在线程内创建对象，槽函数在这个线程中执行
    sockethelper=new SocketHelper(this);
    // 注册信号类型
    qRegisterMetaType<qintptr>("qintptr");
    // 连接信号和槽
    connect(sockethelper,&SocketHelper::Create,sockethelper,&SocketHelper::CreateSocket);
    connect(sockethelper,&SocketHelper::AddList,this,&MServer::AddInf);
    connect(sockethelper,&SocketHelper::RemoveList,this,&MServer::RemoveInf);
}

MServer::~MServer()
{
    // 释放所有socket
    while(list_information.count()>0)
    {
        emit list_information[0].socket->DeleteSocket();
        list_information.removeAt(0);
    }

    // 释放所有线程
    while(list_thread.count()>0)
    {
        list_thread[0]->quit();
        // 等待退出
        list_thread[0]->wait();
        // 释放
        list_thread[0]->deleteLater();
        list_thread.removeAt(0);
    }
    // UI线程里的sockethelper
    sockethelper->disconnect();
    delete sockethelper;
}

// 获取负载最少的线程索引
// -1:UI线程
int MServer::GetMinLoadThread()
{
    // 只有1个子线程
    if(list_thread.count()==1)
    {
        return 0;
    }
    // 多个子线程
    else if(list_thread.count()>1)
    {
        int minload=list_thread[0]->ThreadLoad;
        int index=0;
        for(int i=1;i<list_thread.count();i++)
        {
            if(list_thread[i]->ThreadLoad<minload)
            {
                index = i;
                minload=list_thread[i]->ThreadLoad;
            }
        }
        return index;
    }
    // 没有子线程
    return -1;
}

// 设置子线程数
void MServer::SetThread(int num)
{
    for(int i=0;i<num;i++)
    {
        // 新建线程
        list_thread.append(new MThread(this));
        // 线程负载初始0
        list_thread[i]->ThreadLoad = 0;
        list_thread[i]->start();
    }
}

// 在对应线程里创建新socket连接
void MServer::incomingConnection(qintptr socketDescriptor)
{
    // 获取负载最少的子线程索引
    int index = GetMinLoadThread();
    if( index!= -1)// 非UI线程时
    {
        // 交给子线程运行
        emit list_thread[index]->sockethelper->Create(socketDescriptor,index);
    }
    else
    {
        // 交给UI线程运行
        emit sockethelper->Create(socketDescriptor,index);
    }
}

// 注册元属性
Q_DECLARE_METATYPE(MSocket*)
// 添加socket信息
void MServer::AddInf(MSocket* socket,int index)
{
    SocketInformation inf;

    QString ip = socket->peerAddress().toString();
    quint16 port = socket->peerPort();
    QString str_inf = QString("[%1:%2]").arg(ip).arg(port);

    inf.str_inf=str_inf;
    inf.socket=socket;
    inf.threadIndex=index;
    list_information.append(inf);
}

// 移除socket信息
void MServer::RemoveInf(MSocket* socket)
{
    for (int i=0;i<list_information.count();i++)
    {
        if(list_information[i].socket == socket)
        {
            list_information.removeAt(i);
            break;
        }
    }
}
