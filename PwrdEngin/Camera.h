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
		//virtual void Reset();
		virtual void SetViewParam(Vector3* eye,Vector3 *lookat);
		virtual void SetProjParam(float FOV,float aspect,float near_plane,float far_plane);
		virtual void SetDragRect(RECT &rc) {drag_rc_=rc;}
		void SetHWND(HWND hwnd){ hwnd_=hwnd;}
		void SetWindow(int width,int height){width_=width;height_=height;}
		void SetScalers(float rotation_scaler=0.01f,float move_scaler=5.0f){ rotation_scaler_=rotation_scaler;move_scaler_=move_scaler;}
		const Matrix * GetViewMatrix() const { return & view_;}
		const Matrix * GetProjMatrix() const { return & pro_;}
		const Vector3 * GetEyePt() const { return & eye_;}
		const Vector3 * GetLookatPt() const { return & lookat_;}
		float GetNearClip() const { return near_plane_;}
		float GetFarClip() const { return far_plane_;}
	protected:
		bool IsKeyDown(BYTE key) const 
		{
			return key_mask_[key];
		}
		void GetInput(bool reset_cursor_after_move);
		Matrix view_;
		Matrix pro_;
		int keys_down_;
		bool key_mask_[KEY_MOVE_MAX];//存放各CAM的状态，主要是key控制的状态
		Vector3 keyboard_direction_;
		POINT last_mouse_position_;
		int mouse_wheel_delta_;
		Vector2 mouse_delta_;
		Vector3 default_eye_;
		Vector3 default_lookat_;
		Vector3 eye_;
		Vector3 lookat_;
		float camera_yaw_angle_;
		float camera_pitch_angle_;
		RECT drag_rc_;
		Vector3 velocity_;
		float FOV_;
		float aspect_;
		float near_plane_;
		float far_plane_;
		float rotation_scaler_;
		float move_scaler_;
		bool clip_to_boundary_;
		Vector3 min_boundary_;
		Vector3 max_boundary_;
		bool reset_cursor_after_move_;
		HWND hwnd_;
		int width_;
		int height_;
	};
	class EASYCamera:public CameraBase
	{
	public:
		EASYCamera();
		~EASYCamera();
		virtual bool HandleMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
		virtual void FrameMove(float elapse_time);
		const Matrix * GetWorldMatrix() const { return & world_;}
		void SetWindow(int width,int height,float arcball_radius=0.9f)
		{
			world_arcball_.SetWindow(width,height,arcball_radius);
			view_arcball_.SetWindow(width,height,arcball_radius);
		}
		void SetRaius(float radius){ radius_=radius;}
	protected:
		Matrix world_;
		ArcBall view_arcball_;
		ArcBall world_arcball_;
		float radius_;
		Matrix last_world_rotate;
		Matrix final_world_rotate;
	};
}
#endif
