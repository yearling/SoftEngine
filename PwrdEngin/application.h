#ifndef __APPLICATION_H__
#define __APPLICATION_H__
#pragma once
#include "stdafx.h"
#include <memory>
#include <exception>
namespace SoftEngine
{
	struct WidnowException:virtual std::exception
	{
		WidnowException():std::exception("[YYUTWindow]:create window failed!")
		{
		}
	};
	class Window
	{
	public:
		Window(HINSTANCE hinstance):hWnd(NULL),
			hInstance(hinstance),
			bWindow(true),iWidth(0),iHeight(0),iClientOffsetX(0),iClientOffsetY(0){};
		void Init(int _width,int Height,const TCHAR *);
		void ShowWindow(int cmd_show=SW_SHOW);
		operator HWND(){ return hWnd;}
	public:
		HINSTANCE hInstance;
		HWND hWnd;
		int iWidth;
		int iHeight;
		bool bWindow;
		int iClientOffsetX;
		int iClientOffsetY;
	};
	class Application
	{
	public:
		friend Window;
		Application(void);
		virtual ~Application(void);
		virtual void WindowCreate(int width,int height,const TCHAR *,int=SW_SHOW);
		virtual void Initial();
		virtual int Run();
		virtual void Exit();
	protected:
		static LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) ;
		virtual LRESULT MyProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) throw();
	private:
		void * operator new(size_t){};
		void * operator new[](size_t){};
		void operator delete(void *){}; 
		void operator delete[](void *){}; 

	public:
		std::shared_ptr<Window> spMainWindow;
		HINSTANCE hInstance;
	};

	extern Application *YYUT_application;
	Application &GetApplication();
}
#endif