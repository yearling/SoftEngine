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
		Window(HINSTANCE hinstance):m_hWnd(NULL),
			m_hInstance(hinstance),
			m_bWindow(true),m_iWidth(0),m_iHeight(0),m_iClientOffsetX(0),m_iClientOffsetY(0){};
		void Init(int _width,int Height,const TCHAR *);
		void ShowWindow(int cmd_show=SW_SHOW);
		operator HWND(){ return m_hWnd;}
	public:
		HINSTANCE m_hInstance;
		HWND m_hWnd;
		int m_iWidth;
		int m_iHeight;
		bool m_bWindow;
		int m_iClientOffsetX;
		int m_iClientOffsetY;
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
		std::shared_ptr<Window> m_spMainWindow;
		HINSTANCE m_hInstance;
	};

	extern Application *g_Application;
	Application &GetApplication();
}
#endif