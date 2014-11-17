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
		bool hand;
		if(v.m_vTangant.w>0.0f)
			hand=true;
		else
			hand=false;
		Vector3 vTangent(v.m_vTangant.x,v.m_vTangant.y,v.m_vTangant.z);
		Vector3 vBiTangent=v.m_vNormal^vTangent;
		if(hand==false)
			vBiTangent=-vBiTangent;
		Matrix3x3 worldTrans(m_matWorld._11,m_matWorld._12,m_matWorld._13,
			m_matWorld._21,m_matWorld._22,m_matWorld._33,
			m_matWorld._31,m_matWorld._32,m_matWorld._33);
		out.m_vNormal=v.m_vNormal*worldTrans;
		out.m_vTangant=vTangent*worldTrans;
		out.m_vBitangant=vBiTangent*worldTrans;
		//Vector4 transNormal=Vector4(v.m_vNormal.x,v.m_vNormal.y,v.m_vNormal.z,0.0f);
		//std::cout<<v.m_vNormal<<std::endl;
		//transNormal*=m_matWorldInverseTranspose;
		//std::cout<<transNormal<<std::endl;
		//out.m_vNormal=Vector3(transNormal.x,transNormal.y,transNormal.z).Normalize();
		/*Vector4 transTangant=v.m_vTangant;
		transTangant*=m_matWorld;
		if(v.m_vTangant.w>0.0f)
			{
				out.m_vTangant=transTangant;
		}
		else
			out.m_vTangant=-transTangant;*/
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
		Vector3  N=normal;
		Vector3 T=ps.m_vTangant.Normalize();
		Vector3 B=ps.m_vBitangant.Normalize();
		//Vector3 B=N^T;
		Matrix3x3 matT2W(T,B,N);
		Color color=(Color)m_pNormalMapSamper->GetColor(ps.m_vTexcoord.x,ps.m_vTexcoord.y);
		Vector4 trans=Vector4(color*2.0f)-Vector4(1.0f,1.0f,1.0f,1.0f);
		Vector3 normalTrans(trans.x,trans.y,trans.z);
		normalTrans=(normalTrans*matT2W).Normalize();
		normalTrans.Normalize();
		normal.x=normalTrans.x;
		normal.y=normalTrans.y;
		normal.z=normalTrans.z;
		
		//////////////////////////////////////////////////////////////////////////
		Color texColor=(Color) m_pSamper->GetColor(ps.m_vTexcoord.x,ps.m_vTexcoord.y);
		float fDirect=normal.Dot(-m_vLightDirection);
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