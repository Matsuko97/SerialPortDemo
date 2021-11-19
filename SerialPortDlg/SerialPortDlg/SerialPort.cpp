#include "include.h"

HANDLE hCom;

HANDLE hThreadEvent;
bool bEventRun = false;
bool bBeComm = false;
bool bHexShow = false;
bool bHexSend = false;
bool bAutoSend = false;

HWND hWnd;
DWORD dwThreadID;
OVERLAPPED Eol={0};
OVERLAPPED Wol={0};
OVERLAPPED Rol={0};
bool fStopMsg;
int val;
TCHAR info[144];
TCHAR myHex[16];
BYTE myByte[50];

//配置文件
TCHAR szIni[MAX_PATH] = {0};
TCHAR szSection[MAX_PATH] = TEXT("串口控制");
TCHAR szBuffer[8] = TEXT("");

BYTE sndbuf[8] = {0x01, 0x04, 0x00, 0x00, 0x00, 0x02, 0x71, 0xCB}; 

BOOL GetMyByte(BYTE *pBuff, int buflen, int *Value)
{
	if (pBuff == NULL || buflen < 9)
	{
		return FALSE;                                  //如果缓冲区为空或者字节数不够，返回false
	}

	*Value = pBuff[3]<<8 | pBuff[4];              //将数据从缓冲区中取出
	return TRUE;

}

//BOOL InitComm(int Port, int BaudRate, int DataBits, int StopBits, int Parity)
BOOL InitComm(PortState portstate)
{

	TCHAR PortStr[16];
	if (portstate.m_nPort < 10)
	{
		_stprintf_s(PortStr, 16, TEXT("COM%d"), portstate.m_nPort);
	}
	else
	{
		_stprintf_s(PortStr, 16, TEXT("\\\\.\\COM%d"), portstate.m_nPort);
	}

	hCom = CreateFile(PortStr,
		GENERIC_READ |GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL |FILE_FLAG_OVERLAPPED,
		NULL
		);

	if(hCom != INVALID_HANDLE_VALUE){

		SetupComm(hCom,1024,512);

		COMMTIMEOUTS TimeOuts; //设定读超时
		TimeOuts.ReadIntervalTimeout=1000;
		TimeOuts.ReadTotalTimeoutMultiplier=500;
		TimeOuts.ReadTotalTimeoutConstant=5000; //设定写超时
		TimeOuts.WriteTotalTimeoutMultiplier=500;
		TimeOuts.WriteTotalTimeoutConstant=2000;

		SetCommTimeouts(hCom,&TimeOuts); //设置超时

		DCB myDCB;
		GetCommState(hCom,&myDCB);
		myDCB.BaudRate = portstate.m_nBaud;   // 设置波特率9600  
		myDCB.fBinary = TRUE; // 设置二进制模式，此处必须设置TRUE  
		myDCB.fParity = FALSE; // 支持奇偶校验  
		myDCB.fOutxCtsFlow = FALSE;  // No CTS output flow control  
		myDCB.fOutxDsrFlow = FALSE;  // No DSR output flow control  
		myDCB.fDtrControl = DTR_CONTROL_DISABLE; // No DTR flow control  
		myDCB.fDsrSensitivity = FALSE; // DSR sensitivity  
		myDCB.fTXContinueOnXoff = TRUE; // XOFF continues Tx  
		myDCB.fOutX = FALSE;     // No XON/XOFF out flow control  
		myDCB.fInX = FALSE;        // No XON/XOFF in flow control  
		myDCB.fErrorChar = FALSE;    // Disable error replacement  
		myDCB.fNull = FALSE;  // Disable null stripping  
		myDCB.fRtsControl = RTS_CONTROL_DISABLE;   //No RTS flow control  
		myDCB.fAbortOnError = FALSE;  // 当串口发生错误，并不终止串口读写  
		myDCB.ByteSize = portstate.m_nData;   // 数据位,范围:4-8  
		myDCB.Parity = portstate.m_nCheck; // 校验模式  
		myDCB.StopBits = portstate.m_nStop;   // 1位停止位  

		SetCommState(hCom,&myDCB);

		PurgeComm(hCom, PURGE_TXABORT| PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR); 
	}
	else
	{
		MessageBox(NULL, TEXT("Open Serial Fail"), TEXT("Error"), MB_ICONERROR);
		return FALSE ;
	}
	return TRUE;
}

