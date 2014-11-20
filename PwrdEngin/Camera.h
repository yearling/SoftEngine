#ifndef __CAMERABASE_H__
#define __CAMERABASE_H__
#pragma once
#include "stdafx.h"
#include "Gamemath.h"
namespace SoftEngine
{
	class ArcBall
	{
	public:
		ArcBall();
		void Reset();
		void Init(HWND hwnd);
		inline void SetWindow(int width,int height,float radius=0.9f);
		inline void SetTranslationRadius(float radius_translation)
		{
			m_fRadiusTranlation=radius_translation;
		}
		inline void SetOffset(int x,int y){m_Offset.x=x;m_Offset.y=y;}

		void OnBegin(int x,int y);
		void OnMove(int x,int y);
		void OnEnd();
		bool HandleMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
		const Matrix* GetRotationMatrix(){return MatrixRotationQuaternion(&m_matRotation,&m_qNow);}
		const Matrix* GetTranslationMatrix() const { return &m_matTranslation;}
		const Matrix* GetTranslationDeltaMatrix() const { return &m_matTranslationDelta;}
		bool IsBeingDragged() const { return m_bDrag;}
		Quaternion GetQuaternionNow() const { return m_qNow;}
		void SetQuaternionNow(const Quaternion & q){m_qNow=q;}
		static Quaternion QuaternionFromBallPoints(const Vector3 &from,const Vector3 & to);
	protected:
		Matrix m_matRotation;// orientation
		Matrix m_matTranslation;//position
		Matrix m_matTranslationDelta;//position
		POINT m_Offset;//window offset, upper_left corner window
		int m_iWidth;// window width
		int m_iHeight;//window height
		Vector2 m_vCenter;// center of the arc ball
		float m_fRadius;//radius in screen coords
		float m_fRadiusTranlation;//arc ball's radius for translating the target
		Quaternion m_qPreRotate;//quaternion before button down
		Quaternion m_qNow;//quaternion for current drag
		bool m_bDrag;//whether user is dragging arc ball 
		POINT m_pointLastMouse;
		Vector3 m_vDown;
		Vector3 m_vNow;
		Vector3 ScreenToVector(float streen_x,float screen_y);
	};
	enum KEY_STAT
	{
		KEY_MOVE_FORWARD=0,
		KEY_MOVE_BACKWARD,
		KEY_MOVE_LEFT,
		KEY_MOVE_RIGHT,
		KEY_MOVE_UP,
		KEY_MOVE_DOWN,
		KEY_MOVE_MAX
	};
	class CameraBase
	{
	public:
		CameraBase(void);
		virtual ~CameraBase(void);
		virtual bool HandleMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
		virtual void FrameMove(float elapse_time)=0;
		virtual void SetViewParam(Vector3* eye,Vector3 *lookat);
		virtual void SetProjParam(float FOV,float aspect,float near_plane,float far_plane);
		virtual void SetDragRect(RECT &rc) {m_rcDragRect=rc;}
		void SetHWND(HWND hwnd){ m_hwnd=hwnd;}
		void SetWindow(int width,int height){m_iWidth=width;m_iHeight=height;}
		void SetScalers(float rotation_scaler=0.01f,float move_scaler=5.0f){ m_fRotationScalar=rotation_scaler;m_fMoveScalar=move_scaler;}
		const Matrix * GetViewMatrix() const { return & m_matView;}
		const Matrix * GetProjMatrix() const { return & m_matProject;}
		const Vector3 * GetEyePt() const { return & m_vEye;}
		const Vector3 * GetLookatPt() const { return & m_vLookAt;}
		float GetNearClip() const { return m_fNearPlane;}
		float GetFarClip() const { return m_fFarPlane;}
	protected:
		bool IsKeyDown(BYTE key) const 
		{
			return m_bKeyMasks[key];
		}
		void GetInput(bool reset_cursor_after_move);
		Matrix m_matView;
		Matrix m_matProject;
		int m_iKeysDown;
		bool m_bKeyMasks[KEY_MOVE_MAX];//存放各CAM的状态，主要是key控制的状态
		Vector3 m_vKeyboradDirection;
		POINT m_ptLastMousePosition;
		int m_iMouseWhellDelta;
		Vector2 m_vMouseDelta;
		Vector3 m_vDefaultEye;
		Vector3 m_vDefaultLookAt;
		Vector3 m_vEye;
		Vector3 m_vLookAt;
		float m_fCamerYawAngle;
		float m_fCameraPitchAngle;
		RECT m_rcDragRect;
		float m_fFOV;
		float m_fAspect;
		float m_fNearPlane;
		float m_fFarPlane;
		float m_fRotationScalar;
		float m_fMoveScalar;
		bool m_bClipToBoundary;
		Vector3 m_vMinBoundary;
		Vector3 m_vMaxBoundary;
		bool m_bResetCursorAfterMove;
		HWND m_hwnd;
		int m_iWidth;
		int m_iHeight;
	};
	class EASYCamera:public CameraBase
	{
	public:
		EASYCamera();
		~EASYCamera();
		virtual bool HandleMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
		virtual void FrameMove(float elapse_time);
		const Matrix * GetWorldMatrix() const { return & m_matWorld;}
		void SetWindow(int width,int height,float arcball_radius=0.9f)
		{
			m_WorldArcBall.SetWindow(width-1,height-1,arcball_radius);
			m_ViewArcBall.SetWindow(width-1,height-1,arcball_radius);
		}
		void SetRaius(float radius){ m_fRadius=radius;}
	protected:
		Matrix m_matWorld;
		ArcBall m_ViewArcBall;
		ArcBall m_WorldArcBall;
		float m_fRadius;
		Matrix m_matLastWorldRotate;
		Matrix m_matFinalWorldRotate;
	};
}
#endif
