#include "include.h"

HWND hComEx;

INT_PTR CALLBACK ComExProc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_INITDIALOG:

		return 0;

	case WM_SYSCOMMAND:
		if(wParam == SC_CLOSE)
		{
			DestroyWindow(hdlg);
		}
		return 0;

		//case WM_COMMAND:
		//	{
		//		int idDC = LOWORD(wParam);
		//		switch(idDC)
		//		{

		//		default:
		//			break;
		//		}
		//	}
		//	return 0;

	case WM_CLOSE:
		DestroyWindow(hdlg);
		return 0;
	}
	return (INT_PTR)FALSE;
}
