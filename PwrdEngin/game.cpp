#include "stdafx.h"
#include "game.h"
#include "Gamemath.h"
#include "Device.h"
#include <stdio.h>
#include <locale.h>
#include <iostream>
#include <mmsystem.h>
#include <sstream>
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
		m_spMainWindow->Init(width,height,_T("Soft Engine"));
		m_spMainWindow->ShowWindow();	
		AllocConsoleDebug();
		m_pDevice=new Device();
		if(!m_pDevice->Init(m_spMainWindow.get()))
			throw std::exception("Initial failed!\n");
		VERTEXELEMENT v_list[]={
			{0,DECLTYPE_FLOAT3,DECLUSAGE_POSITION,0},
			DECL_END()
		};
		m_pVertexDecl=m_pDevice->CreateVertexDeclaration(v_list);
		if(m_pVertexDecl)
			m_pDevice->SetVertexDeclaration(m_pVertexDecl);
		m_pVertex_buffer=m_pDevice->CreateVertexBuffer(sizeof(Vector3)*8);
		m_pIndexBuffer=m_pDevice->CreateIndexBuffer(36);
		Vector3 *vp=(Vector3*)m_pVertex_buffer->Lock();
		UINT *ip=(UINT*)m_pIndexBuffer->Lock();
		vp[0]=Vector3(-1.0f,-1.0f,-1.0f);
		vp[1]=Vector3(-1.0f,1.0f,-1.0f);
		vp[2]=Vector3(1.0f,1.0f,-1.0f);
		vp[3]=Vector3(1.0f,-1.0f,-1.0f);
		vp[4]=Vector3(-1.0f,-1.0f,1.0f);
		vp[5]=Vector3(-1.0f,1.0f,1.0f);
		vp[6]=Vector3(1.0f,1.0f,1.0f);
		vp[7]=Vector3(1.0f,-1.0f,1.0f);
		m_pIndexBuffer->UnLock();
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
		m_pIndexBuffer->UnLock();
		//////////////////////////////////////////////////////////////////////////
		m_pFbxPaser=new FbxPaser();
		m_pFbxPaser->Init(m_pDevice);
		//parser_->Load("..\\media\\box.fbx");
		//parser_->Load("E:\\scene_fbx\\test\\box_normal.fbx");
		m_pFbxPaser->Load("E:\\scene_fbx\\ring.fbx");
		//////////////////////////////////////////////////////////////////////////
		m_pArcBall=new ArcBall();
		m_pArcBall->Init(m_spMainWindow->m_hWnd);

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
			float elapse_time=(float)(timeGetTime()-m_dwLastTime)/1000;
			m_dwLastTime=timeGetTime();
			PreRender(elapse_time);
			Render(elapse_time);
		} 
		return 0;
	}

	void Game::Exit()
	{

	}

	LRESULT Game::MyProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if(m_pArcBall)
		{
			if(m_pArcBall->HandleMessage(hwnd,msg,wParam,lParam))
				return S_OK;
		}
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
		m_pDevice=nullptr;
		m_pVertexDecl=nullptr;
		m_pVertex_buffer=nullptr;
		m_pIndexBuffer=nullptr;
		m_dwLastTime=0;
		m_dwLastFrameTime=0;
		m_iLastFrameCounts=0;
		m_iFPS=0;
		m_pArcBall=nullptr;
	}

	void Game::Render(float elpase_time)
{
		//cout<<elapse_time<<endl;
		static float angle=0.0f;
		angle+=elpase_time*PI;
		if(angle>PI*2)
			angle=0.0f;
		Matrix rote;
		MatrixRotationY(&rote,angle);
		m_pDevice->Clear(_RGB(25,25,25));
		Vector3 eye(0.0f,0.0f,-80.0f);
		Vector3 at(0.0f,0.0f,0.0f);
		Vector3 up(0.0f,1.0f,0.0f);
		MatrixLookAtLH(&m_matView,&eye,&at,&up);
		
		MatrixPerspectiveFOVLH(&m_matProject,PI*0.5f,(float)m_spMainWindow->m_iWidth/(float)
			m_spMainWindow->m_iHeight,1.0f,1000.0f);
		
		if(m_pDevice->BeginScene())
		{
		//	device_->SetWorld(&rote);
			m_pDevice->SetView(&m_matView);
			m_pDevice->SetProject(&m_matProject);
			m_pDevice->SetViewPort();
			m_pDevice->SetStreamSource(GetPaser()->GetVertexBuffer());
			m_pDevice->SetIndices(GetPaser()->GetIndexBuffer());
			m_pDevice->SetVertexDeclaration(GetPaser()->GetVertexDeclaration());
			m_pDevice->DrawIndexedPrimitive(PT_TRIANGLEIST,0,0,8,0,m_pFbxPaser->GetFaceNumber());
			m_pDevice->EndScene();
		}
		//////////////////////////////////////////////////////////////////////////
		//to dispaly FPS
		std::stringstream ss;
		std::string to_dispaly;
		ss<<"FPS:"<<GetFPS();
		to_dispaly=ss.str();
		m_pDevice->TextDraw(to_dispaly,0,0,_RGB(255,0,0));
		ss.str("");
		ss<<"Total face number:"<<m_pFbxPaser->GetFaceNumber();
		to_dispaly=ss.str();
		m_pDevice->TextDraw(to_dispaly,0,20,_RGB(0,255,0));
		//////////////////////////////////////////////////////////////////////////
		m_pDevice->Present();
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

	void Game::PreRender(float elpase_time)
	{
		//////////////////////////////////////////////////////////////////////////
		if(timeGetTime()-m_dwLastFrameTime>1000)
		{
			m_iFPS=m_iLastFrameCounts;
			m_iLastFrameCounts=0;
			m_dwLastFrameTime=timeGetTime();
		}
		else
		{
			m_iLastFrameCounts++;
		}

		//////////////////////////////////////////////////////////////////////////
		static bool first=true;
		if(first)
		{
			first=false;
			Quaternion q(2.4f,5.1f,8.3f,2.9f);
			cout<<q*q<<endl;
			Matrix m;
			MatrixRotationQuaternion(&m,&q);
			cout<<m<<endl;	
			float deter;
			MatrixInverse(&m,&deter,&m);
			cout<<m<<endl;
			MatrixInverse(&m,&deter,&m);
			cout<<m<<endl;

			cout<<deter<<endl;
		}
	}

}