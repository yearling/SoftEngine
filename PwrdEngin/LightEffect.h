#ifndef __LIGHTEFFECT_H__
#define __LIGHTEFFECT_H__
#include "stdafx.h"
#include "device.h"
#include "Parameter.h"
namespace SoftEngine
{
	class BisicalVertexShader:public IVertexShader
	{
	public:
		BisicalVertexShader(Device*p);
		virtual ~BisicalVertexShader();
		virtual void VSMain(RenderVertex &v,VSShaderOutput &out);
		virtual void BeginSetGlobalParam();
	private:
		Matrix m_matWorld;
		Matrix m_matView;
		Matrix m_matProject;
	};
	class GroundPixelShader:public IPixelShader
	{
	public:
		GroundPixelShader(Device*p);
		virtual ~GroundPixelShader();
		virtual int PSMain(VSShaderOutput &);
		virtual void BeginSetGlobalParam();
	};
}
#endif