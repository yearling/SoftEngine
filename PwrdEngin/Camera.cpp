#include "StdAfx.h"
#include "Camera.h"
#include "Gamemath.h"
namespace SoftEngine
{
	//CameraBase::CameraBase(void)
	//{
	//	keys_down_=0;
	//	ZeroMemory(key_mask_,sizeof(byte)*CAM_MAX_KEYS);
	//	Vector3 eye=Vector3(0.0f,0.0f,0.0f);
	//	Vector3 lookat=Vector3(0.0f,0.0f,0.0f);
	//	SetViewParam(&eye,&lookat);
	//	SetProjParam(PI/4,1.0f,1.0f,1000.f);
	//	GetCursorPos(&last_mouse_position_);
	//	mouse_lbutton_down_=false;
	//	mouse_mbutton_down_=false;
	//	mouse_rbutton_down_=false;
	//	current_button_mask_=0;
	//	mouse_wheel_delta_=0;
	//	camera_yaw_angle_=0.0f;
	//	camera_pitch_angle_=0.0f;
	//	SetRect(&drag_rc_,LONG_MIN,LONG_MIN,LONG_MAX,LONG_MAX);
	//	velocity_=Vector3(0.0f,0.0f,0.0f);
	//	movement_drag_=false;
	//	velocity_drag_=Vector3(0,0,0);
	//	drag_timer_=0.0f;
	//	total_drag_time_to_zero_=0.25;
	//	rot_velocity=Vector2(0,0);
	//	rotation_scaler_=0.01f;
	//	move_scaler_=5.0f;
	//	invert_pitch_=false;
	//	enable_position_movement_=true;
	//	enable_Y_axis_movement_=true;
	//	mouse_delta_=Vector2(0,0);
	//	frames_to_smooth_mouse_delta_=2.0f;
	//	clip_to_boundary_=false;
	//	min_boundary_=Vector3(-1,-1,-1);
	//	max_boundary_=Vector3(1,1,1);
	//	reset_cursor_after_move_=false;
	//	hwnd_=NULL;
	//}


	//CameraBase::~CameraBase(void)
	//{
	//}

	//void CameraBase::SetViewParam(Vector3* eye,Vector3 *lookat)
	//{
	//	if(NULL==eye || NULL==lookat)
	//		return;
	//	default_eye_=eye_=*eye;
	//	default_lookat_=lookat_= *lookat;
	//	Vector3 up(0,1,0);
	//	MatrixLookAtLH(&view_,&eye_,&lookat_,&up);
	//	//先pitch 再yaw
	//	//得到的view其实是视坐标基的逆，所以要先进行一次逆得到视坐标的基
	//	//然后得到Z轴坐标，进行计算。
	//	//因为是先pitch再yaw，也就是说，先绕x轴，再绕y轴把z轴从世界坐标绕到现在的视坐标
	//	//旋转保持轴长度不变和向量到旋转轴的距离不变。
	//	//1.绕X轴把Z轴转到YZ平面
	//	//2.绕Y轴把Z轴转到位置
	//	//如果要计算的话要反过来算,先算2
	//	//就是Z轴现在的位置绕Y轴转到YZ平面上，投影到XZ平面上，角度为-arctan(x/z),反过来也就是2，就是arctan(x/z)
	//	//然后因为刚才绕Y轴旋转，投影到XZ平面上的sqrt(x^2+z^2)长度不变，就是现在Y轴投到Z轴上的长度，
	//	//所以绕X轴的旋角为arctan(y,sqrt(x^2+z^2)),角度从Z到Y，反方向，取负
	//	Matrix inverse_view;
	//	D3DXMatrixInverse(&inverse_view,NULL,&view_);
	//	Vector3 * z_basis=(Vector3)&inverse_view._31;
	//	camera_yaw_angle_=atan2f(z_basis->x,z_basis->z);
	//	float len=sqrtf(z_basis->z*z_basis->z+z_basis->x*z_basis->x);
	//	camera_pitch_angle_=-atan2f(z_basis->y,len);
	//}


