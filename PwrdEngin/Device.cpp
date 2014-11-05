#include "StdAfx.h"
#include "Device.h"
#include "Gamemath.h"
#include <assert.h>
#include <math.h>
#include <iostream>
#include <algorithm>
using namespace std;
namespace SoftEngine
{
	Device::Device():m_pDrawImpl(nullptr),
					m_pBackBuffer(nullptr),
					m_iPitch(0),m_iWidth(0),m_iHeight(0),
					m_pVertexDecl(nullptr),
					m_pDesIndexBuffer(nullptr),
					m_pDesVertexBuffer(nullptr),
					m_pGameSource(nullptr),
					m_cullmode(CULL_CCW),
					m_fillmode(FILL_SOLID),
					m_pPs(nullptr),
					m_pVs(nullptr)
	{
		memset(&m_rcClip,0,sizeof(m_rcClip));
		MatrixIdentity(&m_matWorld);
		MatrixIdentity(&m_matView);
		MatrixIdentity(&m_matProject);
		MatrixIdentity(&m_matViewPort);
	}

	

	bool Device::Init(DrawImpl* draw_imp)
	{
		m_pDrawImpl=draw_imp;
		return true;
	}

	bool Device::Init(const Window *windows)
	{
		if(windows==nullptr)
			return false;
		else
		{
			m_pDrawImpl=new DrawImpl();
			if(!m_pDrawImpl->Init(windows->m_hWnd,windows->m_iWidth,windows->m_iHeight,windows->m_iClientOffsetX,windows->m_iClientOffsetY,windows->m_bWindow))
				return false;
			m_iWidth=windows->m_iWidth;
			m_iHeight=windows->m_iHeight;
		}
		return true;
	}

	bool Device::BeginScene()
	{
		assert(m_pDrawImpl&&"render should init");
		if(m_pBackBuffer=m_pDrawImpl->LockBackSurface(&m_iPitch,&m_iWidth,&m_iHeight))
		{
			m_rcClip.left=0;
			m_rcClip.top=0;
			m_rcClip.right=m_iWidth;
			m_rcClip.bottom=m_iHeight;
			return true;
		}
		
		return false;
	}

	bool Device::EndScene()
	{
		m_pDrawImpl->UnlockBackSurface();
		m_iPitch=0;
		m_iWidth=0;
		m_iHeight=0;
		m_pBackBuffer=nullptr;
		memset(&m_rcClip,0,sizeof(m_rcClip));
		return true;
	}

	void Device::DrawLine(const VSShaderOutput &out0,const VSShaderOutput &out1)
	{
		int cx0,cy0,cx1,cy1,x0,y0,x1,y1;
		cx0=x0=out0.m_vScreenPosition.x;
		cy0= y0=out0.m_vScreenPosition.y;
		cx1= x1=out1.m_vScreenPosition.x;
		cy1= y1=out1.m_vScreenPosition.y;
		//都剪裁掉了
		if(!Draw2DClipe(m_rcClip,cx0,cy0,cx1,cy1))
			return;
		int color;
		float lerp;
		VSShaderOutput tmp;
		//退化为一个点
		if(cx0==cx1 && cy0==cy1)
		{
			lerp=float(cx0-x0)/float(x1-x0);
			tmp=Lerp(out0,out1,lerp);	
			color=m_pPs->PSMain(tmp);
			DrawPixel(cx0,cy0,color);
			return;
		}

		int dx,dy;
		dx=cx1-cx0;
		dy=cy1-cy0;
		int step;
		float lerpstep;
		if(abs(dx)>abs(dy))
		{
			step=abs(dx);
			lerp=(float)(cx0-x0)/(float)(x1-x0);
			//lerpstep=1.0f/(float)(x1-x0);
			lerpstep=(float)(cx1-cx0)/(float)(x1-x0)/step;
		}
		else
		{
			step=abs(dy);
			lerp=(float)(cy0-y0)/(float)(y1-y0);
			//lerpstep=1.0f/(float)(y1-y0);
			lerpstep=(float)(cy1-cy0)/(float)(y1-y0)/step;
		}
		float x_add=static_cast<float>(dx)/static_cast<float>(step);
		float y_add=static_cast<float>(dy)/static_cast<float>(step);
		float x=cx0;
		float y=cy0;
		for(int i=0;i<step;i++)
		{
			tmp=Lerp(out0,out1,lerp);	
			color=m_pPs->PSMain(tmp);
			DrawPixel(Round(x),Round(y),color);
			x+=x_add;
			y+=y_add;
			lerp+=lerpstep;
		}
	}

