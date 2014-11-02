#include "StdAfx.h"
#include "CameraBase.h"
#include "Gamemath.h"
namespace SoftEngine
{
	CameraBase::CameraBase(void):eye_(0.0f,0.0f,0.0f),look_at_(0.0f,0.0f,0.0f),
		FOV_(0.0f),near_plane_(1.0f),far_plane_(1000.0f),width_(0.0f),height_(0.0f)
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


	Quaternion AArcBall::QuaternionFromBallPoints(const Vector3 &from,const Vector3 & to)
	{
		Vector3 part;
		//from 和 to 都是单位向量，所以点乘的结果就是cos⁡θ
		float fdot=from*to;
		//from 和 to 都是单位向量，所点叉乘的结果可能不是单位向量，模为|A||B|*sin⁡θ
		//所以A/sin⁡θ才得到单位向量，然后四元数的向量部分是A*sin⁡θ，所以就是quaternion(AXB,A.B)的结果了。
		part=from^to;
		return Quaternion(part.x,part.y,part.z,fdot);
	}

	Vector3 AArcBall::ScreenToVector(float streen_x,float screen_y)
	{
		//设为左手坐标系，前Z正，右X正，上Y正，这样子我们操作的是负Z半轴的那个球
		float x=(streen_x-offset_.x-width_/2)/(radius_*width_/2);
		float y=-(screen_y-offset_.y-height_/2)/(radius_*height_/2);
		float z=0.0f;
		float mag=x*x+y*y;
		if(mag>1.0f)
		{
			float scale=1.0f/sqrtf(mag);
			x*=scale;
			y*=scale;
		}
		else
			z=-sqrtf(1.0f-mag);
		return Vector3(x,y,z);
	}

	AArcBall::AArcBall()
	{
		Reset();
		down_vec_=Vector3(0,0,0);
		now_vec_=Vector3(0,0,0);
		offset_.x=offset_.y=0;
	}

	void AArcBall::Reset()
	{
		QuaternionIdentity(&pre_rotate_);
		QuaternionIdentity(&now_qua_);
		MatrixIdentity(&rotation_);
		MatrixIdentity(&translation_);
		MatrixIdentity(&translation_delta_);
		drag_=false;
		radius_=1.0f;
		radius_tranlation_=1.0f;
	}

	void AArcBall::Init(HWND hwnd)
	{
		RECT rc;
		GetClientRect(hwnd,&rc);
		SetWindow(rc.right,rc.bottom);	
	}

	void AArcBall::SetWindow(int width,int height,float radius/*=0.9f*/)
	{
		width_=width;
		height_=height;
		radius_=radius;
		center_=Vector2(width_/2.0f,height_/2.0f);
	}

	void AArcBall::OnBegin(int x,int y)
	{
		if(x>=offset_.x && x<=offset_.x+width_ && y>=offset_.y && y<=offset_.y+height_)
		{
			drag_=true;
			//down_qua_用来记录上次旋转的结果
			pre_rotate_=now_qua_;
			//down_vec_靠这个来标识起点
			down_vec_=ScreenToVector((float)x,(float)y);
		}
	}

	void AArcBall::OnMove(int x,int y)
	{
		if(drag_)
		{
			now_vec_=ScreenToVector((float)x,(float)y);
			now_qua_=pre_rotate_*QuaternionFromBallPoints(down_vec_,now_vec_);
		}
	}

	void AArcBall::OnEnd()
	{
		drag_=false;
	}

	bool AArcBall::HandleMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
	{
		int mouse_x=(short)LOWORD(lParam);
		int mouse_y=(short)HIWORD(lParam);
		switch(uMsg)
		{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
			SetCapture(hwnd);
			OnBegin(mouse_x,mouse_y);
			return true;
		case WM_LBUTTONUP:
			ReleaseCapture();
			OnEnd();
			return true;
		case WM_CAPTURECHANGED:
			if((HWND)lParam!=hwnd)
			{
				ReleaseCapture();
				OnEnd();
			}
			return true;
		case WM_RBUTTONDOWN:
		case WM_RBUTTONDBLCLK:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONDBLCLK:
			SetCapture(hwnd);
			last_mouse_.x=mouse_x;
			last_mouse_.y=mouse_y;
			return true;
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
			ReleaseCapture();
			return true;
		case WM_MOUSEMOVE:
			if(MK_LBUTTON &wParam)
			{
				OnMove(mouse_x,mouse_y);
			}
			else if((MK_RBUTTON & wParam )||(MK_MBUTTON & wParam))
			{
				float delta_x=(last_mouse_.x-mouse_x)*radius_tranlation_/width_;
				float delta_y=(last_mouse_.y-mouse_y)*radius_tranlation_/height_;
				if(wParam & MK_RBUTTON)
				{
					MatrixTranslation(&translation_delta_,-2*delta_x,2*delta_y,0.0f);
					MatrixMultiply(&translation_,&translation_,&translation_delta_);
				}
				else
				{
					MatrixTranslation(&translation_delta_,0.0f,0.0f,5*delta_y);
					MatrixMultiply(&translation_,&translation_,&translation_delta_);
				}
			}
			return true;
		}
		return false;
	}

}