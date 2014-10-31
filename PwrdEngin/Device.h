#ifndef __RENDER_H__
#define __RENDER_H__
#pragma once
#include "stdafx.h"
#include "DrawImp.h"
#include "application.h"
#include <vector>
namespace SoftEngine
{
	struct RenderVertex
	{
		Vector4 position_;
		Vector3 normal_;
		Vector2 texcoord_;
		bool visible;
	};
	enum PRIMITIVETYPE
	{
		PT_TRIANGLEIST=0,
	};
	enum DECLEAR_TYPE
	{
		DECLTYPE_FLOAT1=0,
		DECLTYPE_FLOAT2,
		DECLTYPE_FLOAT3,
		DECLTYPE_FLOAT4,
		DECLTYPE_COLOR
	};
	enum DECLEAR_USAGE
	{
		DECLUSAGE_POSITION,
		DECLUSAGE_NORMAL,
		DECLUSAGE_TEXCOORD,
		DECLUSAGE_COLOR,
		DECLUSAGE_END
	};
#define DECL_END() {0xff,0xff,DECLUSAGE_END,0xff}
	struct VERTEXELEMENT
	{
		WORD offset_;
		BYTE type_;					//类型，即DECLEAR_TYPE
		BYTE usage_;				//用途，即DECLEAR_USAGE
		BYTE usage_index_;			//用途索引
	};
	class VertexDeclaration
	{
	public:
		VertexDeclaration();
		bool SetVertexDeclaration(const VERTEXELEMENT *v);
		inline int GetPositionOffset() const {return cache_position_offset_;}
		inline int GetSize(){ return size_;}
	private:
		std::vector<VERTEXELEMENT> vec_vertex_;
		int cache_position_offset_;
		int cache_color_offset_;
		int cache_normal_offset_;
		int cache_texcoord_offset_;
		int size_;
	};
	
	class VertexBuffer
	{
	public:
		VertexBuffer();
		bool CreateBuffer(UINT length);
		void *Lock(UINT offset_to_lock=0,UINT size_to_lock=0);
		void UnLock();
		const byte *GetBuffer() { return buffer_;}
		~VertexBuffer();
	private:
		 byte *buffer_;
		 UINT length_;
		 bool locked_;
	};
	class IndexBuffer
	{
	public:
		IndexBuffer();
		~IndexBuffer();
		bool CreateIndexBuffer(UINT length);
		UINT *Lock(UINT offset_to_lock=0,UINT size_to_lock=0);
		void UnLock();
		const UINT*GetBuffer() {return buffer_;}
	private:
		UINT *buffer_;
		UINT length_;
		bool locked_;
	};
	
	class Device
	{
	public:
		Device();
		~Device();
		bool Init(DrawImp* draw_imp);
		bool Init(const Window *windows);
		bool BeginScene();
		bool EndScene();
		bool Clear(UINT color);
		bool Present();
		void SetWorld(Matrix *world);
		void SetView(Matrix *view);
		void SetProject(Matrix *pro);
		void SetStreamSource(VertexBuffer *p);
		void SetIndices(IndexBuffer*p);
		VertexDeclaration* CreateVertexDeclaration(VERTEXELEMENT v[]);
		bool SetVertexDeclaration(VertexDeclaration *p);
		inline void DrawPixel(int x,int y,DWORD color=_RGB(255,255,255))
		{
			back_buffer_[x+y*pitch]=color;
		}
		
		void DrawLine(int x0,int y0,int x1,int y1,int color=_RGB(255,255,255));
		bool Draw2DClipe(const RECT &rc,int & x0,int &y0,int &x1,int &y1);
		void SetViewPort( int x=0,int y=0,int width=0,int height=0,float minZ=0.0f,float maxZ=1.0f);
		VertexBuffer* CreateVertexBuffer(UINT length);
		IndexBuffer* CreateIndexBuffer(UINT length);
		bool DrawIndexedPrimitive(PRIMITIVETYPE type,int base_vertex_index,UINT min_index,
			UINT num_vertics,UINT start_index,UINT primitiveCount);
	private:
		inline Vector3 ToVector3(const byte* base_ptr,UINT pos,UINT data_size,UINT offset);
		bool DrawIndexedTrianglelist(int base_vertex_index,UINT min_index,
			UINT num_vertics,UINT start_index,UINT primitiveCount);
	private:
		DrawImp *draw_imp_;
		UINT *back_buffer_;
		int pitch;
		int width_;
		int height_;
		RECT clipe_rc;
		Matrix world_;
		Matrix view_;
		Matrix project_;
		Matrix view_port_;
		VertexDeclaration* vertex_declaration_;
		VertexBuffer *des_vertex_buffer_;
		IndexBuffer *des_index_buffer_;
		
		std::vector<RenderVertex> des_render_buffer_;
	};
}
#endif