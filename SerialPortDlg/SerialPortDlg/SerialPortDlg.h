#ifndef _SERIALPORTDLG_H_
#define _SERIALPORTDLG_H_

typedef struct PORT{
	TCHAR PortName[8];
	struct PORT *next;
}Port,*PortInfo;


INT_PTR CALLBACK ComDlgProc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK About_MM_TabPage1DlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK About_MM_TabPage2DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

BOOL InitControls(HWND hwnd);
//int PortEnumAllComm(TCHAR *buf, int bufLen);
PortInfo PortEnumAllComm();

//¼ì²â´®¿ÚµÄÈÈ°Î²å
BOOL OnDeviceChange(UINT nEventType,DWORD dwData);
void UpdateDevice(PDEV_BROADCAST_DEVICEINTERFACE pDevInf, UINT nEventType);
void RegisterForDevChange(HWND hDlg);
void RefreshComm(HWND hwnd , UINT nEventType);
void RecoverState();

extern HWND hComDlg;
extern TCHAR sSendData[144];
extern int dwHexDataLen;
extern BYTE phSendData[144];

#endif