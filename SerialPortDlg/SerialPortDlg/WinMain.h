#ifndef _WINMAIN_H_
#define _WINMAIN_H_

BOOL InitWindow(HINSTANCE hInstance, int nShowCmd);
LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

extern HINSTANCE hInst;

#endif