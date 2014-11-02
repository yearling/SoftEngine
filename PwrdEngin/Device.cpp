#include "StdAfx.h"
#include "Device.h"
#include "Gamemath.h"
#include <assert.h>
#include <math.h>
#include <iostream>
using namespace std;
namespace SoftEngine
{
	Device::Device():draw_imp_(nullptr),
					back_buffer_(nullptr),
					pitch(0),width_(0),height_(0),
					vertex_declaration_(nullptr),
					des_index_buffer_(nullptr),
					des_vertex_buffer_(nullptr)
	{
		memset(&clipe_rc,0,sizeof(clipe_rc));
		MatrixIdentity(&world_);
		MatrixIdentity(&view_);
		MatrixIdentity(&project_);
		MatrixIdentity(&view_port_);
	}

	bool Device::Init(DrawImp* draw_imp)
	{
		draw_imp_=draw_imp;
		return true;
	}

	bool Device::Init(const Window *windows)
	{
		if(windows==nullptr)
			return false;
		else
		{
			draw_imp_=new DrawImp();
			if(!draw_imp_->Init(windows->hwnd_,windows->width_,windows->height_,windows->client_offset_x_,windows->client_offset_y_,windows->is_windowed_))
				return false;
		}
		return true;
	}

	bool Device::BeginScene()
	{
		assert(draw_imp_&&"render should init");
		if(back_buffer_=draw_imp_->LockBackSurface(&pitch,&width_,&height_))
		{
			clipe_rc.left=0;
			clipe_rc.top=0;
			clipe_rc.right=width_;
			clipe_rc.bottom=height_;
			return true;
		}
		return false;
	}

	bool Device::EndScene()
	{
		draw_imp_->UnlockBackSurface();
		pitch=0;
		width_=0;
		height_=0;
		back_buffer_=nullptr;
		memset(&clipe_rc,0,sizeof(clipe_rc));
		return true;
	}

