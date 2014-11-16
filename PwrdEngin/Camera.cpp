#include "StdAfx.h"
#include "Camera.h"
#include "Gamemath.h"
namespace SoftEngine
{
	CameraBase::CameraBase(void)
	{
		m_iKeysDown=0;
		memset(m_bKeyMasks,0,sizeof(byte)*KEY_MOVE_MAX);
		Vector3 eye=Vector3(0.0f,0.0f,0.0f);
		Vector3 lookat=Vector3(0.0f,0.0f,0.0f);
		SetViewParam(&eye,&lookat);
		SetProjParam(PI/4,1.0f,1.0f,1000.f);
		GetCursorPos(&m_ptLastMousePosition);
		m_iMouseWhellDelta=0;
		m_fCamerYawAngle=0.0f;
		m_fCameraPitchAngle=0.0f;
		SetRect(&m_rcDragRect,LONG_MIN,LONG_MIN,LONG_MAX,LONG_MAX);
		m_fRotationScalar=0.01f;
		m_fMoveScalar=5.0f;
		m_vMouseDelta=Vector2(0,0);
		m_bClipToBoundary=false;
		m_vMinBoundary=Vector3(-1,-1,-1);
		m_vMaxBoundary=Vector3(1,1,1);
		m_bResetCursorAfterMove=false;
		m_hwnd=NULL;
	}


	CameraBase::~CameraBase(void)
	{
	}

	void CameraBase::SetViewParam(Vector3* eye,Vector3 *lookat)
	{
		if(NULL==eye || NULL==lookat)
			return;
		m_vDefaultEye=m_vEye=*eye;
		m_vDefaultLookAt=m_vLookAt= *lookat;
		Vector3 up(0,1,0);
		MatrixLookAtLH(&m_matView,&m_vEye,&m_vLookAt,&up);
		//先pitch 再yaw
		//得到的view其实是视坐标基的逆，所以要先进行一次逆得到视坐标的基
		//然后得到Z轴坐标，进行计算。
		//因为是先pitch再yaw，也就是说，先绕x轴，再绕y轴把z轴从世界坐标绕到现在的视坐标
		//旋转保持轴长度不变和向量到旋转轴的距离不变。
		//1.绕X轴把Z轴转到YZ平面
		//2.绕Y轴把Z轴转到位置
		//如果要计算的话要反过来算,先算2
		//就是Z轴现在的位置绕Y轴转到YZ平面上，投影到XZ平面上，角度为-arctan(x/z),反过来也就是2，就是arctan(x/z)
		//然后因为刚才绕Y轴旋转，投影到XZ平面上的sqrt(x^2+z^2)长度不变，就是现在Y轴投到Z轴上的长度，
		//所以绕X轴的旋角为arctan(y,sqrt(x^2+z^2)),角度从Z到Y，反方向，取负
		Matrix inverse_view;
		MatrixInverse(&inverse_view,NULL,&m_matView);
		Vector3 * z_basis=(Vector3*)&inverse_view._31;
		m_fCamerYawAngle=atan2f(z_basis->x,z_basis->z);
		float len=sqrtf(z_basis->z*z_basis->z+z_basis->x*z_basis->x);
		m_fCameraPitchAngle=-atan2f(z_basis->y,len);
	}

	bool CameraBase::HandleMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
	{
		switch(uMsg)
		{
		case WM_KEYDOWN:
			{
				switch(wParam)
				{
				case 'W':
					m_bKeyMasks[KEY_MOVE_FORWARD]=true;
					break;
				case 'S':
					m_bKeyMasks[KEY_MOVE_BACKWARD]=true;
					break;
				case 'A':
					m_bKeyMasks[KEY_MOVE_LEFT]=true;
					break;
				case 'D':
					m_bKeyMasks[KEY_MOVE_RIGHT]=true;
					break;
				}

				break;
			}
		case WM_KEYUP:
			{
				switch(wParam)
				{
				case 'W':
					m_bKeyMasks[KEY_MOVE_FORWARD]=false;
					break;
				case 'S':
					m_bKeyMasks[KEY_MOVE_BACKWARD]=false;
					break;
				case 'A':
					m_bKeyMasks[KEY_MOVE_LEFT]=false;
					break;
				case 'D':
					m_bKeyMasks[KEY_MOVE_RIGHT]=false;
					break;
				}
				break;
			}
		case WM_MOUSEWHEEL:
			m_iMouseWhellDelta+=(short)HIWORD(wParam)*0.2;
			break;
		}
		return false;
	}

	void CameraBase::SetProjParam(float FOV,float aspect,float near_plane,float far_plane)
	{
		m_fFOV=FOV;
		m_fAspect=aspect;
		m_fNearPlane=near_plane;
		m_fFarPlane=far_plane;
		MatrixPerspectiveFOVLH(&m_matProject,m_fFOV,m_fAspect,m_fNearPlane,m_fFarPlane);
	}

