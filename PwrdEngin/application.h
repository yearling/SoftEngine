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
		Window(HINSTANCE hinstance):hwnd_(NULL),
			main_instance(hinstance),
			is_windowed_(true),width_(0),height_(0),client_offset_x_(0),client_offset_y_(0){};
		void Init(int _width,int Height,const TCHAR *);
		void ShowWindow(int cmd_show=SW_SHOW);
		operator HWND(){ return hwnd_;}
	public:
		HINSTANCE main_instance;
		HWND hwnd_;
		int width_;
		int height_;
		bool is_windowed_;
		int client_offset_x_;
		int client_offset_y_;
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
		std::shared_ptr<Window> main_window_;
		HINSTANCE main_instance_;
	};

	extern Application *YYUT_application;
	Application &GetApplication();
}
#endif