	void Device::DrawLine(int x0,int y0,int x1,int y1,int color)
	{
		if(!Draw2DClipe(clipe_rc,x0,y0,x1,y1))
			return;
		if(x0==x1 && y0==y1)
		{
			DrawPixel(x0,y0,color);
			return;
		}
		int dx,dy;
		dx=x1-x0;
		dy=y1-y0;
		int step;
		abs(dx)>abs(dy)?step=abs(dx):step=abs(dy);
		float x_add=static_cast<float>(dx)/static_cast<float>(step);
		float y_add=static_cast<float>(dy)/static_cast<float>(step);
		float x=x0;
		float y=y0;
		for(int i=0;i<step;i++)
		{
			DrawPixel(Round(x),Round(y),color);
			x+=x_add;
			y+=y_add;
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
		draw_imp_->ClearBackBuffer(color);
		return true;
	}

	bool Device::Present()
	{
		des_render_buffer_.clear();
		draw_imp_->Flip();
		return true;
	}

	void Device::SetViewPort(int x/*=0*/,int y/*=0*/,int width/*=0*/,int height/*=0*/,float minZ/*=0.0f*/,float maxZ/*=1.0f*/)
	{
		if(width==0)
			width=width_;
		if(height==0)
			height=height_;
			view_port_.m[0][0]=width/2;view_port_.m[0][1]=0.0f;view_port_.m[0][2]=0.0f;view_port_.m[0][3]=0.0f;
			view_port_.m[1][0]=0.0f;view_port_.m[1][1]=-height/2;view_port_.m[1][2]=0.0f;view_port_.m[1][3]=0.0f;
			view_port_.m[2][0]=0.0f;view_port_.m[2][1]=0.0f;view_port_.m[2][2]=maxZ-minZ;view_port_.m[2][3]=0.0f;
			view_port_.m[3][0]=x+width/2;view_port_.m[3][1]=y+height/2;view_port_.m[3][2]=minZ;view_port_.m[3][3]=1.0f;
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
				vertex_declaration_=p;
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

	void Device::SetWorld(Matrix *world)
	{
		if(world)
			world_=*world;
	}

	void Device::SetView(Matrix *view)
	{
		if(view)
			view_=*view;
	}

	void Device::SetStreamSource(VertexBuffer *p)
	{
		des_vertex_buffer_=p;
	}

	void Device::SetIndices(IndexBuffer*p)
	{
		des_index_buffer_=p;
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
		assert(des_vertex_buffer_ && des_index_buffer_ &&vertex_declaration_);
		int position_offset=vertex_declaration_->GetPositionOffset();
		int strip=vertex_declaration_->GetSize();
		const byte *vertex_buffer_trans=des_vertex_buffer_->GetBuffer()+base_vertex_index*strip;
		const UINT *index_buffer_=des_index_buffer_->GetBuffer();
		for(UINT i=0;i<primitiveCount*3;i++)
		{
			RenderVertex tmp;
			tmp.visible=true;
			UINT pos=index_buffer_[start_index++];
			tmp.position_=ToVector3(vertex_buffer_trans,pos,strip,position_offset);
			des_render_buffer_.push_back(tmp);
		}
		Matrix trans=world_*view_*project_*view_port_;
		for(auto iter=des_render_buffer_.begin();iter!=des_render_buffer_.end();++iter)
		{
			iter->position_*=trans;
			iter->position_.ProjectDivied();
		}
		for(UINT i=0;i<des_render_buffer_.size()/3;)
		{
			Vector4 &p0=des_render_buffer_[i*3+0].position_;
			Vector4 &p1=des_render_buffer_[i*3+1].position_;
			Vector4 &p2=des_render_buffer_[i*3+2].position_;
			DrawLine(p0.x,p0.y,p1.x,p1.y);
			DrawLine(p1.x,p1.y,p2.x,p2.y);
			DrawLine(p2.x,p2.y,p0.x,p0.y);
			i++;
		}
		return true;
	}

	Vector3 Device::ToVector3(const byte* base_ptr,UINT pos,UINT data_size,UINT offset)
	{
		const byte* p=base_ptr+(pos*data_size+offset);
		return Vector3(reinterpret_cast<const float*>(p));
	}

	void Device::SetProject(Matrix *pro)
	{
		if(pro)
		project_=*pro;
	}

	bool Device::TextDraw(std::string text, int x,int y,DWORD color)
	{
		draw_imp_->DrawTextGDI(text,x,y,color);
		return(1);
	}

	

	


	VertexDeclaration::VertexDeclaration():cache_position_offset_(0),
		cache_color_offset_(0),cache_normal_offset_(0),cache_texcoord_offset_(0),
		size_(0)
	{
	}

	bool VertexDeclaration::SetVertexDeclaration(const VERTEXELEMENT *v)
	{
		if(v==nullptr)
			return false;
		vec_vertex_.clear();
		cache_position_offset_=0;
		cache_color_offset_=0;
		cache_normal_offset_=0;
		cache_texcoord_offset_=0;
		size_=0;
		if(v)
		{
			while(v->usage_!=DECLUSAGE_END)
			{
				switch(v->type_)
				{
				case DECLTYPE_FLOAT1:
						size_+=4;
					break;
				case DECLTYPE_FLOAT2:
					size_+=8;
					break;
				case DECLTYPE_FLOAT3:
					size_+=12;
					break;
				case DECLTYPE_FLOAT4:
					size_+=16;
					break;
				case DECLTYPE_UINT:
					size_+=4;
					break;
				default:
					break;
				}
				vec_vertex_.push_back(*v);
				switch(v->usage_)
				{
				case DECLUSAGE_POSITION:
					 if(v->type_==DECLTYPE_FLOAT3&&v->usage_index_==0)		
						cache_position_offset_=v->offset_;
					 break;
				case DECLUSAGE_NORMAL:
					 if(v->type_==DECLTYPE_FLOAT3&&v->usage_index_==0)		
						cache_normal_offset_=v->offset_;
					 break;
				case DECLUSAGE_TEXCOORD:
					 if(v->type_==DECLTYPE_FLOAT3&&v->usage_index_==0)		
						 cache_texcoord_offset_=v->offset_;
					 break;
				case DECLUSAGE_COLOR:
					 if(v->type_==DECLTYPE_FLOAT3&&v->usage_index_==0)		
						 cache_color_offset_=v->offset_;
						break;
				default:
					break;
				}
				v++;
			}
		}
		return true;
	}


	VertexBuffer::VertexBuffer():buffer_(nullptr),locked_(false)
	{

	}

	VertexBuffer::~VertexBuffer()
	{
		if(buffer_)
			delete[] buffer_;
	}

	bool VertexBuffer::CreateBuffer(UINT length)
	{
		if(length<=0)
			return false;
		if(length!=length_)
		{
			if(buffer_)
				delete[] buffer_;
		}
		else
			return true;
		buffer_=new byte[length];
		if(buffer_)
			{
				length_=length;
				return true;
			}
		else 
			return false;
	}

	void * VertexBuffer::Lock(UINT offset_to_lock,UINT size_to_lock)
	{
		if(locked_)
			return nullptr;
		if(offset_to_lock>length_-1)
			return nullptr;
		if(offset_to_lock+size_to_lock>length_)
			return nullptr;
		locked_=true;
		return  static_cast<void*>(buffer_+offset_to_lock);
	}

	void VertexBuffer::UnLock()
	{
		locked_=false;
	}


	IndexBuffer::IndexBuffer():buffer_(nullptr),length_(0),locked_(false)
	{

	}

	IndexBuffer::~IndexBuffer()
	{
		if(buffer_)
		{
			delete[] buffer_;
		}
	}

	bool IndexBuffer::CreateIndexBuffer(UINT length)
	{
		if(length<=0)
			return false;
		if(length!=length_)
		{
			if(buffer_)
				delete[] buffer_;
		}
		else
			return true;
		buffer_=new UINT[length];
		if(buffer_)
		{
			length_=length;
			return true;
		}
		else 
			return false;
	}

	UINT* IndexBuffer::Lock(UINT offset_to_lock/*=0*/,UINT size_to_lock/*=0*/)
	{
		if(locked_)
			return nullptr;
		if(offset_to_lock>length_-1)
			return nullptr;
		if(offset_to_lock+size_to_lock>length_)
			return nullptr;
		locked_=true;
		return  static_cast<UINT*>(buffer_+offset_to_lock);
	}

	void IndexBuffer::UnLock()
	{
		locked_=false;
	}

}