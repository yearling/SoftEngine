#include "StdAfx.h"
#include "LightEffect.h"
#include "Parameter.h"
#include <vector>
namespace SoftEngine
{

	LightEffect::LightEffect(void):IEffect()
	{
		//m_cullmode=CULL_NODE;
		//m_cullmode=CULL_CW;
	}


	LightEffect::~LightEffect(void)
	{
	}

	void LightEffect::Draw(int base_vertex_index,UINT start_index,UINT primitiveCount)
	{
		FillPipline(base_vertex_index,start_index,primitiveCount);
		RenderVertex* v[3];
		for(int i=0;i<m_vecRenderBuffer.size();)
		{
			for(int j=0;j<3;j++,i++)
			{
				v[j]=&m_vecRenderBuffer[i];
			}
			VSandPS(v);
		}
	}

	void LightEffect::SetData(void *p)
	{
		m_pSeftData=(SeftData*)p;
	}

	void LightEffect::FillPipline(int base_vertex_index,UINT start_index,UINT primitiveCount)
	{
		m_vecRenderBuffer.clear();
		m_vecIndexBuffer.clear();
		int position_offset=m_pVertexDecl->GetPositionOffset();
		int strip=m_pVertexDecl->GetSize();
		const byte *vertex_buffer_trans=m_pDesVertexBuffer->GetBuffer()+base_vertex_index*strip;
		const UINT *index_buffer_=m_pDesIndexBuffer->GetBuffer();
		RenderVertex tmp_vertex;
		UINT  tmp_index;
		for(UINT i=0;i<primitiveCount;i++)
		{
			for(int j=0;j<3;j++)
			{
				tmp_index=index_buffer_[start_index++];
				tmp_vertex.m_bVisible=true;
				tmp_vertex.m_vPosition=ToVector3(vertex_buffer_trans,tmp_index,strip,position_offset);
				m_vecRenderBuffer.push_back(tmp_vertex);
				m_vecIndexBuffer.push_back(tmp_index);
			}
			
		}
	}

	void LightEffect::VSandPS(RenderVertex &v0,RenderVertex &v1,RenderVertex &v2)
	{
		
		
	}

	void LightEffect::VSandPS(RenderVertex* v[3])
	{
		SeftData *sf=GetData();
		Matrix world=sf->world;
		Matrix view=sf->view;
		Matrix project=sf->project;
		Matrix viewport=sf->viewPort;
		Matrix change=world*view*project*viewport;
		for(int i=0;i<3;i++)
			{
				v[i]->m_vPosition*=change;
				v[i]->m_vPosition.ProjectDivied();
		}
		//////////////////////////////////////////////////////////////////////////
		//±³ÃæÏûÒþ
		if(m_cullmode!=CULL_NODE)
		{
			Vector3 v01(v[1]->m_vPosition.x-v[0]->m_vPosition.x,
				v[1]->m_vPosition.y-v[0]->m_vPosition.y,0.0f);
			Vector3 v12(v[2]->m_vPosition.x-v[1]->m_vPosition.x,
				v[2]->m_vPosition.y-v[1]->m_vPosition.y,0.0f);
			Vector3 vCull=v01^v12;
			if((m_cullmode==CULL_CCW && vCull.z<0 )||(m_cullmode==CULL_CW &&vCull.z>0))
			{
				return;	
			}
		}
		//////////////////////////////////////////////////////////////////////////
		m_pDevice->DrawLine(v[0]->m_vPosition.x,v[0]->m_vPosition.y,
							v[1]->m_vPosition.x,v[1]->m_vPosition.y);
		m_pDevice->DrawLine(v[1]->m_vPosition.x,v[1]->m_vPosition.y,
							v[2]->m_vPosition.x,v[2]->m_vPosition.y);
		m_pDevice->DrawLine(v[2]->m_vPosition.x,v[2]->m_vPosition.y,
							v[0]->m_vPosition.x,v[0]->m_vPosition.y);
	}

}