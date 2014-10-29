#ifndef __GAME_H__
#define __GAME_H__
#pragma once
#include "stdafx.h"
#include "application.h"
#include "DrawImp.h"
namespace SoftEngine
{
	class Game:public Application
	{
	public:
		Game();
		virtual void Initial();
		virtual int Run();
		virtual void Exit();
		void GameMain();
	protected:
		virtual LRESULT MyProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) throw();
		DrawImp draw_imp_;
	private:
		void AllocConsoleDebug();
	};
}
#endif