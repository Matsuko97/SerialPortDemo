#include "include.h"

TCHAR szAppName[]=TEXT("Test");
HINSTANCE hInst;

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowCmd)
{
	 MSG msg;
	 hInst = hInstance;
	 if (!InitWindow(hInstance, nShowCmd))
		return 0;

	 while (GetMessage(&msg,NULL,0,0))
	 {
		 TranslateMessage(&msg);
		 DispatchMessage(&msg);
	 }

	 return msg.wParam;
}

BOOL InitWindow(HINSTANCE hInstance, int nShowCmd)
{
	HWND hwnd;
	WNDCLASS wndclass;
	
	wndclass.style       = CS_VREDRAW | CS_HREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra  = 0;
	wndclass.cbWndExtra  = 0;
	wndclass.hInstance   = hInstance;	
	wndclass.hIcon       = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor     = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName  = MAKEINTRESOURCEW(IDR_MENU1);
	wndclass.lpszClassName = szAppName;
	
	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("Program Need Windows NT!"), szAppName, MB_ICONERROR);
		return FALSE;
	}
 
	hwnd = CreateWindow(
						szAppName,
						szAppName, 
						WS_OVERLAPPEDWINDOW,
						CW_USEDEFAULT, 
						CW_USEDEFAULT,	
						CW_USEDEFAULT, 
						CW_USEDEFAULT,
						NULL,
						NULL,
						hInstance,
						NULL);
	if (hwnd == NULL)
		return FALSE;
	
	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);
	
	return TRUE;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int            cxChar, cyChar ,cxClient ,cyClient;

	switch (message)
	{
	case WM_CREATE:
		cxChar = LOWORD (GetDialogBaseUnits ()) ;
		cyChar = HIWORD (GetDialogBaseUnits ()) ;
		//
		return 0 ;

	case WM_SIZE:
		cxClient = LOWORD (lParam) ;
		cyClient = HIWORD (lParam) ;

		return 0;

	case WM_COMMAND:
		switch (LOWORD (wParam))
		{
		case ID_FILE:

			break;

		case ID_COM:
				hComDlg = CreateDialog(hInst,MAKEINTRESOURCE(IDD_SERIALPORTDLG),hwnd,(DLGPROC)ComDlgProc);
				ShowWindow(hComDlg, SW_NORMAL);
			break; 

		}
		return 0 ;

	case WM_DESTROY:
		
		PostQuitMessage (0) ;
		return 0 ;
	}
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}

