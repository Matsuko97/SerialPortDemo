#include "include.h"

HWND hComDlg;
HICON hComIcon[2];

TCHAR sSendData[144];
TCHAR devInfo[144];
int dwHexDataLen;
BYTE phSendData[144];

//�Զ�����
HANDLE hThreadAuto;

INT_PTR CALLBACK ComDlgProc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_INITDIALOG:

		//״̬����ʼ��
		bHexSend = false;
		bEventRun = false;
		bHexShow = false;
		bBeComm = false;

		//��ʼ���ؼ�
		InitControls(hdlg);

		//���������ļ���
		CreateConfig();

		//ע���豸GUIDֵ
		RegisterForDevChange(hdlg);

		return 0;

	case WM_SYSCOMMAND:
		if(wParam == SC_CLOSE)
		{
			DestroyWindow(hdlg);
		}
		return 0;

	case WM_COMMAND:
		{
			int idDC = LOWORD(wParam);
			switch(idDC)
			{
			case IDC_BTNSEND:
				if(bEventRun)
				{
					if(bHexSend)
					{
						GetDlgItemText(hdlg,IDC_EDITSEND,sSendData,_countof(sSendData));

						dwHexDataLen = Str2Hex(sSendData , phSendData);
						OnSendEvent(hCom, phSendData ,dwHexDataLen);
					}
					else
					{
						GetDlgItemText(hdlg , IDC_EDITSEND , sSendData , _countof(sSendData));

						OnSendEvent(hCom , sSendData , _countof(sSendData)); 
					}

#if 0
					OnSendEvent(hCom, sndbuf , 8);
#endif
				}
				else
				{
					MessageBox(NULL, TEXT("����δ��"), TEXT("Error"), MB_ICONERROR);
				}
				break;

			case IDC_BTNOPENCOM:
				if(!bEventRun)
				{
					SetSerialPort(hdlg);
					OnSaveSetting(hdlg);
					if(bEventRun){
						SetWindowText(GetDlgItem(hdlg,IDC_BTNOPENCOM),TEXT("�رմ���"));
						(HICON)SendMessage(GetDlgItem(hdlg,IDC_COMICON), STM_SETIMAGE, IMAGE_ICON, LPARAM(hComIcon[1]));
					}
				}
				else
				{
					OnSaveSetting(hdlg);
					bEventRun = false;
					SetWindowText(GetDlgItem(hdlg,IDC_BTNOPENCOM),TEXT("�򿪴���"));
					(HICON)SendMessage(GetDlgItem(hdlg,IDC_COMICON), STM_SETIMAGE, IMAGE_ICON, LPARAM(hComIcon[0]));
				}
				break;

			case IDC_HEXSHOW:
				if(!bHexShow)
				{
					bHexShow = true;
				}
				else
				{
					bHexShow = false;
				}
				break;

			case IDC_HEXSEND:
				if(!bHexSend)
				{
					bHexSend = true;
				}
				else
				{
					bHexSend = false;
				}
				break;

			case IDC_BTNCLEARREC:
				SetWindowText(GetDlgItem(hdlg,IDC_EDITREC),TEXT(""));
				break;

			case IDC_BTNCLEARSEND:
				SetWindowText(GetDlgItem(hdlg,IDC_EDITSEND),TEXT(""));
				break;

			case IDC_BTNEX:
				hComEx = CreateDialog(hInst,MAKEINTRESOURCE(IDD_SERIALEX),hComDlg,(DLGPROC)ComExProc);
				ShowWindow(hComEx, SW_NORMAL);
				break;

			case IDC_AUTOSEND:
				if(!bAutoSend)
				{
					bAutoSend = true;
					OnAutoSend();
				}
				else
				{
					bAutoSend = false;
				}
				break;

			default:
				break;
			}
		}
		return 0;

	case WM_DEVICECHANGE:

		if(OnDeviceChange(wParam,lParam))
		{
			RefreshComm(hdlg,wParam);
			AppendText( GetDlgItem(hdlg, IDC_EDITREC),  devInfo );
		}

		return 0;

	case WM_CLOSE:
		DestroyWindow(hdlg);
		return 0;
	}
	return (INT_PTR)FALSE;
}