	inline void linerLerp(const int & x0,const int &y0,const int& x1,const int &y1,float lerp,int &des_x,int &des_y)
	{
			des_x=x0+(x1-x0)*lerp;
			des_y=y0+(y1-y0)*lerp;
	}
	bool Device::Draw2DClipe(const RECT &rc,int & x0,int &y0,int &x1,int &y1)
	{
		if(x0<rc.left && x1<rc.left)
			return false;
		else if(x0<rc.left)
		{
			y0=y0+(float)(y1-y0)*(float)(rc.left-x0)/float(x1-x0);
			x0=rc.left;
		}
		else if(x1<rc.left)
		{
			y1=y1+float(y0-y1)*float(rc.left-x1)/float(x0-x1);
			x1=rc.left;
		}
		if(y0<rc.top && y1<rc.top)
			return false;
		else if(y0<rc.top)
		{
			x0=x0+float(x1-x0)*float(rc.top-y0)/float(y1-y0);
			y0=rc.top;
		}
		else if(y1<rc.top)
		{
			x1=x1+float(x0-x1)*float(rc.top-y1)/float(y0-y1);
			y1=rc.top;
		}

		if(x0 >rc.right && x1>rc.right)
			return 0;
		else if(x0> rc.right)
		{
			y0=y0+float(y1-y0)*float(x0-rc.right)/float(x0-x1);
			x0=rc.right;
		}
		else if(x1>rc.right)
		{
			y1=y1+float(y0-y1)*float(x1-rc.right)/float(x1-x0);
			x1=rc.right;
		}

		if(y0>rc.bottom && y1>rc.bottom)
			return false;
		else if(y0>rc.bottom)
		{
			x0=x0+float(x1-x0)*float(y0-rc.bottom)/float(y0-y1);
			y0=rc.bottom;
		}
		else if(y1>rc.bottom)
		{
			x1=x1+float(x0-x1)*float(y1-rc.bottom)/float(y1-y0);
			y1=rc.bottom;
		}
		return true;
	}

	bool Device::Clear(UINT color)
	{
		m_pDrawImpl->ClearBackBuffer(color);
		return true;
	}

	bool Device::Present()
	{
		m_vecRenderBuffer.clear();
		m_pDrawImpl->Flip();
		return true;
	}

	void Device::SetViewPort(int x/*=0*/,int y/*=0*/,int width/*=0*/,int height/*=0*/,float minZ/*=0.0f*/,float maxZ/*=1.0f*/)
	{
		if(width==0)
			width=m_iWidth;
		if(height==0)
			height=m_iHeight;
			m_matViewPort.m[0][0]=width/2;m_matViewPort.m[0][1]=0.0f;m_matViewPort.m[0][2]=0.0f;m_matViewPort.m[0][3]=0.0f;
			m_matViewPort.m[1][0]=0.0f;m_matViewPort.m[1][1]=-height/2;m_matViewPort.m[1][2]=0.0f;m_matViewPort.m[1][3]=0.0f;
			m_matViewPort.m[2][0]=0.0f;m_matViewPort.m[2][1]=0.0f;m_matViewPort.m[2][2]=maxZ-minZ;m_matViewPort.m[2][3]=0.0f;
			m_matViewPort.m[3][0]=x+width/2;m_matViewPort.m[3][1]=y+height/2;m_matViewPort.m[3][2]=minZ;m_matViewPort.m[3][3]=1.0f;
	}

	VertexDeclaration* Device::CreateVertexDeclaration(VERTEXELEMENT v[])
	{
		VertexDeclaration *p=new VertexDeclaration();
		if(p->SetVertexDeclaration(v))
			return p;
		else
			{
				delete p;
				return nullptr;
		}
	}

	bool Device::SetVertexDeclaration(VertexDeclaration *p)
	{
		if(p==nullptr)
			return false;
		else
			{
				m_pVertexDecl=p;
				return true;
		}
	}

