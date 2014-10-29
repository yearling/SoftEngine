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
		draw_imp_.Init(main_window_->hwnd_,width,height,main_window_->client_offset_x_,main_window_->client_offset_y_);
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
		draw_imp_.Flip();
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
			cout<<m1;
			Matrix m2;
			MatrixMultiply(&m2,&m1,&m1);
			cout<<m2;
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