BOOL InitControls(HWND hwnd)
{
	//TCHAR PortName[144];
	//int count = PortEnumAllComm(PortName, _countof(PortName));                  //��õ�ǰ���ں�

	//SetDlgItemText(hwnd, IDC_COMNUM, PortName);                          //����Ͽ�ı�ͷ�����Ϣ���Իس����з���β
	//TCHAR *pCom = PortName;                                             //ָ���ַ�������λ

	//for (int i=0; i<count; ++i)
	//{
	//	SendMessage(GetDlgItem(hwnd,IDC_COMNUM), CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)pCom);   //��pComΪ�ף��Իس�����Ϊβ���ַ���
	//	pCom += _tcslen(pCom) + 1;                                    //����Ͽ�����һ��Ӵ��ں�
	//}

	PortInfo head = PortEnumAllComm();
	SetDlgItemText(hwnd, IDC_COMNUM, head->PortName);
	while(head)
	{
		SendMessage(GetDlgItem(hwnd,IDC_COMNUM), CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)head->PortName);
		head = head->next;
	}

	//������
	TCHAR *baudrate[] = {TEXT("2400"), TEXT("4800"), TEXT("9600"), TEXT("14400"), 
		TEXT("19200"), TEXT("38400"), TEXT("57600"), TEXT("115200")};
	SetDlgItemText(hwnd, IDC_BAUD, baudrate[2]);
	for (int i=0; i<_countof(baudrate); ++i)
	{
		SendMessage(GetDlgItem(hwnd,IDC_BAUD), CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)baudrate[i]);
	}

	//����λ
	TCHAR bits[8];
	for (int i=5; i<=8; ++i)
	{
		_stprintf_s(bits, 8, TEXT("%d"), i);
		SendMessage(GetDlgItem(hwnd,IDC_BITS), CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)bits);
	}

	SendMessage(GetDlgItem(hwnd,IDC_BITS), CB_SETCURSEL, 3, 0);

	//ֹͣλ
	TCHAR *stopbit[] = {TEXT("1"), TEXT("1.5"), TEXT("2")};

	for (int i=0; i<_countof(stopbit); ++i)
	{
		SendMessage(GetDlgItem(hwnd,IDC_STOP), CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)stopbit[i]);
	}

	SendMessage(GetDlgItem(hwnd,IDC_STOP), CB_SETCURSEL, 0, 0);

	//У��λ
	TCHAR *paritybit[] = {TEXT("NONE"), TEXT("ODD"), TEXT("EVEN")};
	for (int i=0; i<_countof(paritybit); ++i)
	{
		SendMessage(GetDlgItem(hwnd,IDC_PARITY), CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)paritybit[i]);
	}
	SendMessage(GetDlgItem(hwnd,IDC_PARITY), CB_SETCURSEL, 0, 0);

	//������

	//ͼ��
	hComIcon[0] = LoadIcon(hInst,MAKEINTRESOURCE(IDI_COMCLOSE));
	hComIcon[1] = LoadIcon(hInst/*GetModuleHandle(NULL)*/,MAKEINTRESOURCE(IDI_COM));
	(HICON)SendMessage(GetDlgItem(hwnd,IDC_COMICON), STM_SETIMAGE, IMAGE_ICON, LPARAM(hComIcon[0]));

	return TRUE;
}

//int PortEnumAllComm(TCHAR *buf, int bufLen)
//{
//    HKEY hkey;  
//    int i = 0;
//
//    *buf = 0;
//    int result = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
//                            TEXT( "Hardware\\DeviceMap\\SerialComm" ),
//                            NULL,
//                            KEY_READ,
//                            &hkey );
//
//    if (ERROR_SUCCESS == result)                                              //�򿪴���ע���
//    {   
//        TCHAR portName[0x100] = {0};
//        TCHAR commName[0x100] = {0};
//        DWORD dwLong, dwSize;
//
//        do
//        {   
//            dwSize = sizeof(portName)/sizeof(TCHAR);
//            dwLong = dwSize;
//            result = RegEnumValue(hkey,
//                                 i,
//                                 portName,
//                                 &dwLong, NULL,
//                                 NULL,
//                                 (LPBYTE)commName,
//                                 &dwSize
//                                 );                                           //dwLongָ��portName�ĳ��ȣ�dwSizeָ��commName�ĳ���
//
//            if (ERROR_NO_MORE_ITEMS == result)
//            {
//                break;                                                        //commName���Ǵ�������
//            }
//
//            _tcscpy_s(buf, bufLen, commName);
//            buf += (_tcslen(buf) + 1);
//            i++;
//
//        } while (1);
//
//        RegCloseKey(hkey);
//    }
//
//    *buf = 0;
//
//    return i;
//}

