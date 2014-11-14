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
		out.m_vNormal=v.m_vNormal;
		Vector4 transNormal=Vector4(v.m_vNormal.x,v.m_vNormal.y,v.m_vNormal.z,0.0f);
		//std::cout<<v.m_vNormal<<std::endl;
		transNormal*=m_matWorldInverseTranspose;
		//std::cout<<transNormal<<std::endl;
		out.m_vNormal=Vector3(transNormal.x,transNormal.y,transNormal.z).Normalize();
		Vector4 transTangant=Vector4(v.m_vTangant.x,v.m_vTangant.y,v.m_vTangant.z,0.0f);
		transTangant*=m_matWorld;
		out.m_vTangant=transTangant;
	}

	void BisicalVertexShader::BeginSetGlobalParam()
	{
		SeftData *pdata=(SeftData*)GetGobalData();
		m_matWorld=pdata->world;
		m_matView=pdata->view;
		m_matProject=pdata->project;
		m_matWorldInverseTranspose=pdata->matWorldInverseTranspose;
	}


	GroundPixelShader::GroundPixelShader(Device*p):IPixelShader(p)
	{

	}

	GroundPixelShader::~GroundPixelShader()
	{

	}

	int GroundPixelShader::PSMain(VSShaderOutput &ps)
	{
		//Normal map
		Vector3 normal=ps.m_vNormal.Normalize();
		/*Vector3  N=normal;
		Vector3 T=(ps.m_vTangant-N*(N*ps.m_vTangant)).Normalize();
		Vector3 B=T^N;
		Matrix matT2W(T.x,T.y,T.z,0.0f,
					   B.x,B.y,B.z,0.0f,
					   N.x,N.y,N.z,0.0f,
					   0.0f,0.0f,0.0f,1.0f);
		Color color=(Color)m_pNormalMapSamper->GetColor(ps.m_vTexcoord.x,ps.m_vTexcoord.y);
		Vector4 trans=Vector4(color*2.0f)-Vector4(1.0f,1.0f,1.0f,1.0f);
		trans=(trans*matT2W).Normalize();
		normal.x=trans.x;
		normal.y=trans.y;
		normal.z=trans.z;*/
		//////////////////////////////////////////////////////////////////////////
		Color texColor=(Color) m_pSamper->GetColor(ps.m_vTexcoord.x,ps.m_vTexcoord.y);
		float fDirect=normal.Dot(m_vLightDirection);
		texColor=texColor*(fDirect*m_fDirectScalar+m_cAmbient*m_fAbmientScalar);
		return texColor.Sature();
	}

	void GroundPixelShader::BeginSetGlobalParam()
	{
		SeftData *pdata=(SeftData*)GetGlobalData();
		m_pSamper=pdata->m_pSamper;	
		m_pNormalMapSamper=pdata->m_pNormalSamper;
		m_vLightDirection=pdata->vDirect;
		m_cDirectColor=(Color)pdata->direct;
		m_fDirectScalar=pdata->fDirectScalar;
		m_cAmbient=(Color)pdata->ambient;
		m_fAbmientScalar=pdata->fAmbientScalar;
		m_matRotation=pdata->world;
	}

}