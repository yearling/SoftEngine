#ifndef __RENDER_H__
#define __RENDER_H__
#pragma once
#include "stdafx.h"
#include "DrawImp.h"
namespace SoftEngine
{
	class Render
	{
	public:
		Render();
		bool Init(DrawImp* draw_imp);
		bool BeginDraw();
		bool EndDraw();
		inline void DrawPixel(int x,int y,DWORD color=_RGB(255,255,255))
		{
			back_buffer_[x+(height_-y-1)*pitch]=color;
		}
		inline int Round(const float f) {return int(a+0.5);}
	private:
		DrawImp *draw_imp_;
		UINT *back_buffer_;
		int pitch;
		int width_;
		int height_;
	};
}
#endif