	void CameraBase::GetInput(bool reset_cursor_after_move)
	{
		m_bResetCursorAfterMove=reset_cursor_after_move;
		m_vKeyboradDirection=Vector3(0,0,0);
		if(m_bKeyMasks[KEY_MOVE_FORWARD])
			m_vKeyboradDirection.z+=1.0f;
		if(m_bKeyMasks[KEY_MOVE_BACKWARD])
			m_vKeyboradDirection.z-=1.0f;
		if(m_bKeyMasks[KEY_MOVE_UP])
			m_vKeyboradDirection.y+=1.0f;
		if(m_bKeyMasks[KEY_MOVE_DOWN])
			m_vKeyboradDirection.y-=1.0f;
		if(m_bKeyMasks[KEY_MOVE_RIGHT])
			m_vKeyboradDirection.x+=1.0f;
		if(m_bKeyMasks[KEY_MOVE_LEFT])
			m_vKeyboradDirection.z-=1.0f;
	}


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
		return false;
	}


	EASYCamera::EASYCamera()
	{
		m_fRadius=10.0f;
		MatrixIdentity(&m_matWorld);
		MatrixIdentity(&m_matView);
		MatrixIdentity(&m_matLastWorldRotate);
		MatrixIdentity(&m_matFinalWorldRotate);
	}

	EASYCamera::~EASYCamera()
	{

	}

	bool EASYCamera::HandleMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
	{
		CameraBase::HandleMessage(hwnd,uMsg,wParam,lParam);
		if(uMsg==WM_LBUTTONDOWN )
		{
			int x=(short)LOWORD(lParam);
			int y=(short)HIWORD(lParam);
			m_WorldArcBall.OnBegin(x,y);
		}
		if(uMsg==WM_RBUTTONDOWN )
		{
			int x=(short)LOWORD(lParam);
			int y=(short)HIWORD(lParam);
			m_ViewArcBall.OnBegin(x,y);
		} 
		if(uMsg==WM_MOUSEMOVE)
		{
			int x=(short)LOWORD(lParam);
			int y=(short)HIWORD(lParam);
			m_WorldArcBall.OnMove(x,y);
			m_ViewArcBall.OnMove(x,y);
		}
		if(uMsg==WM_LBUTTONUP)
		{
			int x=(short)LOWORD(lParam);
			int y=(short)HIWORD(lParam);
			m_WorldArcBall.OnEnd();
		}
		if(uMsg==WM_RBUTTONUP)
		{
			int x=(short)LOWORD(lParam);
			int y=(short)HIWORD(lParam);
			m_ViewArcBall.OnEnd();
		}
		if(uMsg==WM_CAPTURECHANGED)
		{
			if(hwnd!=(HWND)lParam)
			{
				m_WorldArcBall.OnEnd();
			}
		}
		return false;
	}

	void EASYCamera::FrameMove(float elapse_time)
	{
		m_fRadius-=m_iMouseWhellDelta*elapse_time;	
		m_iMouseWhellDelta=0;
		Matrix view_arcball;
		MatrixInverse(&view_arcball,nullptr,m_ViewArcBall.GetRotationMatrix());
		Vector3 local_ahead(0.0f,0.0f,1.0f);
		Vector3 local_up(0.0f,1.0f,0.0f);
		Vector3 world_lookat,world_up;
		Vec3TransformCoord(&world_up,&local_up,&view_arcball);
		Vec3TransformCoord(&world_lookat,&local_ahead,&view_arcball);
		m_vEye=m_vLookAt-world_lookat*m_fRadius;
		MatrixLookAtLH(&m_matView,&m_vEye,&m_vLookAt,&world_up);
		Matrix view_inverse;
		Matrix world_rotate_view=*m_WorldArcBall.GetRotationMatrix();
		Matrix last_world_rotate_invert;
		MatrixInverse(&last_world_rotate_invert,nullptr,&m_matLastWorldRotate);
		MatrixInverse(&view_inverse,nullptr,&m_matView);
		m_matFinalWorldRotate*=m_matView*last_world_rotate_invert*world_rotate_view*view_inverse;
		//这个时候也不知道旋转哪去了，who care,最后把那放到指定位置就行了。
		m_matWorld=m_matFinalWorldRotate;
		m_matWorld._41=m_vLookAt.x;
		m_matWorld._42=m_vLookAt.y;
		m_matWorld._43=m_vLookAt.z;
		m_matLastWorldRotate=world_rotate_view;
		//////////////////////////////////////////////////////////////////////////
		static float y=0;
		static float x=0;
		float scalar=0.5f;
		if(m_bKeyMasks[KEY_MOVE_FORWARD]==true)
		{
			y+=elapse_time*m_fRadius*scalar;
		}
		if(m_bKeyMasks[KEY_MOVE_BACKWARD]==true)
		{
			y-=elapse_time*m_fRadius*scalar;
		}
		if(m_bKeyMasks[KEY_MOVE_LEFT]==true)
		{
			x-=elapse_time*m_fRadius*scalar;	
		}
		if(m_bKeyMasks[KEY_MOVE_RIGHT]==true)
		{
			x+=elapse_time*m_fRadius*scalar;
		}
		m_matWorld._42+=y;
		m_matWorld._41+=x;
		m_matWorld._43+=0;
	}

}