PortInfo PortEnumAllComm()
{
	Port *p,*rear;
	PortInfo head;
	head = NULL;
	int i = 0;
	HKEY hkey;  
	int bufLen = _countof(head->PortName);

	int result = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
		TEXT( "Hardware\\DeviceMap\\SerialComm" ),
		NULL,
		KEY_READ,
		&hkey );

	if (ERROR_SUCCESS == result)                                              //�򿪴���ע���
	{   
		TCHAR portName[0x100] = {0};
		TCHAR commName[0x100] = {0};
		DWORD dwLong, dwSize;

		do
		{   
			dwSize = sizeof(portName)/sizeof(TCHAR);
			dwLong = dwSize;
			result = RegEnumValue(hkey,
				i,
				portName,
				&dwLong, NULL,
				NULL,
				(LPBYTE)commName,
				&dwSize
				);                                           //dwLongָ��portName�ĳ��ȣ�dwSizeָ��commName�ĳ���

			if (ERROR_NO_MORE_ITEMS == result)
			{
				break;                                                        //commName���Ǵ�������
			}

			p = (Port*)malloc(sizeof(Port));
			_tcscpy_s(p->PortName, bufLen, commName);
			p->next = NULL;
			if(NULL==head)
			{
				head = p;
				rear = head;
			}
			else
			{
				rear->next = NULL;
				rear = p;
			}

			++i;
		} while (1);

		RegCloseKey(hkey);
	}

	return head;
}

void UpdateDevice(PDEV_BROADCAST_DEVICEINTERFACE pDevInf, UINT nEventType)
{
	switch(nEventType)
	{
	case   DBT_DEVICEARRIVAL:
		//bEventRun = (bool)GetPrivateProfileInt(TEXT("״̬����") , TEXT("bEventRun") , 0 , szIni);
		//bBeComm = (bool)GetPrivateProfileInt(TEXT("״̬����"), TEXT("bBeComm") , 0 , szIni);

		if(bEventRun)
		{
		SetWindowText(GetDlgItem(hComDlg,IDC_BTNOPENCOM),TEXT("�رմ���"));
		(HICON)SendMessage(GetDlgItem(hComDlg,IDC_COMICON), STM_SETIMAGE, IMAGE_ICON, LPARAM(hComIcon[1]));
		}
		_stprintf_s(devInfo, _countof(devInfo), TEXT("Adding��")); 
		_tcscat_s( devInfo, _countof(devInfo),TEXT("\r\n")); 

		break;

	case   DBT_DEVICEREMOVECOMPLETE:
		//if(bEventRun)
		//{
		//	WritePrivateProfileString( TEXT("״̬����") , TEXT("bEventRun") , TEXT("1") , szIni );
		//	if(bBeComm)
		//		WritePrivateProfileString( TEXT("״̬����") , TEXT("bBeComm") , TEXT("1") , szIni );
		//
		//}
		SetWindowText(GetDlgItem(hComDlg,IDC_BTNOPENCOM),TEXT("�򿪴���"));
		(HICON)SendMessage(GetDlgItem(hComDlg,IDC_COMICON), STM_SETIMAGE, IMAGE_ICON, LPARAM(hComIcon[0]));

		_stprintf_s(devInfo, _countof(devInfo), TEXT("Removing��"));
		_tcscat_s( devInfo, _countof(devInfo),TEXT("\r\n"));

		break;

	default:
		break;
	}  

	return  ; 
}

BOOL OnDeviceChange(UINT nEventType,DWORD dwData)
{
	if(DBT_DEVICEARRIVAL == nEventType || DBT_DEVICEREMOVECOMPLETE == nEventType)
	{
		PDEV_BROADCAST_HDR pHdr = (PDEV_BROADCAST_HDR)dwData;
		PDEV_BROADCAST_DEVICEINTERFACE pDevInf;
		PDEV_BROADCAST_HANDLE pDevHnd;
		PDEV_BROADCAST_OEM pDevOem;
		PDEV_BROADCAST_PORT pDevPort;
		PDEV_BROADCAST_VOLUME pDevVolume;
		switch (pHdr->dbch_devicetype)
		{
		case DBT_DEVTYP_DEVICEINTERFACE:
			pDevInf = (PDEV_BROADCAST_DEVICEINTERFACE)pHdr;
			UpdateDevice(pDevInf, nEventType);
			break;

		case DBT_DEVTYP_HANDLE:
			pDevHnd = (PDEV_BROADCAST_HANDLE)pHdr;
			break;

		case DBT_DEVTYP_OEM:
			pDevOem = (PDEV_BROADCAST_OEM)pHdr;
			break;

		case DBT_DEVTYP_PORT:
			pDevPort = (PDEV_BROADCAST_PORT)pHdr;
			break;

		case DBT_DEVTYP_VOLUME:
			pDevVolume = (PDEV_BROADCAST_VOLUME)pHdr;
			break;
		}
	}
	return TRUE;
}