	IndexBuffer* Device::CreateIndexBuffer(UINT length)
	{
		IndexBuffer *p=new IndexBuffer();
		if(p)
		{
			if(p->CreateIndexBuffer(length))
				return p;
		}
		return nullptr;
	}

	VertexBuffer* Device::CreateVertexBuffer(UINT length)
	{
		VertexBuffer *p=new VertexBuffer();
		if(p)
		{
			if(p->CreateBuffer(length))
				return p;
		}
		return nullptr;
	}



	void Device::SetStreamSource(VertexBuffer *p)
	{
		m_pDesVertexBuffer=p;
	}

	void Device::SetIndices(IndexBuffer*p)
	{
		m_pDesIndexBuffer=p;
	}

	bool Device::DrawIndexedPrimitive(PRIMITIVETYPE type,int base_vertex_index,UINT min_index, UINT num_vertics,UINT start_index,UINT primitiveCount)
	{
		switch(type)
		{
		case PT_TRIANGLEIST:
			return DrawIndexedTrianglelist( base_vertex_index, min_index, num_vertics, start_index, primitiveCount);
		default:
			break;
		}
		return true;
	}

	bool Device::DrawIndexedTrianglelist(int base_vertex_index,UINT min_index, UINT num_vertics,UINT start_index,UINT primitiveCount)
	{
		assert(m_pDesVertexBuffer && m_pDesIndexBuffer &&m_pVertexDecl);
		FillPipline(base_vertex_index,num_vertics,start_index,primitiveCount);
		VSShaderOutput tmp;
		std::for_each(m_vecRenderBuffer.begin(),m_vecRenderBuffer.end(),[&](RenderVertex & v)
		{
			m_pVs->VSMain(v,tmp);
			tmp.m_vScreenPosition=tmp.m_vPosition*m_matViewPort;
			tmp.m_vScreenPosition.ProjectDivied();
			m_vecVSOutput.push_back(tmp);
		});

		//////////////////////////////////////////////////////////////////////////
		if(m_cullmode!=CULL_NONE)
		for(UINT i=0;i<m_vecIndexBuffer.size();)
		{
			VSShaderOutput &out0=m_vecVSOutput[m_vecIndexBuffer[i++]];
			VSShaderOutput &out1=m_vecVSOutput[m_vecIndexBuffer[i++]];
			VSShaderOutput &out2=m_vecVSOutput[m_vecIndexBuffer[i++]];
			Vector4 v0=out0.m_vScreenPosition;
			Vector4 v1=out1.m_vScreenPosition;
			Vector4 v2=out2.m_vScreenPosition;
			Vector3 v01=Vector3(v1.x-v0.x,v1.y-v0.y,0.0f);
			Vector3 v12=Vector3(v2.x-v1.x,v2.y-v1.y,0.0f);
			Vector3 vCull=v01^v12;
			if((m_cullmode==CULL_CCW && vCull.z<0 )||(m_cullmode==CULL_CW &&vCull.z>0))
			{
				out0.m_bVisible=false;
				out1.m_bVisible=false;
				out2.m_bVisible=false;
			}
		}
		switch(m_fillmode)
		{
		case FILL_POINT:
			break;
		case FILL_WIREFRAME:
			FillWireFrame();
			break;
		case FILL_SOLID:
			FillSolid();
			break;
		default:
			break;
		}
		return true;
	}
	void Device::FillPipline(int base_vertex_index,UINT num_vertics,UINT start_index,UINT primitiveCount)
	{
		m_vecRenderBuffer.clear();
		m_vecIndexBuffer.clear();
		m_vecVSOutput.clear();
		int position_offset=m_pVertexDecl->GetPositionOffset();
		assert(position_offset==0 && "position offset should be 0");
		int iColorOffset=m_pVertexDecl->GetColorOffset();
		assert(iColorOffset==24 && "color offset should be 24");
		int strip=m_pVertexDecl->GetSize();
		const byte *vertex_buffer_trans=m_pDesVertexBuffer->GetBuffer()+base_vertex_index*strip;
		const UINT *index_buffer_=m_pDesIndexBuffer->GetBuffer();
		RenderVertex tmp_vertex;
		UINT  tmp_index;
		for(UINT i=0;i<num_vertics;i++)
		{
			tmp_vertex.m_bVisible=true;
			tmp_vertex.m_vPosition=ToVector3(vertex_buffer_trans,i,strip,position_offset);
			tmp_vertex.m_vColor=ToVector4(vertex_buffer_trans,i,strip,iColorOffset);
			m_vecRenderBuffer.push_back(tmp_vertex);
		}
		for(UINT i=0;i<primitiveCount*3;i++)
		{
				tmp_index=index_buffer_[start_index++];
				m_vecIndexBuffer.push_back(tmp_index);
		}
	}
	//画线框
	void Device::FillWireFrame()
	{
		for(UINT i=0;i<m_vecIndexBuffer.size();)
		{
			int index0=m_vecIndexBuffer[i++];
			int index1=m_vecIndexBuffer[i++];
			int index2=m_vecIndexBuffer[i++];
			if(m_vecVSOutput[index0].m_bVisible==true ||
				m_vecVSOutput[index1].m_bVisible==true||
				m_vecVSOutput[index2].m_bVisible==true)
			{
				DrawLine(m_vecVSOutput[index0],m_vecVSOutput[index1]);
				DrawLine(m_vecVSOutput[index1],m_vecVSOutput[index2]);
				DrawLine(m_vecVSOutput[index2],m_vecVSOutput[index0]);
			}
		}
	}

