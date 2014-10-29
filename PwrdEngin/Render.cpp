#include "StdAfx.h"
#include "Render.h"
#include <assert.h>
#include <math.h>
namespace SoftEngine
{
	Render::Render():draw_imp_(nullptr),back_buffer_(nullptr),pitch(0),width_(0),height_(0)
	{
		memset(&clipe_rc,0,sizeof(clipe_rc));
	}

	bool Render::Init(DrawImp* draw_imp)
	{
		draw_imp_=draw_imp;
		return true;
	}

	bool Render::BeginDraw()
	{
		assert(draw_imp_&&"render should init");
		if(back_buffer_=draw_imp_->LockBackSurface(&pitch,&width_,&height_))
		{
			clipe_rc.left=0;
			clipe_rc.top=0;
			clipe_rc.right=width_-1;
			clipe_rc.bottom=height_-1;
			return true;
		}
		return false;
	}

	bool Render::EndDraw()
	{
		draw_imp_->UnlockBackSurface();
		pitch=0;
		width_=0;
		height_=0;
		back_buffer_=nullptr;
		memset(&clipe_rc,0,sizeof(clipe_rc));
		return true;
	}

	void Render::DrawLine(int x0,int y0,int x1,int y1,int color)
	{
		if(!Draw2DClipe(clipe_rc,x0,y0,x1,y1))
			return;
		if(x0==x1 && y0==y1)
		{
			DrawPixel(x0,y0,color);
			return;
		}
		int dx,dy;
		dx=x1-x0;
		dy=y1-y0;
		int step;
		abs(dx)>abs(dy)?step=abs(dx):step=abs(dy);
		float x_add=static_cast<float>(dx)/static_cast<float>(step);
		float y_add=static_cast<float>(dy)/static_cast<float>(step);
		float x=x0;
		float y=y0;
		for(int i=0;i<step;i++)
		{
			DrawPixel(Round(x),Round(y),color);
			x+=x_add;
			y+=y_add;
		}

	}
	inline void linerLerp(const int & x0,const int &y0,const int& x1,const int &y1,float lerp,int &des_x,int &des_y)
	{
			des_x=x0+(x1-x0)*lerp;
			des_y=y0+(y1-y0)*lerp;
	}
	bool Render::Draw2DClipe(const RECT &rc,int & x0,int &y0,int &x1,int &y1)
	{
		if(x0<rc.left && x1<rc.left)
			return false;
		else if(x0<rc.left)
		{
			y0=y0+(float)(y1-y0)*(float)(rc.left-x0)/float(x1-x0);
			x0=rc.left;
		}
		else if(x1<rc.left)
		{
			y1=y1+float(y0-y1)*float(rc.left-x1)/float(x0-x1);
			x1=rc.left;
		}
		if(y0<rc.top && y1<rc.top)
			return false;
		else if(y0<rc.top)
		{
			x0=x0+float(x1-x0)*float(rc.top-y0)/float(y1-y0);
			y0=rc.top;
		}
		else if(y1<rc.top)
		{
			x1=x1+float(x0-x1)*float(rc.top-y1)/float(y0-y1);
			y1=rc.top;
		}

		if(x0 >rc.right && x1>rc.right)
			return 0;
		else if(x0> rc.right)
		{
			y0=y0+float(y1-y0)*float(x0-rc.right)/float(x0-x1);
			x0=rc.right;
		}
		else if(x1>rc.right)
		{
			y1=y1+float(y0-y1)*float(x1-rc.right)/float(x1-x0);
			x1=rc.right;
		}

		if(y0>rc.bottom && y1>rc.bottom)
			return false;
		else if(y0>rc.bottom)
		{
			x0=x0+float(x1-x0)*float(y0-rc.bottom)/float(y0-y1);
			y0=rc.bottom;
		}
		else if(y1>rc.bottom)
		{
			x1=x1+float(x0-x1)*float(y1-rc.bottom)/float(y1-y0);
			y1=rc.bottom;
		}
		return true;
	}

	

}