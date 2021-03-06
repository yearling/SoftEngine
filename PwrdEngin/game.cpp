#include "stdafx.h"
#include "game.h"
#include "Gamemath.h"
#include <stdio.h>
#include <locale.h>
#include <iostream>
using std::cout;
using std::endl;
namespace SoftEngine
{
	void Game::Initial()
	{
		//create window and show window
		Application::Initial();
		int width=800;
		int height=600;
		main_window_->Init(width,height,_T("Soft Engine"));
		main_window_->ShowWindow();	
		AllocConsoleDebug();
		render_=new Device();
		if(!render_->Init(main_window_.get()))
			throw std::exception("Initial failed!\n");
	}

	int Game::Run()
	{
		MSG msg;
		while(1)
		{
			if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
			{ 
				if (msg.message == WM_QUIT)
					break;
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			} 
			GameMain();
		} 
		return 0;
	}

	void Game::Exit()
	{

	}

	LRESULT Game::MyProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch(msg)
		{
		case WM_KEYDOWN:
			{
				switch( wParam )
				{
				case VK_ESCAPE:
					{
						SendMessage( hwnd, WM_CLOSE, 0, 0 );
						break;
					}
				}
				break;
			}
		case WM_DESTROY: 
			{
				PostQuitMessage(0);
				return(0);
			} break;
		default:
			break;
		}
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	Game::Game()
	{

	}

	void Game::GameMain()
	{
		render_->Clear(_RGB(255,0,0));
		if(render_->BeginDraw())
		{
			for(int i=0;i<100;i++)
			{
				int x0=rand()%2000-1000;
				int y0=rand()%2000-1000;
				int x1=rand()%2000-1000;
				int y1=rand()%2000-1000;
				render_->DrawLine(x0,y0,x1,y1);
			}
			render_->EndDraw();
		}
		render_->Present();
		Sleep(30);
		static bool first_time=true;
		if(first_time)
		{
			first_time=false;
			float a[16]={1,2,3,4,
						5,6,7,8,
						9,10,11,12,
						13,14,15,16};
			Matrix m1(a);
			float b[16]={2,3,5,0,
						 1,4,8,0,
						2,1,1,0,
						0,0,0,1};
			cout<<m1;
			Matrix m2(b);
			float deter=MatrixDetermint(&m1);
			cout<<deter<<endl;
			deter=MatrixDetermint(&m2);
			cout<<deter<<endl;
			auto m3=m1*m2;
			cout<<m3<<endl;
			cout<<MatrixDetermint(&m3)<<endl;
		}
	}

	void Game::AllocConsoleDebug()
	{
		AllocConsole();
		FILE *stream;
		freopen_s(&stream,"CONOUT$", "w",stdout);
		freopen_s(&stream,"CONIN$","w+t",stdin); 
		freopen_s(&stream,"CONOUT$", "w+t",stderr); 
		setlocale(LC_ALL,"chs");  
		std::ios::sync_with_stdio();
		std::cout.clear();
		std::wcout.clear();
	}

}