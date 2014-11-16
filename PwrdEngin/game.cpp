#include "stdafx.h"
#include "game.h"
#include "Gamemath.h"
#include "Device.h"
#include <stdio.h>
#include <locale.h>
#include <iostream>
#include <mmsystem.h>
#include <sstream>
#include "LightEffect.h"
#include "BMPReader.h"
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
		m_pDevice->SetViewPort();
		m_pVS=new BisicalVertexShader(m_pDevice);
		m_pPS=new GroundPixelShader(m_pDevice);
		//////////////////////////////////////////////////////////////////////////
		m_pFbxPaser=new FbxPaser();
		m_pFbxPaser->Init(m_pDevice);
		//m_pFbxPaser->Load("..\\media\\box.fbx");
		//m_pFbxPaser->Load("E:\\scene_fbx\\test\\box_normal.fbx");
		//m_pFbxPaser->Load("E:\\scene_fbx\\ring.fbx");
		//m_pFbxPaser->Load("E:\\scene_fbx\\test\\pyramid.fbx");
		//m_pFbxPaser->Load("E:\\scene_fbx\\test\\rectangle.fbx");
		//m_pFbxPaser->Load("E:\\scene_fbx\\test\\box_texture.fbx");
		//m_pFbxPaser->Load("E:\\scene_fbx\\test\\plane_texture.fbx");
		m_pFbxPaser->Load("E:\\scene_fbx\\test\\tea.fbx");
		//m_pFbxPaser->Load("E:\\scene_fbx\\test\\plane2X2.fbx");
		//m_pFbxPaser->Load("E:\\scene_fbx\\test\\triangle_mesh.fbx");
		//m_pFbxPaser->Load("E:\\scene_fbx\\test\\ball_ground.fbx");
		//m_pFbxPaser->Load("E:\\scene_fbx\\test\\ball_ground_sm.fbx");
		//m_pFbxPaser->Load("E:\\scene_fbx\\test\\plane20bump.fbx");
		//m_pFbxPaser->Load("E:\\scene_fbx\\test\\plane2x2bump.fbx");
		//m_pFbxPaser->Load("E:\\scene_fbx\\test\\teaNormal.fbx");
		//m_pFbxPaser->Load("E:\\scene_fbx\\test\\ballNormal.fbx");
		//m_pFbxPaser->Load("E:\\scene_fbx\\test\\ball_test.fbx");
		//m_pFbxPaser->Load("E:\\scene_fbx\\test\\rectangle_normal.fbx");
		//////////////////////////////////////////////////////////////////////////
		m_pEasyCamera=new EASYCamera();
		m_pEasyCamera->SetHWND(m_spMainWindow->m_hWnd);
		m_pEasyCamera->SetWindow(width,height);
		m_pEasyCamera->SetRaius(40.0f);
		Vector3 eye(0.0f,0.0f,-600.0f);
		Vector3 at(0.0f,0.0f,0.0f);
		Vector3 up(0.0f,1.0f,0.0f);
		m_pEasyCamera->SetViewParam(&eye,&at);
		m_pEasyCamera->SetProjParam(PI*0.5f,(float)m_spMainWindow->m_iWidth/(float)
			m_spMainWindow->m_iHeight,1.0f,1000.0f);
		m_pBMP=BMPReader::GetInstance().LoadBMP("E:\\scene_fbx\\test\\wall.bmp");
		//m_pBMP=BMPReader::GetInstance().LoadBMP("E:\\scene_fbx\\test\\wall_N_flat.bmp");
		m_pNormalBMP=BMPReader::GetInstance().LoadBMP("E:\\scene_fbx\\test\\wall_N.bmp");
		//m_pNormalBMP=BMPReader::GetInstance().LoadBMP("E:\\scene_fbx\\test\\wall_N_flat.bmp");
		m_pSamper=new TextureSampler();
		m_pNormalMapSamper=new TextureSampler();
		m_pSamper->SetBMP(m_pBMP);
		m_pNormalMapSamper->SetBMP(m_pNormalBMP);
	}
	void Game::Render(float elpase_time)
	{
		static bool iOnceTime=true;
		if(iOnceTime)
		{

			m_pDevice->Clear(_RGB(25,25,25));
			if(m_pDevice->BeginScene())
			{
				m_pDevice->SetStreamSource(GetPaser()->GetVertexBuffer());
				m_pDevice->SetIndices(GetPaser()->GetIndexBuffer());
				m_pDevice->SetVertexDeclaration(GetPaser()->GetVertexDeclaration());
				m_pDevice->SetPS(m_pPS);
				m_pDevice->SetVS(m_pVS);
				m_pDevice->PSSetData();
				m_pDevice->VSSetData();
				//m_pDevice->SetCullMode(CULL_NONE);
				m_pDevice->SetFillMode(FILL_SOLID);
				//m_pDevice->SetFillMode(FILL_WIREFRAME);
				m_pDevice->DrawIndexedPrimitive(PT_TRIANGLEIST,0,0,m_pFbxPaser->GetVertexNumber(),0,m_pFbxPaser->GetFaceNumber());
				/*for(int i=0;i<200;i++)
					m_pDevice->DrawPixel(i,i,_RGB(255,0,0));*/
				m_pDevice->EndScene();

			}
			//iOnceTime=false;
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
		m_pDevice->TextDraw(m_strLight,0,30,_RGB(255,0,255));
		//////////////////////////////////////////////////////////////////////////
		m_pDevice->Present();
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
		m_pEasyCamera->FrameMove(elpase_time);
		//////////////////////////////////////////////////////////////////////////
		static float angle=0.0f;
		angle+=elpase_time*PI*0.2;
		if(angle>PI*2)
			angle=0.0f;
		Matrix rote;
		MatrixIdentity(&rote);
		MatrixRotationY(&rote,angle);
		memset(&m_sd,0,sizeof(SeftData));
		m_sd.world=*m_pEasyCamera->GetWorldMatrix();
		m_sd.view=*m_pEasyCamera->GetViewMatrix();
		m_sd.project=*m_pEasyCamera->GetProjMatrix();
		m_sd.viewPort=*m_pDevice->GetViewPort();
		m_sd.m_pSamper=m_pSamper;
		m_sd.m_pNormalSamper=m_pNormalMapSamper;
		Matrix matIT;
		MatrixInverse(&matIT,nullptr,&m_sd.world);
		MatrixTranspose(&matIT,&matIT);
		m_sd.matWorldInverseTranspose=matIT;
		m_sd.direct=_RGB(255,255,255);
		m_sd.ambient=_RGB(255,255,255);
		m_sd.fAmbientScalar=0.0f;
		m_sd.fDirectScalar=2.0f;
		static Vector3 lightdirect(0,0,1);
		if(m_bKeys[0])
			lightdirect.x+=0.1f;
		if(m_bKeys[1])
			lightdirect.y+=0.1f;
		if(m_bKeys[2])
			lightdirect.z+=0.1f;
		if(m_bKeys[3])
			lightdirect.x-=0.1f;
		if(m_bKeys[4])
			lightdirect.y-=0.1f;
		if(m_bKeys[5])
			lightdirect.z-=0.1f;
		std::stringstream ss;
		ss<<"Light position: "<<lightdirect.x<<"    "<<lightdirect.y<<"    "<<lightdirect.z;
		Normalize(&m_sd.vDirect,&lightdirect);
		m_pDevice->SetGameSource((void*)&m_sd);
		m_strLight.clear();
		m_strLight=ss.str();
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
		m_pEasyCamera=nullptr;
		m_pFbxPaser=nullptr;
		m_pPS=nullptr;
		m_pVS=nullptr;
		m_pSamper=nullptr;
		m_pNormalMapSamper=nullptr;
		m_pNormalBMP=nullptr;
		m_pBMP=nullptr;
		m_pNormalBMP=nullptr;
		for(int i=0;i<10;i++)
			m_bKeys[i]=false;
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
		if(m_pEasyCamera)
		{
			if(m_pEasyCamera->HandleMessage(hwnd,msg,wParam,lParam))
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
				case 'J':
					{
						m_bKeys[0]=true;
						break;
					}
				case 'K':
					{
						m_bKeys[1]=true;
						break;
					}
				case 'L':
					{
						m_bKeys[2]=true;
						break;
					}
				case 'U':
					{
						m_bKeys[3]=true;
						break;
					}
				case 'I':
					{
						m_bKeys[4]=true;
						break;
					}
				case 'O':
					{
						m_bKeys[5]=true;
						break;
					}
				}
				break;
			}
		case WM_KEYUP:
			{
				switch (wParam)
				{
				case 'J':
					{
						m_bKeys[0]=false;
						break;
					}
				case 'K':
					{
						m_bKeys[1]=false;
						break;
					}
				case 'L':
					{
						m_bKeys[2]=false;
						break;
					}
				case 'U':
					{
						m_bKeys[3]=false;
						break;
					}
				case 'I':
					{
						m_bKeys[4]=false;
						break;
					}
				case 'O':
					{
						m_bKeys[5]=false;
						break;
					}
				default:
					break;
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