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
					m_pVs(nullptr),
					m_pTexture(nullptr)
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
		z_buffer=new float[m_iHeight*m_iWidth];
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
		int x0,y0,x1,y1;
		x0=out0.m_vScreenPosition.x;
		y0=out0.m_vScreenPosition.y;
		x1=out1.m_vScreenPosition.x;
		y1=out1.m_vScreenPosition.y;
		int color;
		float lerp;
		VSShaderOutput tmp;
		//退化为一个点
		if(x0==x1 && y0==y1)
		{
			tmp=out0;
			color=m_pPs->PSMain(tmp);
			DrawPixel(x0,y0,color);
			return;
		}

		int dx,dy;
		dx=x1-x0;
		dy=y1-y0;
		int step;
		float lerpstep;
		if(abs(dx)>abs(dy))
		{
			step=abs(dx);
			lerpstep=1.0f/step;
		}
		else
		{
			step=abs(dy);
			lerpstep=1.0f/step;
		}
		float x_add=static_cast<float>(dx)/static_cast<float>(step);
		float y_add=static_cast<float>(dy)/static_cast<float>(step);
		float x=x0;
		float y=y0;
		lerp=0.0f;
		for(int i=0;i<step;i++)
		{
			tmp=PrespectLerp(out0,out1,lerp);	
			color=m_pPs->PSMain(tmp);
			DrawPixel(m_iWidth,m_iHeight,_RGB(255,255,255));
			DrawPixel(ceil(x),ceil(y),color);
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
			width=m_iWidth-1;
		if(height==0)
			height=m_iHeight-1;
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
		for(int i=0;i<m_iWidth;i++)
			for(int j=0;j<m_iHeight;j++)
				SetZBuffer(i,j,FLT_MAX);
		assert(m_pDesVertexBuffer && m_pDesIndexBuffer &&m_pVertexDecl);
		FillPipline(base_vertex_index,num_vertics,start_index,primitiveCount);
		VSShaderOutput tmp;
		std::for_each(m_vecRenderBuffer.begin(),m_vecRenderBuffer.end(),[&](RenderVertex & v)
		{
			m_pVs->VSMain(v,tmp);
			tmp.m_vScreenPosition=tmp.m_vPosition*m_matViewPort;
			tmp.m_vScreenPosition.ProjectDivied();
			tmp.m_vProjectCutting=tmp.m_vPosition;
			//tmp.m_vProjectCutting.ProjectDivied();
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
		//////////////////////////////////////////////////////////////////////////
		UINT           edges_index[3]; 
		UINT           uPreventCullAgain;
		for(UINT i=0;i<m_vecIndexBuffer.size();)
		{
			for(int j=0;j<3;j++,i++)
				edges_index[j]=m_vecIndexBuffer[i];
			////near plane
			NearCull(edges_index,Plane(0,0,1,1));
		}
		for(auto iter=m_vecVSOutput.begin();iter!=m_vecVSOutput.end();++iter)
			iter->m_vProjectCutting.ProjectDivied();
		//uPreventCullAgain=m_vecIndexBuffer.size();	
		//for(UINT i=0;i<uPreventCullAgain;)
		//{
		//	for(int j=0;j<3;j++,i++)
		//		edges_index[j]=m_vecIndexBuffer[i];
		//	////near plane
		//	FaceCull(edges_index,Plane(0,0,1,0));
		//}
		/*{

			std::cout<<"index size is "<<m_vecIndexBuffer.size()<<std::endl;
			for(int i=0;i<m_vecIndexBuffer.size();i++)
				std::cout<<"i:"<<i<<"  index"<<m_vecIndexBuffer[i]<<":"<<m_vecVSOutput[m_vecIndexBuffer[i]].m_vScreenPosition<<std::endl;
		}*/
		uPreventCullAgain=m_vecIndexBuffer.size();	
		for(UINT i=0;i<uPreventCullAgain;)
		{
			for(int j=0;j<3;j++,i++)
				edges_index[j]=m_vecIndexBuffer[i];
			//left
			FaceCull(edges_index,Plane(1,0,0,1));
		}
		/*{
			std::cout<<"after left"<<std::endl;
			std::cout<<"index size is "<<m_vecIndexBuffer.size()<<std::endl;
			for(int i=0;i<m_vecIndexBuffer.size();i++)
				std::cout<<"i:"<<i<<"  index"<<m_vecIndexBuffer[i]<<":"<<m_vecVSOutput[m_vecIndexBuffer[i]].m_vScreenPosition<<std::endl;
		}*/
		uPreventCullAgain=m_vecIndexBuffer.size();	
		for(UINT i=0;i<uPreventCullAgain;)
		{
			for(int j=0;j<3;j++,i++)
				edges_index[j]=m_vecIndexBuffer[i];
			////right
			FaceCull(edges_index,Plane(-1,0,0,1));
		}
		uPreventCullAgain=m_vecIndexBuffer.size();	
		for(UINT i=0;i<uPreventCullAgain;)
		{
			for(int j=0;j<3;j++,i++)
				edges_index[j]=m_vecIndexBuffer[i];
			////top
			FaceCull(edges_index,Plane(0,-1,0,1));
		}
		/*{
			std::cout<<"after left"<<std::endl;
			std::cout<<"index size is "<<m_vecIndexBuffer.size()<<std::endl;
			for(int i=0;i<m_vecIndexBuffer.size();i++)
				std::cout<<"i:"<<i<<"  index"<<m_vecIndexBuffer[i]<<":"<<m_vecVSOutput[m_vecIndexBuffer[i]].m_vScreenPosition<<std::endl;
		}	*/
		uPreventCullAgain=m_vecIndexBuffer.size();	
		for(UINT i=0;i<uPreventCullAgain;)
		{
			for(int j=0;j<3;j++,i++)
				edges_index[j]=m_vecIndexBuffer[i];
			////bottom
			FaceCull(edges_index,Plane(0,1,0,1));
		}
		uPreventCullAgain=m_vecIndexBuffer.size();	
		for(UINT i=0;i<uPreventCullAgain;)
		{
			for(int j=0;j<3;j++,i++)
				edges_index[j]=m_vecIndexBuffer[i];
			////far 
			FaceCull(edges_index,Plane(0,0,-1,1));
		}
		/*for(int i=0;i<m_vecIndexBuffer.size();i++)
			std::cout<<m_vecRenderBuffer[m_vecIndexBuffer[i]].m_vTexcoord.x<<"		"
			<<m_vecRenderBuffer[m_vecIndexBuffer[i]].m_vTexcoord.x<<std::endl;*/
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
		addCount=0;
		int position_offset=m_pVertexDecl->GetPositionOffset();
		assert(position_offset==0 && "position offset should be 0");
		int iColorOffset=m_pVertexDecl->GetColorOffset();
		assert(iColorOffset==24 && "color offset should be 24");
		int iUVOffset=m_pVertexDecl->GetUVOffset();
		assert(iUVOffset==40 && "uv offset should be 40");
		int strip=m_pVertexDecl->GetSize();
		const byte *vertex_buffer_trans=m_pDesVertexBuffer->GetBuffer()+base_vertex_index*strip;
		const UINT *index_buffer_=m_pDesIndexBuffer->GetBuffer();
		RenderVertex tmp_vertex;
		UINT  tmp_index;
		
		for(UINT i=0;i<primitiveCount*3;i++)
		{
				tmp_index=index_buffer_[start_index++];
				tmp_vertex.m_bVisible=true;
				tmp_vertex.m_vPosition=ToVector3(vertex_buffer_trans,tmp_index,strip,position_offset);
				tmp_vertex.m_vColor=ToVector4(vertex_buffer_trans,tmp_index,strip,iColorOffset);
				tmp_vertex.m_vTexcoord=ToVector2(vertex_buffer_trans,tmp_index,strip,iUVOffset);
				m_vecRenderBuffer.push_back(tmp_vertex);
				m_vecIndexBuffer.push_back(i);
		}
	/*	for(int i=0;i<m_vecIndexBuffer.size();i++)
			std::cout<<m_vecRenderBuffer[m_vecIndexBuffer[i]].m_vTexcoord.x<<"		"
					<<m_vecRenderBuffer[m_vecIndexBuffer[i]].m_vTexcoord.x<<std::endl;*/
	}
	void Device::FaceCull(UINT index[3],const Plane &CullPlane)
	{
		float cull[3];
		UINT inside_index[3];
		UINT outside_index[3];
		int inside_count=0;
		int outside_count=0;
		bool allInvisibal=false;
		for(int i=0;i<3;i++)
		{
			if(m_vecVSOutput[index[i]].m_bVisible==false)
			{
				allInvisibal=true;
				break;
			}
			cull[i]=CullPlane*m_vecVSOutput[index[i]].m_vProjectCutting;
			if(cull[i]>0)
			{
				inside_index[inside_count++]=index[i];
			}
			else
				outside_index[outside_count++]=index[i];
		}
		if(allInvisibal)
			return;
		if(outside_count==3)
		{
			for(int i=0;i<3;i++)
				m_vecVSOutput[index[i]].m_bVisible=false;
		}
		if(inside_count==1)
		{
			OneVertexInView(inside_index[0],outside_index[0],outside_index[1],CullPlane);
		}
		if(inside_count==2)
		{
			TwoVertexInView(inside_index[0],inside_index[1],outside_index[0],CullPlane);
		}
		
	}
	void Device::TwoVertexInView(UINT inIndex0,UINT inIndex1,UINT outIndex,const Plane &cullPlane)
	{
		VSShaderOutput &inVertex0=m_vecVSOutput[inIndex0];
		VSShaderOutput &inVertex1=m_vecVSOutput[inIndex1];
		VSShaderOutput &outVertex=m_vecVSOutput[outIndex];
		outVertex.m_bVisible=false;	
		float lerp0out=-cullPlane*inVertex0.m_vProjectCutting/(cullPlane*(outVertex.m_vProjectCutting-inVertex0.m_vProjectCutting));
		float lerp1out=-cullPlane*inVertex1.m_vProjectCutting/(cullPlane*(outVertex.m_vProjectCutting-inVertex1.m_vProjectCutting));
		VSShaderOutput new0Out=PrespectLerp(inVertex0,outVertex,lerp0out);
		new0Out.m_bVisible=true;
		VSShaderOutput new1Out=PrespectLerp(inVertex1,outVertex,lerp1out);	
		new1Out.m_bVisible=true;
	//	CullInScreen(new0Out.m_vScreenPosition.x,new0Out.m_vScreenPosition.y);
	//	CullInScreen(new1Out.m_vScreenPosition.x,new1Out.m_vScreenPosition.y);
		outVertex=new1Out;
		m_vecVSOutput.push_back(new0Out);
		m_vecIndexBuffer.push_back(m_vecVSOutput.size()-1);
		m_vecVSOutput.push_back(new1Out);
		m_vecIndexBuffer.push_back(m_vecVSOutput.size()-1);
		//m_vecIndexBuffer.push_back(inIndex0);
		m_vecVSOutput.push_back(m_vecVSOutput[inIndex0]);
		m_vecIndexBuffer.push_back(m_vecVSOutput.size()-1);
	}
	void Device::OneVertexInView(UINT inIndex,UINT outIndex0,UINT outIndex1,const Plane &cullPlane)
	{

		VSShaderOutput &inVertex=m_vecVSOutput[inIndex];
		VSShaderOutput &out0Vertex=m_vecVSOutput[outIndex0];
		VSShaderOutput &out1Vertex=m_vecVSOutput[outIndex1];
		float lerpIn0=-cullPlane*inVertex.m_vProjectCutting/(cullPlane*(out0Vertex.m_vProjectCutting-inVertex.m_vProjectCutting));
		float lerpIn1=-cullPlane*inVertex.m_vProjectCutting/(cullPlane*(out1Vertex.m_vProjectCutting-inVertex.m_vProjectCutting));
		VSShaderOutput newIn0=PrespectLerp(inVertex,out0Vertex,lerpIn0);
		VSShaderOutput newIn1=PrespectLerp(inVertex,out1Vertex,lerpIn1);
	//	CullInScreen(newIn0.m_vScreenPosition.x,newIn0.m_vScreenPosition.y);
	//	CullInScreen(newIn1.m_vScreenPosition.x,newIn1.m_vScreenPosition.y);
		newIn0.m_bVisible=true;
		newIn1.m_bVisible=true;
		m_vecVSOutput[outIndex0]=newIn0;
		m_vecVSOutput[outIndex1]=newIn1;
	}

	//画线框
	void Device::FillWireFrame()
	{
		for(UINT i=0;i<m_vecIndexBuffer.size();)
		{
			auto vertex0=m_vecVSOutput[m_vecIndexBuffer[i++]];
			auto vertex1=m_vecVSOutput[m_vecIndexBuffer[i++]];
			auto vertex2=m_vecVSOutput[m_vecIndexBuffer[i++]];
			if(vertex0.m_bVisible==true &&
				vertex1.m_bVisible==true &&
				vertex2.m_bVisible==true)
			{
				DrawLine(vertex0,vertex1);
				DrawLine(vertex1,vertex2);
				DrawLine(vertex2,vertex0);
			}
		}
	}

	void Device::FillSolid()
	{
	/*	for(int i=0;i<m_vecIndexBuffer.size();i++)
			std::cout<<m_vecRenderBuffer[m_vecIndexBuffer[i]].m_vTexcoord.x<<"		"
			<<m_vecRenderBuffer[m_vecIndexBuffer[i]].m_vTexcoord.x<<std::endl;*/
		VSShaderOutput *triangle[3];
		for(UINT i=0;i<m_vecIndexBuffer.size();)
		{
			bool Invisiable=false;
			for(int j=0;j<3;j++)
			{
				triangle[j]=&m_vecVSOutput[m_vecIndexBuffer[i++]];
				if(triangle[j]->m_bVisible==false)
				{
					Invisiable=true;
					break;
				}
			}
			if(Invisiable)
				continue;
			std::sort(triangle,triangle+3,[&](VSShaderOutput *p0,VSShaderOutput *p1){
				return p0->m_vScreenPosition.x<p1->m_vScreenPosition.x;
			});
			std::stable_sort(triangle,triangle+3,[&](VSShaderOutput*p0,VSShaderOutput *p1){
				return p0->m_vScreenPosition.y<p1->m_vScreenPosition.y;
			});
		//std::for_each(triangle,triangle+3,[&](VSShaderOutput*p){std::cout<<p->m_vScreenPosition<<std::endl;});
			if(triangle[0]->m_vScreenPosition.y==triangle[1]->m_vScreenPosition.y &&
				triangle[1]->m_vScreenPosition.y==triangle[2]->m_vScreenPosition.y)
			{
				DrawLine(*triangle[0],*triangle[2]);
				continue;
			}
			if(triangle[0]->m_vScreenPosition.y==triangle[1]->m_vScreenPosition.y)
			{
				//FillFlatFootTriangle
				FillFlatHeadTriangle(*triangle[0],*triangle[1],*triangle[2]);
				continue;
			}
			if(triangle[1]->m_vScreenPosition.y==triangle[2]->m_vScreenPosition.y)
			{
				FillFlatFootTriangle(*triangle[0],*triangle[1],*triangle[2]);
				continue;
			}
			FillCommonTriangle(*triangle[0],*triangle[1],*triangle[2]);
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

	void Device::NearCull(UINT index[3],const Plane &CullPlane)
	{
		float cull;
		bool allInvisibal=false;
		for(int i=0;i<3;i++)
		{
			//cull=CullPlane*m_vecVSOutput[index[i]].m_vProjectCutting;
			if(m_vecVSOutput[index[i]].m_vProjectCutting.w<1.0f)
			{
				allInvisibal=true;
				break;
			}

		}
		if(allInvisibal)
		{
			for(int i=0;i<3;i++)
				m_vecVSOutput[index[i]].m_bVisible=false;
		}
	}

	void Device::CullInScreen( float &x, float &y)
	{
		if(x<0)
			x=0;
		if(x>m_iWidth)
			x=m_iWidth;
		if(y<0)
			y=0;
		if(y>m_iHeight)
			y=m_iHeight;
	}

	void Device::FillFlatHeadTriangle(VSShaderOutput &v0,VSShaderOutput &v1,VSShaderOutput &v2)
	{
		float x0=v0.m_vScreenPosition.x;
		float y0=v0.m_vScreenPosition.y;
		float x1=v1.m_vScreenPosition.x;
		float y1=v1.m_vScreenPosition.y;
		float x2=v2.m_vScreenPosition.x;
		float y2=v2.m_vScreenPosition.y;
		float lerp_step=1/(y2-y0);
		float lerp=0.0f;
		VSShaderOutput tmp_left;
		VSShaderOutput tmp_right;
		for(int y=y2;y>=ceil(y0)-1;y--)
		{
			tmp_left=PrespectLerp(v2,v0,lerp);
			tmp_right=PrespectLerp(v2,v1,lerp);
			FillLine(tmp_left,tmp_right);
			lerp+=lerp_step;
		}
	}

	void Device::FillFlatFootTriangle(VSShaderOutput &v0,VSShaderOutput &v1,VSShaderOutput &v2)
	{
		float x0=v0.m_vScreenPosition.x;
		float y0=v0.m_vScreenPosition.y;
		float x1=v1.m_vScreenPosition.x;
		float y1=v1.m_vScreenPosition.y;
		float x2=v2.m_vScreenPosition.x;
		float y2=v2.m_vScreenPosition.y;
		float lerp_step=1/(y1-y0);
		float lerp=0.0f;
		VSShaderOutput tmp_left;
		VSShaderOutput tmp_right;
		FillLine(v0,v0);	
		for(float y=y0+1;y<ceil(y1)-1;y++)
		{
			lerp=(y-y0)/(y2-y0);
			tmp_left=PrespectLerp(v0,v1,lerp);	
			tmp_right=PrespectLerp(v0,v2,lerp);	
			FillLine(tmp_left,tmp_right);
		}
	}

	void Device::FillCommonTriangle(VSShaderOutput &v0,VSShaderOutput &v1,VSShaderOutput &v2)
	{
		float x0=v0.m_vScreenPosition.x;
		float y0=v0.m_vScreenPosition.y;
		float x1=v1.m_vScreenPosition.x;
		float y1=v1.m_vScreenPosition.y;
		float x2=v2.m_vScreenPosition.x;
		float y2=v2.m_vScreenPosition.y;	
		float change_lerp=(y1-y0)/(y2-y0);
		VSShaderOutput tmp_change=PrespectLerp(v0,v2,change_lerp);
		if(tmp_change.m_vScreenPosition.x<=v1.m_vScreenPosition.x)
		{
			FillFlatFootTriangle(v0,tmp_change,v1);
			FillFlatHeadTriangle(tmp_change,v1,v2);
		}
		else
		{
			FillFlatFootTriangle(v0,v1,tmp_change);
			FillFlatHeadTriangle(v1,tmp_change,v2);
		}

	}

	void Device::FillLine(const VSShaderOutput &out0,const VSShaderOutput &out1)
	{
		int x0,y0,x1,y1;
		x0=out0.m_vScreenPosition.x;
		y0=ceil(out0.m_vScreenPosition.y)-1;
		x1=out1.m_vScreenPosition.x;
		y1=out1.m_vScreenPosition.y;
		int color;
		float lerp;
		VSShaderOutput tmp;
		float lerpstep;
		lerpstep=1.0f/float(x1-x0);
		lerp=0.0f;
		for(int i=x0;i<x1;i++)
		{
			tmp=PrespectLerp(out0,out1,lerp);	
			if(tmp.m_vPosition.w<=GetZBuffer(i,y0))
			{
				color=m_pPs->PSMain(tmp);
				DrawPixel(i,y0,color);
				SetZBuffer(i,y0,tmp.m_vPosition.w+1);
			}
			lerp+=lerpstep;
		}
	}

	float Device::GetZBuffer(int x,int y)
	{
		return z_buffer[x+y*m_iWidth];
	}

	void Device::SetZBuffer(int x,int y,float f)
	{
		z_buffer[x+y*m_iWidth]=f;
	}

	void Device::SetTexture(bmp*pbmp)
	{
		m_pTexture=pbmp;
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
					 if(v->m_byteType==DECLTYPE_FLOAT2&&v->m_byteUsageIndex==0)		
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

	SoftEngine::VSShaderOutput PrespectLerp(const VSShaderOutput &out0,const VSShaderOutput &out1,float f)
	{
		VSShaderOutput tmp;
		float repo_z0=1/out0.m_vPosition.w;
		float repo_z1=1/out1.m_vPosition.w;
		float repo_z3=(1-f)*repo_z0+f*repo_z1;
		float z3=1/repo_z3;
		tmp.m_vScreenPosition=Lerp(out0.m_vScreenPosition,out1.m_vScreenPosition,f);
		tmp.m_vProjectCutting=Lerp(out0.m_vProjectCutting*repo_z0,out1.m_vProjectCutting*repo_z1,f)*z3;
		tmp.m_vPosition=Lerp(out0.m_vPosition*repo_z0,out1.m_vPosition*repo_z1,f)*z3;
		tmp.m_vNormal=Lerp(out0.m_vNormal*repo_z0,out1.m_vNormal*repo_z1,f)*z3;
		tmp.m_vWordPOsition=Lerp(out0.m_vWordPOsition*repo_z0,out1.m_vWordPOsition*repo_z1,f)*z3;
		tmp.m_vTexcoord=Lerp(out0.m_vTexcoord*repo_z0,out1.m_vTexcoord*repo_z1,f)*z3;
		tmp.m_vColor=Lerp(out0.m_vColor*repo_z0,out1.m_vColor*repo_z1,f)*z3;
		tmp.m_bVisible=out0.m_bVisible || out1.m_bVisible;
		return tmp;
	}

	SoftEngine::VSShaderOutput Lerp(const VSShaderOutput &out0,const VSShaderOutput &out1,float f)
	{
		VSShaderOutput tmp;
		tmp.m_vScreenPosition=Lerp(out0.m_vScreenPosition,out1.m_vScreenPosition,f);
		tmp.m_vProjectCutting=Lerp(out0.m_vProjectCutting,out1.m_vProjectCutting,f);
		tmp.m_vPosition=Lerp(out0.m_vPosition,out1.m_vPosition,f);
		tmp.m_vNormal=Lerp(out0.m_vNormal,out1.m_vNormal,f);
		tmp.m_vWordPOsition=Lerp(out0.m_vWordPOsition,out1.m_vWordPOsition,f);
		tmp.m_vTexcoord=Lerp(out0.m_vTexcoord,out1.m_vTexcoord,f);
		tmp.m_vColor=Lerp(out0.m_vColor,out1.m_vColor,f);
		tmp.m_bVisible=out0.m_bVisible || out1.m_bVisible;
		return tmp;
	}

	float Lerpf(float f0,float f1,float lerp)
	{
		return (1-lerp)*f0+lerp*f1;
	}
	int TextureSampler::GetColor(float u,float v)
	{
		int x=Lerpf(0,m_iBmpWidth-1,u);
		int y=Lerpf(0,m_iBmpHeight-1,v);
		if(y<0)
			y=0;
		if(x<0)
			x=0;
		if(y>m_iBmpHeight-1)
			y=m_iBmpHeight-1;
		if(x>m_iBmpWidth-1)
			x=m_iBmpWidth-1;
		return m_pBmp->GetColor(x,y);
	}

	void TextureSampler::SetBMP(bmp* pBmp)
	{
		m_pBmp=pBmp;
		m_iBmpWidth=pBmp->width;
		m_iBmpHeight=pBmp->height;
	}

}