	void Device::FillSolid()
	{
		for(UINT i=0;i<m_vecIndexBuffer.size();)
		{
			VSShaderOutput &v0=m_vecVSOutput[m_vecIndexBuffer[i++]];
			VSShaderOutput &v1=m_vecVSOutput[m_vecIndexBuffer[i++]];
			VSShaderOutput &v2=m_vecVSOutput[m_vecIndexBuffer[i++]];
			if(v0.m_bVisible==false && v1.m_bVisible==false &&v2.m_bVisible==false)
				return;


		}
	}
	bool Device::TextDraw(std::string text, int x,int y,DWORD color)
	{
		m_pDrawImpl->DrawTextGDI(text,x,y,color);
		return(1);
	}

	

	void Device::SetGameSource(void *pSrc)
	{
		m_pGameSource=pSrc;
	}

	Matrix* Device::GetViewPort()
	{
		return &m_matViewPort;
	}



	void * Device::GetGameSource()
	{
		return m_pGameSource;
	}

	void Device::VSSetData()
	{
		if(m_pVs)
			m_pVs->BeginSetGlobalParam();
	}

	void Device::PSSetData()
	{
		if(m_pPs)
			m_pPs->BeginSetGlobalParam();
	}




	VertexDeclaration::VertexDeclaration():m_iPositionOffsetCached(0),
		m_iColorOffsetCached(0),m_iNormalOffsetCached(0),m_iTexcoordOffsetCached(0),
		m_iSize(0)
	{
	}

	bool VertexDeclaration::SetVertexDeclaration(const VERTEXELEMENT *v)
	{
		if(v==nullptr)
			return false;
		m_vecVertex.clear();
		m_iPositionOffsetCached=0;
		m_iColorOffsetCached=0;
		m_iNormalOffsetCached=0;
		m_iTexcoordOffsetCached=0;
		m_iSize=0;
		if(v)
		{
			while(v->m_byteUsage!=DECLUSAGE_END)
			{
				switch(v->m_byteType)
				{
				case DECLTYPE_FLOAT1:
						m_iSize+=4;
					break;
				case DECLTYPE_FLOAT2:
					m_iSize+=8;
					break;
				case DECLTYPE_FLOAT3:
					m_iSize+=12;
					break;
				case DECLTYPE_FLOAT4:
					m_iSize+=16;
					break;
				case DECLTYPE_UINT:
					m_iSize+=4;
					break;
				default:
					break;
				}
				m_vecVertex.push_back(*v);
				switch(v->m_byteUsage)
				{
				case DECLUSAGE_POSITION:
					 if(v->m_byteType==DECLTYPE_FLOAT3&&v->m_byteUsageIndex==0)		
						m_iPositionOffsetCached=v->m_wOffset;
					 break;
				case DECLUSAGE_NORMAL:
					 if(v->m_byteType==DECLTYPE_FLOAT3&&v->m_byteUsageIndex==0)		
						m_iNormalOffsetCached=v->m_wOffset;
					 break;
				case DECLUSAGE_TEXCOORD:
					 if(v->m_byteType==DECLTYPE_FLOAT3&&v->m_byteUsageIndex==0)		
						 m_iTexcoordOffsetCached=v->m_wOffset;
					 break;
				case DECLUSAGE_COLOR:
					 if(v->m_byteType==DECLTYPE_FLOAT4&&v->m_byteUsageIndex==0)		
						 m_iColorOffsetCached=v->m_wOffset;
						break;
				default:
					break;
				}
				v++;
			}
		}
		return true;
	}


