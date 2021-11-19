#ifndef _ADDTABCTR_H
#define _ADDTABCTR_H


typedef struct tag_dlghdr 
{ 
	HWND       hwndPages;   // current child dialog box 

	TCHAR       szPage_Title[50];         //属性页标签名称

	int         TabPage_Resource_ID;	//属性页对应的对话框ID
	DLGPROC     szPage_DlgProcName;    //属性页对应的对话框过程名称
} TabDlgPage;

HRESULT      OnTabbedDialogInit(int iNum, TabDlgPage * TabPageInfo,HWND  hTab,HINSTANCE g_hInst);


#endif