#ifndef __GAME_H__
#define __GAME_H__
#pragma once
#include "stdafx.h"
#include "application.h"
#include "DrawImp.h"
#include "Device.h"
namespace SoftEngine
{
	class Game:public Application
	{
	public:
		Game();
		virtual void Initial();
		virtual int Run();
		virtual void Exit();
		void GameMain(float elpase_time);
	protected:
		virtual LRESULT MyProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) throw();
		Device*  device_;
		VertexDeclaration *vertex_decl_;
		VertexBuffer* vertex_buffer_;
		IndexBuffer* index_buffer_;
		Matrix world_;
		Matrix proj_;
		Matrix view_;
		DWORD  last_time_;
	private:
		void AllocConsoleDebug();
	};
}
#endif