	Quaternion ArcBall::QuaternionFromBallPoints(const Vector3 &from,const Vector3 & to)
	{
		Vector3 part;
		//from 和 to 都是单位向量，所以点乘的结果就是cos⁡θ
		float fdot=from*to;
		//from 和 to 都是单位向量，所点叉乘的结果可能不是单位向量，模为|A||B|*sin⁡θ
		//所以A/sin⁡θ才得到单位向量，然后四元数的向量部分是A*sin⁡θ，所以就是quaternion(AXB,A.B)的结果了。
		part=from^to;
		return Quaternion(part.x,part.y,part.z,fdot);
	}

	Vector3 ArcBall::ScreenToVector(float streen_x,float screen_y)
	{
		//设为左手坐标系，前Z正，右X正，上Y正，这样子我们操作的是负Z半轴的那个球
		float x=(streen_x-m_Offset.x-m_iWidth/2)/(m_fRadius*m_iWidth/2);
		float y=-(screen_y-m_Offset.y-m_iHeight/2)/(m_fRadius*m_iHeight/2);
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

	ArcBall::ArcBall()
	{
		Reset();
		m_vDown=Vector3(0,0,0);
		m_vNow=Vector3(0,0,0);
		m_Offset.x=m_Offset.y=0;
	}

	void ArcBall::Reset()
	{
		QuaternionIdentity(&m_qPreRotate);
		QuaternionIdentity(&m_qNow);
		MatrixIdentity(&m_matRotation);
		MatrixIdentity(&m_matTranslation);
		MatrixIdentity(&m_matTranslationDelta);
		m_bDrag=false;
		m_fRadius=1.0f;
		m_fRadiusTranlation=1.0f;
	}

	void ArcBall::Init(HWND hwnd)
	{
		RECT rc;
		GetClientRect(hwnd,&rc);
		SetWindow(rc.right,rc.bottom);	
	}

	void ArcBall::SetWindow(int width,int height,float radius/*=0.9f*/)
	{
		m_iWidth=width;
		m_iHeight=height;
		m_fRadius=radius;
		m_vCenter=Vector2(m_iWidth/2.0f,m_iHeight/2.0f);
	}

	void ArcBall::OnBegin(int x,int y)
	{
		if(x>=m_Offset.x && x<=m_Offset.x+m_iWidth && y>=m_Offset.y && y<=m_Offset.y+m_iHeight)
		{
			m_bDrag=true;
			m_qPreRotate=m_qNow;
			m_vDown=ScreenToVector((float)x,(float)y);
		}
	}

	void ArcBall::OnMove(int x,int y)
	{
		if(m_bDrag)
		{
			m_vNow=ScreenToVector((float)x,(float)y);
			m_qNow=m_qPreRotate*QuaternionFromBallPoints(m_vDown,m_vNow);
		}
	}

	void ArcBall::OnEnd()
	{
		m_bDrag=false;
	}

	bool ArcBall::HandleMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
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
			m_pointLastMouse.x=mouse_x;
			m_pointLastMouse.y=mouse_y;
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
				float delta_x=(m_pointLastMouse.x-mouse_x)*m_fRadiusTranlation/m_iWidth;
				float delta_y=(m_pointLastMouse.y-mouse_y)*m_fRadiusTranlation/m_iHeight;
				if(wParam & MK_RBUTTON)
				{
					MatrixTranslation(&m_matTranslationDelta,-2*delta_x,2*delta_y,0.0f);
					MatrixMultiply(&m_matTranslation,&m_matTranslation,&m_matTranslationDelta);
				}
				else
				{
					MatrixTranslation(&m_matTranslationDelta,0.0f,0.0f,5*delta_y);
					MatrixMultiply(&m_matTranslation,&m_matTranslation,&m_matTranslationDelta);
				}
			}
			return true;
		}
		return false;
	}

}