#ifndef __GAME_H__
#define __GAME_H__
#pragma once
#include "stdafx.h"
#include "application.h"
#include "DrawImpl.h"
#include "Device.h"
#include "FbxPaser.h"
#include "Camera.h"
#include "Parameter.h"
namespace SoftEngine
{
	class Game:public Application
	{
	public:
		Game();
		virtual void Initial();
		virtual int Run();
		virtual void Exit();
		void PreRender(float elpase_time);
		void Render(float elpase_time);
		FbxPaser *GetPaser() const{ return m_pFbxPaser;}
		int GetFPS() const {return m_iFPS;}
	protected:
		virtual LRESULT MyProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) throw();
		Device*  m_pDevice;
		VertexDeclaration *m_pVertexDecl;
		VertexBuffer* m_pVertex_buffer;
		IndexBuffer* m_pIndexBuffer;
		Matrix m_matWorld;
		Matrix m_matProject;
		Matrix m_matView;
		DWORD  m_dwLastTime;
		FbxPaser *m_pFbxPaser;
		int m_iLastFrameCounts;
		DWORD m_dwLastFrameTime;
		int m_iFPS;
		EASYCamera *m_pEasyCamera;
		IEffect *m_pEffect;
		SeftData m_sd;
	private:
		void AllocConsoleDebug();
	};
}
#endif