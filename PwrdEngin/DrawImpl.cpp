#include "stdafx.h"
#include "DrawImp.h"
#include <assert.h>

namespace SoftEngine
{

	DrawImpl::DrawImpl(void):m_pPrimaryBuffer(nullptr),
		m_pBackBuffer(nullptr),
		m_iPrimaryPitch(0),
		m_iBackPitch(0),
		m_iWidth(0),
		m_iHeight(0),
		bWindow(true),
		m_hWnd(0),
		m_iClientOffsetX(0),
		m_iClientOffsetY(0)
	{
	}


	DrawImpl::~DrawImpl(void)
	{
	}

	bool DrawImpl::Init(HWND hwnd,int width,int height,int x_offset,int y_offset,bool window/*=true*/)
	{
		HRESULT hr;
		if(FAILED(hr=DirectDrawCreateEx(NULL,(void**)&m_comDDraw,IID_IDirectDraw7,NULL)))
			return false;
		if(window)
		{
			if(FAILED(hr=m_comDDraw->SetCooperativeLevel(hwnd,DDSCL_NORMAL)))
				return false;
		}
		else 
			return false;
		m_iWidth=width;
		m_iHeight=height;
		bWindow=window;
		m_hWnd=hwnd;
		m_iClientOffsetX=x_offset;
		m_iClientOffsetY=y_offset;
		memset(&m_ddsd,0,sizeof(m_ddsd));
		m_ddsd.dwSize=sizeof(m_ddsd);
		if(bWindow)
		{
			m_ddsd.dwFlags = DDSD_CAPS;
			m_ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
			m_ddsd.dwBackBufferCount = 0;
		}
		m_comDDraw->CreateSurface(&m_ddsd,&m_comDDSprimary,NULL);
		DDPIXELFORMAT ddpf;
		memset(&ddpf,0,sizeof(ddpf));
		ddpf.dwSize=sizeof(ddpf);
		m_comDDSprimary->GetPixelFormat(&ddpf);
		if(32!=ddpf.dwRGBBitCount)
			return false;
		memset(&m_ddsd,0,sizeof(m_ddsd));
		m_ddsd.dwSize=sizeof(m_ddsd);
		m_ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
		m_ddsd.dwWidth  =  m_iWidth;
		m_ddsd.dwHeight =  m_iHeight;
		m_ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN |DDSCAPS_VIDEOMEMORY;
		if(FAILED(hr=m_comDDraw->CreateSurface(&m_ddsd,&m_comDDSBack,nullptr)))
			return false;
		RECT screen_rect = {0,0,m_iWidth,m_iHeight};
		SetBackBufferCliper(&screen_rect);
		if (bWindow)
		{
			if (FAILED(m_comDDraw->CreateClipper(0,&m_comDDcliperPrimerSurface,NULL)))
				return(false);

			if (FAILED(m_comDDcliperPrimerSurface->SetHWnd(0, m_hWnd)))
				return(false);

			if (FAILED(m_comDDSprimary->SetClipper(m_comDDcliperPrimerSurface)))
				return(false);
		} // end if screen windowed
		return true;
	}

	void DrawImpl::ClearSurface(CComPtr<IDirectDrawSurface7> surface,DWORD color)
	{
		DDBLTFX ddbltfx; 
		memset(&ddbltfx,0,sizeof(ddbltfx));
		ddbltfx.dwFillColor = color; 
		surface->Blt(0, NULL,  NULL,  DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx); 
	}

	unsigned int * DrawImpl::LockSurface(CComPtr<IDirectDrawSurface7> surface,int *pitch, int *width, int *height)
	{
		if (!surface)
			return(NULL);
		DDSURFACEDESC2 ddsd;
		memset(&ddsd,0,sizeof(ddsd));
		surface->Lock(NULL,&ddsd,DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR,NULL); 
		if (pitch)
			*pitch = ddsd.lPitch;
		if(width)
			*width=ddsd.dwWidth;
		if(height)
			*height=ddsd.dwHeight;
		return (unsigned int *)ddsd.lpSurface;
	}

