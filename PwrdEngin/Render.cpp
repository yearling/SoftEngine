#include "StdAfx.h"
#include "Render.h"
#include <assert.h>

namespace SoftEngine
{
	Render::Render():draw_imp_(nullptr),back_buffer_(nullptr),pitch(0),width_(0),height_(0)
	{

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
		return true;
	}
}