//LONG OnSendEvent(HANDLE hCom, BYTE *bytContent, DWORD dwSendLen)
LONG OnSendEvent(HANDLE hCom, LPCVOID bytContent, DWORD dwSendLen)
{
	DWORD dwActlen = 0;
	DWORD dwErrorFlags;
	COMSTAT ComStat;

	ClearCommError(hCom,&dwErrorFlags,&ComStat);

	Wol.hEvent = CreateEvent(NULL,
		TRUE,
		FALSE,
		NULL );
	BOOL bResult ;

	bResult = WriteFile(hCom,
		bytContent,
		dwSendLen, 
		&dwActlen,
		&Wol );

	DWORD dwRes, dwWrite;

	if(!bResult)
	{        dwRes = WaitForSingleObject ( Wol.hEvent , 5000 );   //5ms超时
	switch(dwRes)
		{
		case   WAIT_OBJECT_0:
			if (!GetOverlappedResult (hCom,
				&Wol,
				&dwWrite,   
				TRUE) )     //TRUE表示直到操作完成函数才返回
			{
				MessageBox(NULL, TEXT("发送指令失败"), TEXT("Error"), MB_ICONERROR);
				//操作失败,可以使用GetLastError()获取错误信息
			}
			else
			{
#if 1
				//发送数据成功
				_stprintf_s(info, _countof(info), TEXT("send success dwWrite = %d\r\n"), dwWrite); 
				AppendText( GetDlgItem(hComDlg, IDC_EDITREC),  info );
				//SetWindowText(GetDlgItem(hComDlg, IDC_EDITREC), info);
#endif
			}
		}
	}

	bBeComm = true;

	return 0;
}

LONG OnReceiveEvent(HANDLE hCom, DWORD timeout)
{
	DWORD dwRes;
	DWORD dwErrors;
	DWORD dwReadLen;
	COMSTAT ComStat;
	fStopMsg = true;

	BOOL bResult ;

	memset(myByte,0,_countof(myByte));
	memset(info , 0 ,_countof(info));

	ClearCommError(hCom,
		&dwErrors,
		&ComStat );

	DWORD dwWantLen = ComStat.cbInQue;
	if(dwWantLen == 0)	return 0;

	bResult = ReadFile(hCom,
		&myByte,
		dwWantLen,
		&dwReadLen,
		&Rol);

	if(bResult){
		for (int i = 0; i < (int)dwWantLen; i++)
		{
			if(bHexShow)
			{
				_stprintf_s(myHex, _countof(myHex), TEXT("%02X "), myByte[i]); 
			}
			else
			{
				_stprintf_s(myHex, _countof(myHex), TEXT("%c "), myByte[i]); 
			}
			_tcscat_s(info ,_countof(info), myHex);
		}
		_tcscat_s( info, _countof(info),TEXT("\r\n")); 
		AppendText( GetDlgItem(hComDlg, IDC_EDITREC),  info );

#if 1
		if(GetMyByte(myByte, dwReadLen, &val))
		{
			val = (val > 32767) ? (val - 65535 - 1):val;                    //整型转化为小数
			double dbval = (val)*0.01;                                            //用于亨立德电子公司的PT100采集器
			_stprintf_s(info, _countof(info), TEXT("当前温度 = %.3lf\r\n"), dbval); //将计算得到的小数值放入pHinfo字符串中
			AppendText( GetDlgItem(hComDlg, IDC_EDITREC),  info );
			//SetWindowText(GetDlgItem(hWnd, IDC_EDITREC), info);
		}
#endif

	}

	else{

		Rol.hEvent = CreateEvent(NULL,
			TRUE,
			FALSE,
			NULL );

		dwRes = WaitForSingleObject(Rol.hEvent,
			timeout );

		switch(dwRes){

			case WAIT_OBJECT_0:
				bResult = GetOverlappedResult(hCom,
					&Rol,
					&dwReadLen,
					TRUE  );
				if(!bResult){

					MessageBox(NULL, TEXT("读取失败"), TEXT("Error"), MB_ICONERROR);

				}
				else{

#if 1
					if(GetMyByte(myByte, dwReadLen, &val))
					{
						val = (val > 32767) ? (val - 65535 - 1):val;                    //整型转化为小数
						double dbval = (val)*0.01;                                            //用于亨立德电子公司的PT100采集器
						_stprintf_s(info, _countof(info), TEXT("当前温度 = %.3lf\n\r"), dbval); //将计算得到的小数值放入pHinfo字符串中
						AppendText( GetDlgItem(hComDlg, IDC_EDITREC),  info );
					}
#endif
				}

				break;

			case WAIT_TIMEOUT:
				break;

			default:
				break ;
		}
	}

	fStopMsg = false ;
	//PurgeComm(hCom, PURGE_TXABORT| PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR); 
	CloseHandle(Rol.hEvent);

	return 0;
}

