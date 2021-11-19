#ifndef _SERIALPORT_H_
#define _SERIALPORT_H_

typedef struct
{
	UINT m_nPort;
	UINT m_nBaud;
	UINT m_nCheck;
	UINT m_nData;
	UINT m_nStop;
}PortState;

BOOL GetMyByte(BYTE *pBuff, int buflen, int *Value);
BOOL InitComm(PortState portstate);
//BOOL InitComm(int Port, int BaudRate, int DataBits, int StopBits, int Parity);
LONG OnReceiveEvent(HANDLE hCom, DWORD timeout);
//LONG OnSendEvent(HANDLE hCom, BYTE *bytContent, DWORD dwSendLen);
LONG OnSendEvent(HANDLE hCom, LPCVOID bytContent, DWORD dwSendLen);

void SetSerialPort(HWND hWnd);
DWORD ComThreadProcEvent(LPVOID pParam);
BOOL ClosePort(HANDLE hCom);
void OnSaveSetting(HWND hdlg);
PortState GetPortState(HWND hdlg);

void AppendText( HWND hwndOutput , TCHAR *newText );

void OnAutoSend();
DWORD AutoSendThread(LPVOID pParam);

void CreateConfig();

extern BYTE sndbuf[8];
extern HANDLE hCom;
extern bool bEventRun;
extern bool bHexShow;
extern bool bHexSend;
extern bool bAutoSend;
extern bool bBeComm;

//≈‰÷√Œƒº˛
extern TCHAR szIni[MAX_PATH];
extern TCHAR szSection[MAX_PATH];
extern TCHAR szBuffer[8];

#endif