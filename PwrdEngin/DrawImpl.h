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
	class DrawImpl
	{
	public:
		DrawImpl(void);
		~DrawImpl(void);
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
		CComPtr<IDirectDraw7> m_comDDraw;
		CComPtr<IDirectDrawSurface7> m_comDDSprimary;
		CComPtr<IDirectDrawSurface7> m_comDDSBack;  
		CComPtr<IDirectDrawClipper> m_comDDcliperBackSurface;
		CComPtr<IDirectDrawClipper> m_comDDcliperPrimerSurface;
		DDSURFACEDESC2       m_ddsd; 
		UINT *m_pPrimaryBuffer;	
		UINT *m_pBackBuffer; 
		int   m_iPrimaryPitch;  
		int   m_iBackPitch; 
		int m_iWidth;
		int m_iHeight;
		int m_iClientOffsetX;
		int m_iClientOffsetY;
		bool bWindow;
		HWND m_hWnd;
	};
	
}

#endif