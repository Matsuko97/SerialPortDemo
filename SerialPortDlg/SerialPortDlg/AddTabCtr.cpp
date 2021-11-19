#include "include.h"

HRESULT OnTabbedDialogInit(int iNum, TabDlgPage * TabPageInfo,HWND  hTab,HINSTANCE g_hInst)   //tab控件初始化函数
{ 
	TCITEM   TIE; 
	int      i = 0;
	
	for (i = 0; i < iNum; i++)
	{

		if (hTab == NULL) 
		{
			return HRESULT_FROM_WIN32(GetLastError());
		}

		TIE.mask = TCIF_TEXT|TCIF_IMAGE ;            //设置TAB标签栏
		TIE.iImage = -1; 
		TIE.pszText =TabPageInfo[i].szPage_Title; 
		TabCtrl_InsertItem(hTab, i, &TIE); 

	}

	for (i = 0 ; i < iNum; i++)
	{
		if (TabPageInfo[i].hwndPages != NULL)
		{
			DestroyWindow(TabPageInfo[i].hwndPages);
		}

		TabPageInfo[i].hwndPages = CreateDialog(g_hInst,MAKEINTRESOURCE(TabPageInfo[i].TabPage_Resource_ID)/*TabPageInfo[i].szPage_ResourceName*/,hTab,TabPageInfo[i].szPage_DlgProcName);
		
	}

	TabCtrl_SetCurSel(hTab,0);
	ShowWindow(TabPageInfo[0].hwndPages,SW_SHOW);

	return S_OK;
} 