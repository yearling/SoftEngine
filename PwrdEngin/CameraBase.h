#ifndef __CAMERABASE_H__
#define __CAMERABASE_H__
#pragma once
#include "stdafx.h"
#include "Gamemath.h"
namespace SoftEngine
{
	class AArcBall
	{
	public:
		AArcBall();
		void Reset();
		void Init(HWND hwnd);
		inline void SetWindow(int width,int height,float radius=0.9f);
		inline void SetTranslationRadius(float radius_translation)
		{
			radius_tranlation_=radius_translation;
		}
		inline void SetOffset(int x,int y){offset_.x=x;offset_.y=y;}

		void OnBegin(int x,int y);
		void OnMove(int x,int y);
		void OnEnd();
		bool HandleMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
		const Matrix* GetRotationMatrix(){return MatrixRotationQuaternion(&rotation_,&now_qua_);}
		const Matrix* GetTranslationMatrix() const { return &translation_;}
		const Matrix* GetTranslationDeltaMatrix() const { return &translation_delta_;}
		bool IsBeingDragged() const { return drag_;}
		Quaternion GetQuaternionNow() const { return now_qua_;}
		void SetQuaternionNow(const Quaternion & q){now_qua_=q;}
		static Quaternion QuaternionFromBallPoints(const Vector3 &from,const Vector3 & to);
	protected:
		Matrix rotation_;// orientation
		Matrix translation_;//position
		Matrix translation_delta_;//position
		POINT offset_;//window offset, upper_left corner window
		int width_;// window width
		int height_;//window height
		Vector2 center_;// center of the arc ball
		float radius_;//radius in screen coords
		float radius_tranlation_;//arc ball's radius for translating the target
		Quaternion pre_rotate_;//quaternion before button down
		Quaternion now_qua_;//quaternion for current drag
		bool drag_;//whether user is dragging arc ball 
		POINT last_mouse_;
		Vector3 down_vec_;
		Vector3 now_vec_;
		Vector3 ScreenToVector(float streen_x,float screen_y);
	};
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
