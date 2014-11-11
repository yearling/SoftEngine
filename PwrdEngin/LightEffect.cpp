#include "StdAfx.h"
#include "LightEffect.h"
#include "Parameter.h"
#include <vector>
namespace SoftEngine
{

	BisicalVertexShader::~BisicalVertexShader()
	{

	}

	BisicalVertexShader::BisicalVertexShader(Device*p):IVertexShader(p)
	{

	}


	void BisicalVertexShader::VSMain(RenderVertex &v,VSShaderOutput &out)
	{
		out.m_bVisible=v.m_bVisible;
		out.m_vColor=v.m_vColor;
		out.m_vPosition=v.m_vPosition*m_matWorld*m_matView*m_matProject;
		out.m_vColor=v.m_vColor;
		out.m_vTexcoord=v.m_vTexcoord;
		Vector4 transNormal=Vector4(v.m_vNormal.x,v.m_vNormal.y,v.m_vNormal.z,0.0f);
		transNormal*=m_matWorld;
		out.m_vNormal=Vector3(transNormal.x,transNormal.y,transNormal.z).Normalize();
	}

	void BisicalVertexShader::BeginSetGlobalParam()
	{
		SeftData *pdata=(SeftData*)GetGobalData();
		m_matWorld=pdata->world;
		m_matView=pdata->view;
		m_matProject=pdata->project;
	}


	GroundPixelShader::GroundPixelShader(Device*p):IPixelShader(p)
	{

	}

	GroundPixelShader::~GroundPixelShader()
	{

	}

	int GroundPixelShader::PSMain(VSShaderOutput &ps)
	{
		Color texColor=(Color) m_pSamper->GetColor(ps.m_vTexcoord.x,ps.m_vTexcoord.y);
		float fDirect=ps.m_vNormal.Dot(m_vLightDirection);
		texColor=texColor*fDirect*m_fDirectScalar+m_cAmbient*m_fAbmientScalar;
		return texColor.Sature();
	}

	void GroundPixelShader::BeginSetGlobalParam()
	{
		SeftData *pdata=(SeftData*)GetGlobalData();
		m_pSamper=pdata->m_pSamper;	
		m_vLightDirection=pdata->vDirect;
		m_cDirectColor=(Color)pdata->direct;
		m_fDirectScalar=pdata->fDirectScalar;
		m_cAmbient=(Color)pdata->ambient;
		m_fAbmientScalar=pdata->fAmbientScalar;
		m_matRotation=pdata->world;
	}

}