#ifndef __CAMERABASE_H__
#define __CAMERABASE_H__
#pragma once
#include "stdafx.h"
#include "Gamemath.h"
namespace SoftEngine
{

	class CameraBase
	{
	public:
		CameraBase(void);
		virtual ~CameraBase(void);
	private:
		Matrix view_;
		Matrix project_;
		Vector3 eye_;
		Vector3 look_at_;
		float FOV_;
		float aspect_;
		float near_plane_;
		float far_plane_;
		int width_;
		int height_;
	};
}
#endif