void SetSerialPort(HWND hWnd)
{
	DWORD dwParam;
	PortState portstate = GetPortState(hWnd);

	if(InitComm(portstate)){
		if(!SetCommMask(hCom, EV_RXCHAR | EV_TXEMPTY))
		{
			MessageBox(NULL, TEXT("建立事件掩码失败"), TEXT("Error"), MB_ICONERROR);
		}

		hThreadEvent = CreateThread((LPSECURITY_ATTRIBUTES)NULL, 0,
			(LPTHREAD_START_ROUTINE)ComThreadProcEvent, &dwParam, 0, &dwThreadID);

		if (hThreadEvent == INVALID_HANDLE_VALUE)
		{
			MessageBox(NULL, TEXT("Create Thread Fail"), TEXT("Error"), MB_ICONERROR);
			return;
		}
		bEventRun = true ;
	}
}

DWORD ComThreadProcEvent(LPVOID pParam)
{
	DWORD dwEvtMask;
	DWORD dwRes;

	Eol.hEvent = CreateEvent(NULL,
		TRUE,
		FALSE,
		NULL );

	while(bEventRun)
	{
		WaitCommEvent(hCom, 
			&dwEvtMask,
			&Eol);
		dwRes = WaitForSingleObject(Eol.hEvent,100);
		switch(dwRes){

			case WAIT_OBJECT_0:

				switch(dwEvtMask){

			case EV_RXCHAR:
				Sleep(10);
				OnReceiveEvent(hCom, 5000);
				break;

			case EV_TXEMPTY:

				break ;
				}

				break;
		}
	}
	ClosePort(hCom);
	return TRUE;
}

