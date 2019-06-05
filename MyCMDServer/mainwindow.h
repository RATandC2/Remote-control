#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "tcpserver.h"
#include "acceptthread.h"
#include <QMainWindow>
#include <QSharedPointer>
#include <QMenu>
#include <QSettings>
#include <QNetworkInterface>
#include "QtNetwork/QHostAddress"
#include <QStringListModel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //��ȡ����ip
    QString get_localmachine_ip();

    virtual void timerEvent(QTimerEvent *event);

    //��ȡ����ʱ��
    void ShowData(QByteArray);

signals:
    void sig_SendData(QString strInfo);

    //��
private slots:

    //��������
    void on_startserver_clicked();
    //�رշ���
    void on_stopserver_clicked();

    //�Ҽ�
    void on_tableWidget_customContextMenuRequested(const QPoint &pos);

    //������Ϣ
    void slot_ClientOnline(QSharedPointer<ClientSession> ClientSession);

    //���ö˿�
    void on_lineEdit_2_returnPressed();

    //��������
    void slot_CreateDialog(int,int);

    //��������
    void slot_ShowRecvData(int, int, QByteArray);

    //�Ͽ�����
    void slot_Disconnect(int s);

    
    //����CMD����
    void slot_cmd_clicked(SOCKET, QString);

    //����ʱ�����
    void timerUpDate();

    //CMD��ť
    void on_cmdbutton_clicked();

    //��Ļ��ť
    void on_screenbutton_clicked();

    //���̰�ť
    void on_processbutton_clicked();

    //�ļ���ť
    void on_filebutton_clicked();

    //������ť
    void on_buiderbutton_clicked();

    //���ڰ�ť
    void on_aboutbutton_clicked();

private:
    Ui::MainWindow *ui;

    //AcceptThread
    QSharedPointer<AcceptThread> m_pThread;

    bool m_isStart;
    QSharedPointer<QMenu> m_menu;
    TCPServer m_Server;

    //���ö˿�
    QString m_Port;
    iocpserver m_IocpServer;
    int m_nRow;
};

#endif // MAINWINDOW_H