	VertexBuffer::VertexBuffer():m_pBuffer(nullptr),m_bLock(false)
	{

	}

	VertexBuffer::~VertexBuffer()
	{
		if(m_pBuffer)
			delete[] m_pBuffer;
	}

	bool VertexBuffer::CreateBuffer(UINT length)
	{
		if(length<=0)
			return false;
		if(length!=m_uLength)
		{
			if(m_pBuffer)
				delete[] m_pBuffer;
		}
		else
			return true;
		m_pBuffer=new byte[length];
		if(m_pBuffer)
			{
				m_uLength=length;
				return true;
			}
		else 
			return false;
	}

	void * VertexBuffer::Lock(UINT offset_to_lock,UINT size_to_lock)
	{
		if(m_bLock)
			return nullptr;
		if(offset_to_lock>m_uLength-1)
			return nullptr;
		if(offset_to_lock+size_to_lock>m_uLength)
			return nullptr;
		m_bLock=true;
		return  static_cast<void*>(m_pBuffer+offset_to_lock);
	}

	void VertexBuffer::UnLock()
	{
		m_bLock=false;
	}


	IndexBuffer::IndexBuffer():m_pBuffer(nullptr),m_uLength(0),m_bLock(false)
	{

	}

	IndexBuffer::~IndexBuffer()
	{
		if(m_pBuffer)
		{
			delete[] m_pBuffer;
		}
	}

	bool IndexBuffer::CreateIndexBuffer(UINT length)
	{
		if(length<=0)
			return false;
		if(length!=m_uLength)
		{
			if(m_pBuffer)
				delete[] m_pBuffer;
		}
		else
			return true;
		m_pBuffer=new UINT[length];
		if(m_pBuffer)
		{
			m_uLength=length;
			return true;
		}
		else 
			return false;
	}

	UINT* IndexBuffer::Lock(UINT offset_to_lock/*=0*/,UINT size_to_lock/*=0*/)
	{
		if(m_bLock)
			return nullptr;
		if(offset_to_lock>m_uLength-1)
			return nullptr;
		if(offset_to_lock+size_to_lock>m_uLength)
			return nullptr;
		m_bLock=true;
		return  static_cast<UINT*>(m_pBuffer+offset_to_lock);
	}

	void IndexBuffer::UnLock()
	{
		m_bLock=false;
	}


	
	IVertexShader::IVertexShader(Device *p)
	{
		m_pDevice=p;
	}

	void * IVertexShader::GetGobalData()
	{
		return m_pDevice->GetGameSource();
	}

	IVertexShader::~IVertexShader()
	{

	}


	IPixelShader::IPixelShader(Device *p)
	{
		m_pDevice=p;
	}

	void * IPixelShader::GetGlobalData()
	{
		return m_pDevice->GetGameSource();
	}

	IPixelShader::~IPixelShader()
	{

	}

	SoftEngine::VSShaderOutput Lerp(const VSShaderOutput &out0,const VSShaderOutput &out1,float f)
	{
		VSShaderOutput tmp;
		tmp.m_vScreenPosition=Lerp(out0.m_vScreenPosition,out1.m_vScreenPosition,f);
		tmp.m_vNormal=Lerp(out0.m_vNormal,out1.m_vNormal,f);
		tmp.m_vWordPOsition=Lerp(out0.m_vWordPOsition,out1.m_vWordPOsition,f);
		tmp.m_bVisible=out0.m_bVisible && out1.m_bVisible;
		tmp.m_vColor=Lerp(out0.m_vColor,out1.m_vColor,f);
		tmp.m_vTexcoord=Lerp(out0.m_vTexcoord,out1.m_vTexcoord,f);
		tmp.m_vPosition=Lerp(out0.m_vPosition,out1.m_vPosition,f);
		return tmp;
	}

}