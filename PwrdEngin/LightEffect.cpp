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
		return m_pSamper->GetColor(ps.m_vTexcoord.x,ps.m_vTexcoord.y);
	}

	void GroundPixelShader::BeginSetGlobalParam()
	{
		SeftData *pdata=(SeftData*)GetGlobalData();
		m_pSamper=pdata->m_pSamper;	
	}

}