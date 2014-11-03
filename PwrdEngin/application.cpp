#include "stdafx.h"
#include "application.h"
#include <assert.h>
#include <Windows.h>
#include <windowsx.h>
namespace SoftEngine
{
	Application *YYUT_application;

	Application::Application(void)
	{
		YYUT_application=this;
	}


	Application::~Application(void)
	{
	}

	void Application::WindowCreate(int width,int height,const TCHAR *title,int cmdshow)
	{
		assert(spMainWindow);
		try
		{
			spMainWindow->Init(width,height,title);
			ShowWindow(*spMainWindow,cmdshow);
			UpdateWindow(*spMainWindow);
		}
		catch(WidnowException &e)
		{
			UNREFERENCED_PARAMETER(e);
			throw;
		}
	}

	void Application::Initial()
	{
		spMainWindow.reset(new Window(hInstance));
	}

	int Application::Run()
	{
		MSG msg;
		while (1)
		{
			while (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
			{
				if(WM_QUIT == msg.message)
					return static_cast<int>(msg.wParam);
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		return static_cast<int>(msg.wParam);
	}

	void Application::Exit()
	{
	}


	LRESULT CALLBACK Application::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		Application &app=GetApplication();
		return app.MyProc(hWnd,message,wParam,lParam);
	}

	LRESULT Application::MyProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		int wmId, wmEvent;
		PAINTSTRUCT ps;
		HDC hdc;

		switch (message)
		{
		case WM_COMMAND:
			wmId    = LOWORD(wParam);
			wmEvent = HIWORD(wParam);
			return DefWindowProc(hWnd, message, wParam, lParam);
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			// TODO: 在此添加任意绘图代码...
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		return 0;
	}

	Application & GetApplication()
	{
		return *YYUT_application;
	}
	//throw an exception 
	void Window::Init(int _width,int Height,const TCHAR *window_name)
	{
		DWORD hr=0;
		iWidth=_width;
		iHeight=Height;
		WNDCLASSEX wcex;
		TCHAR _Regstr[]={_T("YYWINDOW")};
		wcex.cbSize			= sizeof(WNDCLASSEX);
		wcex.style			= CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc	= Application::WndProc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= hInstance;
		wcex.hIcon			=  LoadIcon(NULL,IDI_APPLICATION);
		wcex.hCursor		=  LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground	=	(HBRUSH)(COLOR_WINDOW+1);
		wcex.lpszMenuName	=	 nullptr;
		wcex.lpszClassName	=	_Regstr;
		wcex.hIconSm		=	 LoadIcon(NULL, IDI_APPLICATION);
		hr=RegisterClassEx(&wcex);
		hWnd=CreateWindow(_Regstr,window_name, WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, iWidth, iHeight, NULL, NULL, hInstance, NULL);
		hr=GetLastError();
		if(hWnd)
		{
			if (bWindow)
			{

				RECT window_rect = {0,0,iWidth-1,iHeight-1};
				::AdjustWindowRectEx(&window_rect,GetWindowStyle(hWnd),GetMenu(hWnd) != NULL,GetWindowExStyle(hWnd));
				iClientOffsetX=-window_rect.left;
				iClientOffsetY=-window_rect.top;
				MoveWindow(hWnd,100,50, 
					window_rect.right - window_rect.left, 
					window_rect.bottom - window_rect.top,
					FALSE);
			}
		}
		else
			throw WidnowException();
	}

	void Window::ShowWindow(int cmd_show)
	{
		::ShowWindow(hWnd,cmd_show);
		UpdateWindow(hWnd);
	}

}