	void DrawImpl::UnlockSurface(CComPtr<IDirectDrawSurface7> surface)
	{
		if(surface)
			surface->Unlock(nullptr);
	}

	void DrawImpl::Flip()
	{
		if (m_pPrimaryBuffer || m_pBackBuffer)
			return ;
		RECT    dest_rect; 
		GetWindowRect(m_hWnd, &dest_rect); 
		dest_rect.left+=m_iClientOffsetX;
		dest_rect.top+=m_iClientOffsetY;
		dest_rect.right=dest_rect.left+m_iWidth-1;
		dest_rect.bottom=dest_rect.top+m_iHeight-1;
		if (FAILED(m_comDDSprimary->Blt(&dest_rect, m_comDDSBack,NULL,DDBLT_WAIT,NULL)))
			return ;    
	}

	void DrawImpl::SetBackBufferCliper(RECT *rc)
	{
		if(rc==nullptr)
			return;
		LPRGNDATA region_data; 
		if (FAILED(m_comDDraw->CreateClipper(0,&m_comDDcliperBackSurface,NULL)))
			return ;
		region_data = (LPRGNDATA)malloc(sizeof(RGNDATAHEADER)+sizeof(RECT));
		memcpy(region_data->Buffer, rc, sizeof(RECT));
		region_data->rdh.dwSize          = sizeof(RGNDATAHEADER);
		region_data->rdh.iType           = RDH_RECTANGLES;
		region_data->rdh.nCount          = 1;
		region_data->rdh.nRgnSize        = sizeof(RECT);
		region_data->rdh.rcBound.left    =  rc->left;
		region_data->rdh.rcBound.top     =  rc->top;
		region_data->rdh.rcBound.right   = rc->right;
		region_data->rdh.rcBound.bottom  = rc->bottom;
		if(FAILED(m_comDDcliperBackSurface->SetClipList(region_data,0)))
		{
			free(region_data);
		}
		if(FAILED(m_comDDSBack->SetClipper(m_comDDcliperBackSurface)))
		{
			free(region_data);
		}
		free(region_data);
	}

	unsigned int * DrawImpl::LockBackSurface(int *pitch,int *width,int *height)
	{
		HRESULT hr;
		if (m_comDDSBack==nullptr)
			return(NULL);
		DDSURFACEDESC2 ddsd;
		memset(&ddsd,0,sizeof(ddsd));
		ddsd.dwSize=sizeof(ddsd);
		hr=m_comDDSBack->Lock(NULL,&ddsd,DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR,NULL); 
		if (pitch)
			*pitch = ddsd.lPitch>>2;
		if(width)
			*width=ddsd.dwWidth;
		if(height)
			*height=ddsd.dwHeight;
		return (unsigned int *)ddsd.lpSurface;
	}

	void DrawImpl::UnlockBackSurface()
	{
		UnlockSurface(m_comDDSBack);
	}

	void DrawImpl::ClearBackBuffer(DWORD color/*=_RGB(0,0,0)*/)
	{
		if(m_comDDSBack)
		{
			DDBLTFX ddbltfx; 
			memset(&ddbltfx,0,sizeof(ddbltfx));
			ddbltfx.dwSize=sizeof(ddbltfx);
			ddbltfx.dwFillColor = color; 
			m_comDDSBack->Blt(0, NULL,  NULL,  DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
		}
	}

	void DrawImpl::DrawTextGDI(const std::string &text, int x,int y,DWORD color)
	{
		HDC xdc; 
		if (FAILED(m_comDDSBack->GetDC(&xdc)))
			return;
		//SetTextColor参数是COLORREF是大端位的，用RGB宏来处理。
		SetTextColor(xdc,RGB(((color>>16) & 0xff),((color>>8) & 0xff),(color & 0xff)));
		SetBkMode(xdc, TRANSPARENT);
		TextOutA(xdc,x,y,text.c_str(),text.size());
		m_comDDSBack->ReleaseDC(xdc);
	}




}