/*
����ÿ����ӦWM_DEVICECHANGE��ϢwParam��=7������Ҫ����GUID

Ϊʲô�õ�����Ϣ��wParam����Ϊ7
����Ϊϵͳ���ںܶ�ʱ�򶼻ᴥ��WM_DEVICECHANGE
���Ǽ򵥵���Ϊusb key�Ĳ�βŻᴥ��

���ԣ�Ҫ�����ô��WM_DEVICECHANGE���жϳ������ǲ���
��Ϊusbkey�������ģ�����Ҫ�õ���usbkey��
ϵͳ����ע���guid�������Ǹ�usbkey��interface guid,guid�����֣�

��������ϵͳע�����guid�Ժ�
�Ϳ��Ժ�ĸ�usbkey����ϸ��Ϣ�ˣ����������ǲ廹�ǰ�
*/

void RegisterForDevChange(HWND hDlg)
{
	const GUID GUID_DEVINTERFACE_LIST[] = {
		// GUID_DEVINTERFACE_USB_DEVICE
		{ 0xA5DCBF10, 0x6530, 0x11D2, { 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED } },

		// GUID_DEVINTERFACE_DISK
		{ 0x53f56307, 0xb6bf, 0x11d0, { 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b } },

		// GUID_DEVINTERFACE_HID, 
		{ 0x4D1E55B2, 0xF16F, 0x11CF, { 0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 } },

		// GUID_NDIS_LAN_CLASS
		{ 0xad498944, 0x762f, 0x11d0, { 0x8d, 0xcb, 0x00, 0xc0, 0x4f, 0xc3, 0x35, 0x8c } } 
	};

	HDEVNOTIFY hDevNotify;

	for (int i = 0; i < sizeof(GUID_DEVINTERFACE_LIST) / sizeof(GUID); i++)
	{
		DEV_BROADCAST_DEVICEINTERFACE   broadcastInterface;

		broadcastInterface.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
		broadcastInterface.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;

		memcpy( &(broadcastInterface.dbcc_classguid),
			&(GUID_DEVINTERFACE_LIST[i]), 
			sizeof(struct _GUID));

		hDevNotify = RegisterDeviceNotification(hDlg,
			&broadcastInterface,
			DEVICE_NOTIFY_WINDOW_HANDLE);
	}
}

void RefreshComm(HWND hwnd , UINT nEventType)
{
	SendMessage(GetDlgItem(hwnd,IDC_COMNUM), CB_RESETCONTENT, 0, 0);

	PortInfo head = PortEnumAllComm();
	SetDlgItemText(hwnd, IDC_COMNUM, head->PortName);
	while(head)
	{
		SendMessage(GetDlgItem(hwnd,IDC_COMNUM), CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)head->PortName);
		head = head->next;
	}

	if(nEventType==DBT_DEVICEARRIVAL)
		RecoverState();
}

void RecoverState()
{
	if(bEventRun)
	{
	//	SetWindowText(GetDlgItem(hwnd,IDC_BTNOPENCOM),TEXT("�رմ���"));
	//	(HICON)SendMessage(GetDlgItem(hwnd,IDC_COMICON), STM_SETIMAGE, IMAGE_ICON, LPARAM(hComIcon[1]));
	//}
	//else
	//{
	//	SetWindowText(GetDlgItem(hwnd,IDC_BTNOPENCOM),TEXT("�򿪴���"));
	//	(HICON)SendMessage(GetDlgItem(hwnd,IDC_COMICON), STM_SETIMAGE, IMAGE_ICON, LPARAM(hComIcon[0]));

		if(bBeComm)
		{
			if(bAutoSend)
			{
				OnAutoSend();
			}
			else
			{
				if(bHexSend)
				{
					OnSendEvent(hCom, phSendData ,dwHexDataLen);
				}
				else
				{
					OnSendEvent(hCom , sSendData , _countof(sSendData)); 
				}
			}
		
		}
	}
}