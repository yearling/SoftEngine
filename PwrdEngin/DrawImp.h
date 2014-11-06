#ifndef __DRAWIMP_H__
#define __DRAWIMP_H__
#pragma once
#include "stdafx.h"
#include <atlcomcli.h>
#include <ddraw.h>

namespace SoftEngine
{
//#define  _ARGB(a,r,g,b) ((DWORD)((((a)&0xff))<<24) | (((r)&0xff)<<16) | (((g)&0xff)<<8)| ((b)&0xff)))
#define  _ARGB(a,r,g,b)   ((DWORD)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#define  _RGBA(r,g,b,a) _ARGB(a,r,g,b)
#define  _RGB(r,g,b) _ARGB(0xff,r,g,b)
	class DrawImp
	{
	public:
		DrawImp(void);
		~DrawImp(void);
		bool Init(HWND hwnd,int width,int height,int offsetx,int offsety,bool window=true);	
		void ClearSurface(CComPtr<IDirectDrawSurface7> surface,DWORD color);
		void ClearBackBuffer(DWORD color=_RGB(0,0,0));
		unsigned int *LockSurface(CComPtr<IDirectDrawSurface7> surface,int *pitch, int *width, int *height);
		void UnlockSurface(CComPtr<IDirectDrawSurface7> surface);
		unsigned int *LockBackSurface(int *pitch,int *width,int *height);
		void UnlockBackSurface();
		void Flip();
		void SetBackBufferCliper(RECT *rc=nullptr);
		void DrawTextGDI(const std::string &text, int x,int y,DWORD color);
	private:
		CComPtr<IDirectDraw7> d3d_draw_;
		CComPtr<IDirectDrawSurface7> ddsprimary_;
		CComPtr<IDirectDrawSurface7> ddsback_;  
		CComPtr<IDirectDrawClipper> ddcliper_back_surface_;
		CComPtr<IDirectDrawClipper> ddcliper_primer_surface_;
		DDSURFACEDESC2       ddsd_; 
		UINT *primary_buffer_;	
		UINT *back_buffer_; 
		int   primary_lpitch_;  
		int   back_lpitch_; 
		int width_;
		int height_;
		int client_offset_x_;
		int client_offset_y_;
		bool windowed_;
		HWND hwnd_;
	};
	
}

#endif