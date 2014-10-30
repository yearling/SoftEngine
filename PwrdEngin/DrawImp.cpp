#include "stdafx.h"
#include "DrawImp.h"
#include <assert.h>

namespace SoftEngine
{

	DrawImp::DrawImp(void):primary_buffer_(nullptr),
		back_buffer_(nullptr),
		primary_lpitch_(0),
		back_lpitch_(0),
		width_(0),
		height_(0),
		windowed_(true),
		hwnd_(0),
		client_offset_x_(0),
		client_offset_y_(0)
	{
	}


	DrawImp::~DrawImp(void)
	{
	}

	bool DrawImp::Init(HWND hwnd,int width,int height,int x_offset,int y_offset,bool window/*=true*/)
	{
		HRESULT hr;
		if(FAILED(hr=DirectDrawCreateEx(NULL,(void**)&d3d_draw_,IID_IDirectDraw7,NULL)))
			return false;
		if(window)
		{
			if(FAILED(hr=d3d_draw_->SetCooperativeLevel(hwnd,DDSCL_NORMAL)))
				return false;
		}
		else 
			return false;
		width_=width;
		height_=height;
		windowed_=window;
		hwnd_=hwnd;
		client_offset_x_=x_offset;
		client_offset_y_=y_offset;
		memset(&ddsd_,0,sizeof(ddsd_));
		ddsd_.dwSize=sizeof(ddsd_);
		if(windowed_)
		{
			ddsd_.dwFlags = DDSD_CAPS;
			ddsd_.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
			ddsd_.dwBackBufferCount = 0;
		}
		d3d_draw_->CreateSurface(&ddsd_,&ddsprimary_,NULL);
		DDPIXELFORMAT ddpf;
		memset(&ddpf,0,sizeof(ddpf));
		ddpf.dwSize=sizeof(ddpf);
		ddsprimary_->GetPixelFormat(&ddpf);
		if(32!=ddpf.dwRGBBitCount)
			return false;
		memset(&ddsd_,0,sizeof(ddsd_));
		ddsd_.dwSize=sizeof(ddsd_);
		ddsd_.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
		ddsd_.dwWidth  =  width_;
		ddsd_.dwHeight =  height_;
		ddsd_.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN |DDSCAPS_VIDEOMEMORY;
		if(FAILED(hr=d3d_draw_->CreateSurface(&ddsd_,&ddsback_,nullptr)))
			return false;
		RECT screen_rect = {0,0,width_,height_};
		SetBackBufferCliper(&screen_rect);
		if (windowed_)
		{
			if (FAILED(d3d_draw_->CreateClipper(0,&ddcliper_primer_surface_,NULL)))
				return(false);

			if (FAILED(ddcliper_primer_surface_->SetHWnd(0, hwnd_)))
				return(false);

			if (FAILED(ddsprimary_->SetClipper(ddcliper_primer_surface_)))
				return(false);
		} // end if screen windowed
		return true;
	}

	void DrawImp::ClearSurface(CComPtr<IDirectDrawSurface7> surface,DWORD color)
	{
		DDBLTFX ddbltfx; 
		memset(&ddbltfx,0,sizeof(ddbltfx));
		ddbltfx.dwFillColor = color; 
		surface->Blt(0, NULL,  NULL,  DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx); 
	}

	unsigned int * DrawImp::LockSurface(CComPtr<IDirectDrawSurface7> surface,int *pitch, int *width, int *height)
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

	void DrawImp::UnlockSurface(CComPtr<IDirectDrawSurface7> surface)
	{
		if(surface)
			surface->Unlock(nullptr);
	}

	void DrawImp::Flip()
	{
		if (primary_buffer_ || back_buffer_)
			return ;
		RECT    dest_rect; 
		GetWindowRect(hwnd_, &dest_rect); 
		dest_rect.left+=client_offset_x_;
		dest_rect.top+=client_offset_y_;
		dest_rect.right=dest_rect.left+width_-1;
		dest_rect.bottom=dest_rect.top+height_-1;
		if (FAILED(ddsprimary_->Blt(&dest_rect, ddsback_,NULL,DDBLT_WAIT,NULL)))
			return ;    
	}

	void DrawImp::SetBackBufferCliper(RECT *rc)
	{
		if(rc==nullptr)
			return;
		LPRGNDATA region_data; 
		if (FAILED(d3d_draw_->CreateClipper(0,&ddcliper_back_surface_,NULL)))
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
		if(FAILED(ddcliper_back_surface_->SetClipList(region_data,0)))
		{
			free(region_data);
		}
		if(FAILED(ddsback_->SetClipper(ddcliper_back_surface_)))
		{
			free(region_data);
		}
		free(region_data);
	}

	unsigned int * DrawImp::LockBackSurface(int *pitch,int *width,int *height)
	{
		HRESULT hr;
		if (ddsback_==nullptr)
			return(NULL);
		DDSURFACEDESC2 ddsd;
		memset(&ddsd,0,sizeof(ddsd));
		ddsd.dwSize=sizeof(ddsd);
		hr=ddsback_->Lock(NULL,&ddsd,DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR,NULL); 
		if (pitch)
			*pitch = ddsd.lPitch>>2;
		if(width)
			*width=ddsd.dwWidth;
		if(height)
			*height=ddsd.dwHeight;
		return (unsigned int *)ddsd.lpSurface;
	}

	void DrawImp::UnlockBackSurface()
	{
		UnlockSurface(ddsback_);
	}

	void DrawImp::ClearBackBuffer(DWORD color/*=_RGB(0,0,0)*/)
	{
		if(ddsback_)
		{
			DDBLTFX ddbltfx; 
			memset(&ddbltfx,0,sizeof(ddbltfx));
			ddbltfx.dwSize=sizeof(ddbltfx);
			ddbltfx.dwFillColor = color; 
			ddsback_->Blt(0, NULL,  NULL,  DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
		}
	}




}
