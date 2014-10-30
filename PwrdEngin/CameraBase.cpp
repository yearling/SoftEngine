#include "StdAfx.h"
#include "CameraBase.h"
#include "Gamemath.h"
namespace SoftEngine
{
	CameraBase::CameraBase(void):eye_(0.0f,0.0f,0.0f),look_at_(0.0f,0.0f,0.0f),
		FOV_(0.0f),near_plane_(1.0f),far_plane_(1000.0f),width_(0.0f),height_(0.0f);
	{
		MatrixIdentity(&view_);
		MatrixIdentity(&project_);
		 FOV_;
		float aspect_;
		float near_plane_;
		float far_plane_;
		int width_;
		int height_;
	}


	CameraBase::~CameraBase(void)
	{
	}

}