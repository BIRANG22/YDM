#include <iostream>
//#include "INC_Windows.h"
#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
//#include <windowsx.h>
#include "GeoShapeManager.h"


namespace 
{
	
	int x = -1;
	int y = -1;
	bool click = false;
	LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		switch (msg)
		{
		case WM_CREATE:
		{
			std::cout << "WM_CREATE" << std::endl;
		}break;

		case WM_CLOSE:
		{
			std::cout << "WM_CLOSE" << std::endl;
			// �����츦 �����ϱ� ���� �޽���
			PostQuitMessage(0); // WM_QUIT
		}break;

		case WM_CHAR: // <- �߰�!
		{
			std::cout << "You pressed Key!" << std::endl;
			
		}break;

		case WM_LBUTTONDOWN:
		{
			x = LOWORD(lparam);
			y = HIWORD(lparam);
			std::cout << "���� Ŭ�� ��ġ: (" << x << ", " << y << ")" << std::endl;
			
			
			simplegeo::g_GeoShapeManager.AddCircle(x, y, 20, RGB(255,0,0));

			//HDC hdc = GetDC(hwnd); // ������ ��ü�� ���� DC ȹ��
			//const wchar_t* text = L"o";
			//TextOut(hdc, x, y, text, lstrlenW(text));
			//ReleaseDC(hwnd, hdc); // �� ����!

			InvalidateRect(hwnd, NULL, TRUE); // WM_PAINT �߻� ��û
			//click = false;
		}break;

		case WM_RBUTTONDOWN:
		{
			x = LOWORD(lparam);
			y = HIWORD(lparam);
			std::cout << "������ Ŭ�� ��ġ: (" << x << ", " << y << ")" << std::endl;

			simplegeo::g_GeoShapeManager.AddCircle(x, y, 20, RGB(0, 0, 255));

			/*HDC hdc = GetDC(hwnd);
			const wchar_t* text = L"x";
			TextOut(hdc, x, y, text, lstrlenW(text));
			ReleaseDC(hwnd, hdc);*/

			InvalidateRect(hwnd, NULL, TRUE); // WM_PAINT �߻� ��û
			click = true;
		}break;


		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);

			simplegeo::g_GeoShapeManager.Draw(hdc);  // ������ ��ü�� �׸�

			EndPaint(hwnd, &ps);
		}
		break;

		default:
			return::DefWindowProc(hwnd, msg, wparam, lparam);
		}
		return NULL;
	}

	HWND CreateWidow(const wchar_t* className, const wchar_t* windowName, int width, int height)
	{
		WNDCLASSEX wc = {};
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.lpszClassName = className;
		wc.lpfnWndProc = WndProc; // ������ ���ν���(�Լ�)�� ������ ���


		ATOM classId = 0;
		if (!GetClassInfoEx(HINSTANCE(), L"NzWindowClass", &wc))
		{
			classId = RegisterClassEx(&wc);// �ü������ ���� ���� Ŭ���� ������ ������ ������ ��� ���� �ϳ� �������

			if (0 == classId) return NULL;
		}

		RECT rc = { 0, 0, width, height };

		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, false);//â ũ�⸦ ������ �ݴϴ�.

		std::cout << "width: " << rc.right - rc.left << " height: " << rc.bottom - rc.top << std::endl;

		// �غ�� WNDCLASSEX wc �� �ü���� ����ؼ� ���� �ĺ�Ű�� ������ (classId ) �� ������(Ŀ��)��ü�� ����� �޶� �ϰ� ������ �ڵ��� �޾� �ɴϴ�
		HWND hWnd = CreateWindowEx(NULL, MAKEINTATOM(classId), L"", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
			rc.right - rc.left, rc.bottom - rc.top, HWND(), HMENU(), HINSTANCE(), NULL);

		if (NULL == hWnd) return (HWND)(NULL);// ��������ٸ� ����

		::SetWindowText((HWND)hWnd, windowName);//������â�� �ؽ�Ʈ ����

		ShowWindow((HWND)hWnd, SW_SHOW);//������ ������. SW_HIDE:�Ⱥ���
		UpdateWindow((HWND)hWnd);//������Ʈ�� ����� ���� WM_PAINT �޽��� ó���� ������ ���ϴ�.

		return hWnd;
	}

	void GameLoop()
	{
		MSG msg = {};
		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, HWND(), 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				///HERE: GAME LOGIC UPATE & RENDERING
			}
		}
	}

}

int main()
{
	// GUI â ����
	HWND hWnd = CreateWidow(L"NzWindowClass", L"����ĥ��", 1280, 720);

	if (NULL == hWnd)
	{
		std::cout << "������ ���� ����" << std::endl;
		system("pause");
		return -1;
	}

	std::cout << "hWnd: " << hWnd << std::endl;
	//std::cout << (*hWnd).unused << std::endl; ��Ÿ�� ����!

	GameLoop();

	DestroyWindow(hWnd);

	return 0;

}












/*HWND hWnd = GetConsoleWindow();

	std::cout << "�ܼ� ������ �ڵ�: " << hWnd << std::endl;
	std::cout << "������ �ڵ� : " << HWND() << std::endl;
	

	MSG msg = {};
	while (WM_QUIT != msg.message)
	{
		//if (PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE)) (Non-Blocking)
		// GetMessage�� �޽����� ���� ������ ����Ѵ�. (Blocking)
		if (0 != GetMessage(&msg, hWnd, 0, 0) != 0) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			///HERE: GAME LOGIC UPATE & RENDERING
			std::cout << "HERE: " << hWnd << std::endl;
		}
	}*/