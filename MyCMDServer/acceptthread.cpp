#include "acceptthread.h"
#include "mainwindow.h"
extern MainWindow* g_pWindow;
AcceptThread::AcceptThread(TCPServer& Server,iocpserver& IocpServer)
    :m_Server(Server),m_IocpServer(IocpServer)
{
    m_IocpServer.Init(m_Server);
    m_btArray.resize(8192);
}

AcceptThread::~AcceptThread()
{
    requestInterruption();
    terminate();
}

void AcceptThread::run()
{
    //������Ϣ
    QObject::connect(&m_Server,SIGNAL(sig_ClientOnline(QSharedPointer<ClientSession>)),g_pWindow,SLOT(slot_ClientOnline(QSharedPointer<ClientSession>)));

    //�����̵߳ȴ�����
    while(!isInterruptionRequested())
    {
        bool bRet = m_Server.AcceptConnect(&m_IocpServer);
        if(!bRet)
        {
            break;
        }
    }

}
