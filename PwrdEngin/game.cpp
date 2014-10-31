#include "stdafx.h"
#include "game.h"
#include "Gamemath.h"
#include "Device.h"
#include <stdio.h>
#include <locale.h>
#include <iostream>
#include <mmsystem.h>
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
		device_=new Device();
		if(!device_->Init(main_window_.get()))
			throw std::exception("Initial failed!\n");
		VERTEXELEMENT v_list[]={
			{0,DECLTYPE_FLOAT3,DECLUSAGE_POSITION,0},
			DECL_END()
		};
		vertex_decl_=device_->CreateVertexDeclaration(v_list);
		if(vertex_decl_)
			device_->SetVertexDeclaration(vertex_decl_);
		vertex_buffer_=device_->CreateVertexBuffer(sizeof(Vector3)*8);
		index_buffer_=device_->CreateIndexBuffer(36);
		Vector3 *vp=(Vector3*)vertex_buffer_->Lock();
		UINT *ip=(UINT*)index_buffer_->Lock();
		vp[0]=Vector3(-1.0f,-1.0f,-1.0f);
		vp[1]=Vector3(-1.0f,1.0f,-1.0f);
		vp[2]=Vector3(1.0f,1.0f,-1.0f);
		vp[3]=Vector3(1.0f,-1.0f,-1.0f);
		vp[4]=Vector3(-1.0f,-1.0f,1.0f);
		vp[5]=Vector3(-1.0f,1.0f,1.0f);
		vp[6]=Vector3(1.0f,1.0f,1.0f);
		vp[7]=Vector3(1.0f,-1.0f,1.0f);
		index_buffer_->UnLock();
		//font
		ip[0]=0;ip[1]=1;ip[2]=2;
		ip[3]=0;ip[4]=2;ip[5]=3;
		//back
		ip[6]=4;ip[7]=6;ip[8]=5;
		ip[9]=4;ip[10]=7;ip[11]=6;
		//left
		ip[12]=4;ip[13]=5;ip[14]=1;
		ip[15]=4;ip[16]=1;ip[17]=0;
		//right
		ip[18]=3;ip[19]=2;ip[20]=6;
		ip[21]=3;ip[22]=6;ip[23]=7;
		//top
		ip[24]=1;ip[25]=5;ip[26]=6;
		ip[27]=1;ip[28]=6;ip[29]=2;
		//bottom
		ip[30]=4;ip[31]=0;ip[32]=3;
		ip[33]=4;ip[34]=3;ip[35]=7;
		index_buffer_->UnLock();
		
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
			float elapse_time=(float)(timeGetTime()-last_time_)/1000;
			last_time_=timeGetTime();
			GameMain(elapse_time);
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
		device_=nullptr;
		vertex_decl_=nullptr;
		vertex_buffer_=nullptr;
		index_buffer_=nullptr;
		last_time_=0;
	}

	void Game::GameMain(float elapse_time)
	{
		//cout<<elapse_time<<endl;
		static float angle=0.0f;
		angle+=elapse_time*PI;
		if(angle>PI*2)
			angle=0.0f;
		Matrix rote;
		MatrixRotationY(&rote,angle);
		device_->Clear(_RGB(25,25,25));
		Vector3 eye(0.0f,0.0f,-5.0f);
		Vector3 at(0.0f,0.0f,0.0f);
		Vector3 up(0.0f,1.0f,0.0f);
		MatrixLookAtLH(&view_,&eye,&at,&up);
		
		MatrixPerspectiveFOVLH(&proj_,PI*0.5f,(float)main_window_->width_/(float)
			main_window_->height_,1.0f,1000.0f);
		
		if(device_->BeginScene())
		{
			device_->SetWorld(&rote);
			device_->SetView(&view_);
			device_->SetProject(&proj_);
			device_->SetViewPort();
			device_->SetStreamSource(vertex_buffer_);
			device_->SetIndices(index_buffer_);
			device_->DrawIndexedPrimitive(PT_TRIANGLEIST,0,0,8,0,12);
			device_->EndScene();
		}
		device_->Present();
		Sleep(20);
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