BOOL ClosePort(HANDLE hCom)
{
	if (hCom == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	SetCommMask(hCom, 0);        
	PurgeComm(hCom, PURGE_TXCLEAR|PURGE_RXCLEAR);    /* 清除收/发缓冲 */
	CloseHandle(hCom);                                /* 关闭串口操作句柄 */

	return TRUE;
}

void AppendText( HWND hwndOutput , TCHAR *newText )
{
	// get edit control from dialog
	// get new length to determine buffer size
	int outLength = GetWindowTextLength( hwndOutput ) + lstrlen(newText) + 1;

	// create buffer to hold current and new text
	TCHAR * buf = ( TCHAR * ) GlobalAlloc( GPTR, outLength * sizeof(TCHAR) );
	if (!buf) return;

	// get existing text from edit control and put into buffer
	GetWindowText( hwndOutput, buf, outLength );

	// append the newText to the buffer
	_tcscat_s( buf, outLength, newText );

	// Set the text in the edit control
	SetWindowText( hwndOutput, buf );

	//限制edit控件中的行数
	int iLineNum=SendMessage(hwndOutput, EM_GETLINECOUNT, 0, 0);
	if(iLineNum>=18)
	{
		SetWindowText(hwndOutput,TEXT(""));
	}

	// free the buffer
	GlobalFree( buf );
}


//做成配置文件方便实用
void OnSaveSetting(HWND hdlg)
{
	PortState lastPortState = GetPortState(hdlg);

	_stprintf_s(szBuffer, _countof(szBuffer), TEXT("COM%d"), lastPortState.m_nPort); 
	WritePrivateProfileString(szSection,TEXT("ComPort"),szBuffer,szIni);

	_stprintf_s(szBuffer, _countof(szBuffer), TEXT("%d"), lastPortState.m_nBaud); 
	WritePrivateProfileString(szSection,TEXT("BaudRate"),szBuffer,szIni);

	_stprintf_s(szBuffer, _countof(szBuffer), TEXT("%d"), lastPortState.m_nData); 
	WritePrivateProfileString(szSection,TEXT("DataBits"),szBuffer,szIni);

	GetDlgItemText(hdlg,IDC_STOP,szBuffer,8);
	WritePrivateProfileString(szSection,TEXT("StopBits"),szBuffer,szIni);

	GetDlgItemText(hdlg,IDC_PARITY,szBuffer,8);
	WritePrivateProfileString(szSection,TEXT("Parity"),szBuffer,szIni);

}

PortState GetPortState(HWND hdlg)
{
	PortState portstate = {0};
	TCHAR PortName[8];

	BOOL flag;
	GetDlgItemText(hdlg, IDC_COMNUM, PortName, sizeof(PortName));                    //从窗口控件中获取信息放到PortName字符串中
	_stscanf_s(PortName, TEXT("COM%d"), &portstate.m_nPort);                                   //从PortName字符串中获取串口号

	portstate.m_nBaud = GetDlgItemInt(hdlg,IDC_BAUD,&flag,TRUE);
	portstate.m_nData = GetDlgItemInt(hdlg,IDC_BITS,&flag,TRUE);

	portstate.m_nStop = SendMessage(GetDlgItem(hdlg, IDC_STOP), CB_GETCURSEL, 0, 0);
	portstate.m_nCheck = SendMessage(GetDlgItem(hdlg, IDC_PARITY), CB_GETCURSEL, 0, 0);

	//GetDlgItemText(hdlg,IDC_COMNUM,portstate.m_nPort,8);
	//GetDlgItemText(hdlg,IDC_BAUD,portstate.m_nBaud,8);
	//GetDlgItemText(hdlg,IDC_BITS,portstate.m_nData,8);
	//GetDlgItemText(hdlg,IDC_STOP,portstate.m_nStop,8);
	//GetDlgItemText(hdlg,IDC_PARITY,portstate.m_nCheck,8);

	return portstate;
}

void OnAutoSend()
{
	if (!bEventRun)
	{
		MessageBox(NULL, TEXT("串口未打开"), TEXT("Error"), MB_ICONERROR);
		return;
	}

	DWORD ThreadId;
	DWORD dwParam;
	HANDLE hThread=CreateThread((LPSECURITY_ATTRIBUTES)NULL,
		(DWORD)0,
		(LPTHREAD_START_ROUTINE )AutoSendThread,
		(LPVOID)&dwParam,
		(DWORD)0,
		(LPDWORD)&ThreadId);

	if(hThread==NULL)
	{
		MessageBox(NULL, TEXT("Create Thread Fail"), TEXT("Error"), MB_ICONERROR);
	}	
	return;
}

DWORD AutoSendThread(LPVOID pParam)
{

	DWORD dwPeriod = GetDlgItemInt( hComDlg , IDC_INTERVAL , NULL , TRUE );

	if( 0 == dwPeriod )	return FALSE;

	while(bAutoSend)
	{
		if(bHexSend)
		{
			GetDlgItemText(hComDlg,IDC_EDITSEND,sSendData,_countof(sSendData));

			dwHexDataLen = Str2Hex(sSendData , phSendData);
			OnSendEvent(hCom, phSendData ,dwHexDataLen);
		}
		else
		{
			GetDlgItemText(hComDlg,IDC_EDITSEND,sSendData,_countof(sSendData));

			OnSendEvent(hCom, sSendData,_countof(sSendData)); 
		}
		Sleep(dwPeriod);
	}
	return TRUE;
}

void CreateConfig()
{
	GetModuleFileName(NULL,szIni,sizeof(szIni)/sizeof(TCHAR));
	TCHAR* pDot = _tcsrchr(szIni,'.');
	if (pDot != NULL)
	{
		*pDot = '\0';
		_tcscat_s(szIni , _countof(szIni), TEXT(".ini"));
	}
	return;
}