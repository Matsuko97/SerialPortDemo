#ifndef _ADDTABCTR_H
#define _ADDTABCTR_H


typedef struct tag_dlghdr 
{ 
	HWND       hwndPages;   // current child dialog box 

	TCHAR       szPage_Title[50];         //����ҳ��ǩ����

	int         TabPage_Resource_ID;	//����ҳ��Ӧ�ĶԻ���ID
	DLGPROC     szPage_DlgProcName;    //����ҳ��Ӧ�ĶԻ����������
} TabDlgPage;

HRESULT      OnTabbedDialogInit(int iNum, TabDlgPage * TabPageInfo,HWND  hTab,HINSTANCE g_hInst);


#endif