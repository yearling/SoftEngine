#ifndef __RENDER_H__
#define __RENDER_H__
#pragma once
#include "stdafx.h"
#include "DrawImp.h"
#include "application.h"
namespace SoftEngine
{
	class Render
	{
	public:
		Render();
		~Render();
		bool Init(DrawImp* draw_imp);
		bool Init(const Window *windows);
		bool BeginDraw();
		bool EndDraw();
		bool Clear(UINT color);
		bool Present();
		inline void DrawPixel(int x,int y,DWORD color=_RGB(255,255,255))
		{
			back_buffer_[x+y*pitch]=color;
		}
		inline int Round( float f) {return int(f+0.5);}
		void DrawLine(int x0,int y0,int x1,int y1,int color=_RGB(255,255,255));
		bool Draw2DClipe(const RECT &rc,int & x0,int &y0,int &x1,int &y1);
	private:
		DrawImp *draw_imp_;
		UINT *back_buffer_;
		int pitch;
		int width_;
		int height_;
		RECT clipe_rc;
	};
}
#endif