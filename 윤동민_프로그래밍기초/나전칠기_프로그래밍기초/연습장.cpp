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
			// 윈도우를 종료하기 위한 메시지
			PostQuitMessage(0); // WM_QUIT
		}break;

		case WM_CHAR: // <- 추가!
		{
			std::cout << "You pressed Key!" << std::endl;
			
		}break;

		case WM_LBUTTONDOWN:
		{
			x = LOWORD(lparam);
			y = HIWORD(lparam);
			std::cout << "왼쪽 클릭 위치: (" << x << ", " << y << ")" << std::endl;
			
			
			simplegeo::g_GeoShapeManager.AddCircle(x, y, 20, RGB(255,0,0));

			//HDC hdc = GetDC(hwnd); // 윈도우 전체에 대한 DC 획득
			//const wchar_t* text = L"o";
			//TextOut(hdc, x, y, text, lstrlenW(text));
			//ReleaseDC(hwnd, hdc); // 꼭 해제!

			InvalidateRect(hwnd, NULL, TRUE); // WM_PAINT 발생 요청
			//click = false;
		}break;

		case WM_RBUTTONDOWN:
		{
			x = LOWORD(lparam);
			y = HIWORD(lparam);
			std::cout << "오른쪽 클릭 위치: (" << x << ", " << y << ")" << std::endl;

			simplegeo::g_GeoShapeManager.AddCircle(x, y, 20, RGB(0, 0, 255));

			/*HDC hdc = GetDC(hwnd);
			const wchar_t* text = L"x";
			TextOut(hdc, x, y, text, lstrlenW(text));
			ReleaseDC(hwnd, hdc);*/

			InvalidateRect(hwnd, NULL, TRUE); // WM_PAINT 발생 요청
			click = true;
		}break;


		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);

			simplegeo::g_GeoShapeManager.Draw(hdc);  // 도형들 전체를 그림

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
		wc.lpfnWndProc = WndProc; // 윈도우 프로시저(함수)의 포인터 등록


		ATOM classId = 0;
		if (!GetClassInfoEx(HINSTANCE(), L"NzWindowClass", &wc))
		{
			classId = RegisterClassEx(&wc);// 운영체제한테 지금 만든 클래스 정보를 가지고 윈도우 등록 정보 하나 만들어줘

			if (0 == classId) return NULL;
		}

		RECT rc = { 0, 0, width, height };

		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, false);//창 크기를 조정해 줍니다.

		std::cout << "width: " << rc.right - rc.left << " height: " << rc.bottom - rc.top << std::endl;

		// 준비된 WNDCLASSEX wc 를 운영체제에 등록해서 받은 식별키를 가지고 (classId ) 고 윈도우(커널)객체를 만들어 달라 하고 윈도우 핸들을 받아 옵니다
		HWND hWnd = CreateWindowEx(NULL, MAKEINTATOM(classId), L"", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
			rc.right - rc.left, rc.bottom - rc.top, HWND(), HMENU(), HINSTANCE(), NULL);

		if (NULL == hWnd) return (HWND)(NULL);// 못만들었다면 실패

		::SetWindowText((HWND)hWnd, windowName);//윈도우창에 텍스트 설정

		ShowWindow((HWND)hWnd, SW_SHOW);//윈도우 보여줘. SW_HIDE:안보임
		UpdateWindow((HWND)hWnd);//업데이트를 해줘야 실제 WM_PAINT 메시지 처리가 가능해 집니다.

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
	// GUI 창 생성
	HWND hWnd = CreateWidow(L"NzWindowClass", L"나전칠기", 1280, 720);

	if (NULL == hWnd)
	{
		std::cout << "윈도우 생성 실패" << std::endl;
		system("pause");
		return -1;
	}

	std::cout << "hWnd: " << hWnd << std::endl;
	//std::cout << (*hWnd).unused << std::endl; 런타임 에러!

	GameLoop();

	DestroyWindow(hWnd);

	return 0;

}












/*HWND hWnd = GetConsoleWindow();

	std::cout << "콘솔 윈도우 핸들: " << hWnd << std::endl;
	std::cout << "윈도우 핸들 : " << HWND() << std::endl;
	

	MSG msg = {};
	while (WM_QUIT != msg.message)
	{
		//if (PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE)) (Non-Blocking)
		// GetMessage는 메시지가 없을 때까지 대기한다. (Blocking)
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