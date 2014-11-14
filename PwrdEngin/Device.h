#ifndef __RENDER_H__
#define __RENDER_H__
#pragma once
#include "stdafx.h"
#include "DrawImpl.h"
#include "application.h"
#include "BMPReader.h"
#include <vector>
namespace SoftEngine
{
	class Device;
	struct RenderVertex
	{
		Vector4 m_vPosition;
		Vector3 m_vNormal;
		Vector2 m_vTexcoord;
		Vector4 m_vColor;
		Vector3 m_vTangant;
		bool m_bVisible;
	};
	struct VSShaderOutput
	{
		Vector4 m_vScreenPosition;
		Vector4 m_vProjectCutting;
		Vector4 m_vPosition;
		Vector3 m_vNormal;
		Vector3 m_vWordPOsition;
		Vector2 m_vTexcoord;
		Vector4 m_vColor;
		Vector3 m_vTangant;
		bool m_bVisible;
	};
	VSShaderOutput PrespectLerp(const VSShaderOutput &out0,const VSShaderOutput &out1,float f);
	VSShaderOutput Lerp(const VSShaderOutput &out0,const VSShaderOutput &out1,float f);
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
		DECLTYPE_UINT
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
		WORD m_wOffset;
		unsigned char m_byteType;					//类型，即DECLEAR_TYPE
		unsigned char m_byteUsage;				//用途，即DECLEAR_USAGE
		unsigned char m_byteUsageIndex;			//用途索引
	};
	class VertexDeclaration
	{
	public:
		VertexDeclaration();
		bool SetVertexDeclaration(const VERTEXELEMENT *v);
		inline int GetPositionOffset() const {return m_iPositionOffsetCached;}
		inline int GetColorOffset() const {return m_iColorOffsetCached;}
		inline int GetUVOffset() const {return m_iTexcoordOffsetCached;}
		inline int GetNormalOffset() const {return m_iNormalOffsetCached;}
		inline int GetTangantOffset() const {return m_iTangantCached;}
		inline int GetSize(){ return m_iSize;}
	private:
		std::vector<VERTEXELEMENT> m_vecVertex;
		int m_iPositionOffsetCached;
		int m_iColorOffsetCached;
		int m_iNormalOffsetCached;
		int m_iTexcoordOffsetCached;
		int m_iTangantCached;
		int m_iSize;
	};
	
	class VertexBuffer
	{
	public:
		VertexBuffer();
		bool CreateBuffer(UINT length);
		void *Lock(UINT offset_to_lock=0,UINT size_to_lock=0);
		void UnLock();
		const byte *GetBuffer() { return m_pBuffer;}
		UINT GetSize() const {return m_uLength;}
		~VertexBuffer();
	private:
		 byte *m_pBuffer;
		 UINT m_uLength;
		 bool m_bLock;
	};
	class IndexBuffer
	{
	public:
		IndexBuffer();
		~IndexBuffer();
		bool CreateIndexBuffer(UINT length);
		UINT *Lock(UINT offset_to_lock=0,UINT size_to_lock=0);
		void UnLock();
		const UINT*GetBuffer() {return m_pBuffer;}
		UINT GetSize() const {return m_uLength;}
	private:
		UINT *m_pBuffer;
		UINT m_uLength;
		bool m_bLock;
	};
	enum CULLMODE
	{
		CULL_NONE=0,
		CULL_CW,
		CULL_CCW
	};
	enum FILLMODE
	{
		FILL_POINT,
		FILL_WIREFRAME,
		FILL_SOLID
	};
	class TextureSampler
	{
	public:
		TextureSampler():m_pBmp(nullptr){}
		void SetBMP(bmp* pBmp);
		int GetColor(float u,float v);
	private:
		bmp* m_pBmp;
		int m_iBmpWidth;
		int m_iBmpHeight;
	};
	class IVertexShader
	{
	public:
		IVertexShader(Device *p);
		void *GetGobalData();
		virtual ~IVertexShader()=0;
		virtual void VSMain(RenderVertex &v,VSShaderOutput &out)=0;
		virtual void BeginSetGlobalParam()=0;
	private:
		Device* m_pDevice;
		void *m_pData;
	};
	class IPixelShader
	{
	public:
		IPixelShader(Device *p);
		void *GetGlobalData();
		virtual ~IPixelShader()=0;
		virtual int PSMain(VSShaderOutput &)=0;
		virtual void BeginSetGlobalParam()=0;
	private:
		Device* m_pDevice;
		void *m_pData;
	};
	class Device
	{
	public:
		Device();
		~Device();
		bool Init(DrawImpGDI* draw_imp);
		bool Init(const Window *windows);
		bool BeginScene();
		bool EndScene();
		bool Clear(UINT color);
		bool Present();
		void SetStreamSource(VertexBuffer *p);
		void SetIndices(IndexBuffer*p);
		void SetGameSource(void *pSrc);
		void SetVS(IVertexShader*p){m_pVs=p;}
		void VSSetData();
		void PSSetData();
		void SetPS(IPixelShader*p){m_pPs=p;}
		void SetCullMode(CULLMODE mode=CULL_CCW) { m_cullmode=mode;}
		void SetFillMode(FILLMODE mode=FILL_SOLID) {m_fillmode=mode;}
		void *GetGameSource();
		Matrix* GetViewPort();
		VertexDeclaration* CreateVertexDeclaration(VERTEXELEMENT v[]);
		bool SetVertexDeclaration(VertexDeclaration *p);
		bool TextDraw(std::string text, int x,int y,DWORD color);
		inline void DrawPixel(int x,int y,DWORD color=_RGB(255,255,255))
		{
			//m_pBackBuffer[x+(m_iHeight-y-1)*m_iPitch]=color;
			m_pBackBuffer[x+y*m_iPitch]=color;
		}
		
		void DrawLine(int x0,int y0,int x1,int y1,int color=_RGB(255,255,255));
		void DrawLine(const VSShaderOutput &out0,const VSShaderOutput &out1);
		bool Draw2DClipe(const RECT &rc,int & x0,int &y0,int &x1,int &y1);
		void SetViewPort( int x=0,int y=0,int width=0,int height=0,float minZ=0.0f,float maxZ=1.0f);
		VertexBuffer* CreateVertexBuffer(UINT length);
		IndexBuffer* CreateIndexBuffer(UINT length);
		bool DrawIndexedPrimitive(PRIMITIVETYPE type,int base_vertex_index,UINT min_index,
			UINT num_vertics,UINT start_index,UINT primitiveCount);
	private:
		bool DrawIndexedTrianglelist(int base_vertex_index,UINT min_index,
			UINT num_vertics,UINT start_index,UINT primitiveCount);
		void FillPipline(int base_vertex_index,UINT num_vertics,UINT start_index,UINT primitiveCount);
		void FillWireFrame();
		void FillWireFrame(int index0,int index1);
		void FillSolid();
		void FaceCull(UINT index[3],const Plane &CullPlane);
		void NearCull(UINT index[3],const Plane &CullPlane);
		void OneVertexInView(UINT inIndex,UINT outIndex0,UINT outIndex1,const Plane &cullPlane);
		void TwoVertexInView(UINT inIndex0,UINT inIndex1,UINT outIndex,const Plane &cullPlane);
		void CullInScreen( float &x, float &y);
		void FillFlatHeadTriangle(VSShaderOutput &v0,VSShaderOutput &v1,VSShaderOutput &v2);
		void FillFlatFootTriangle(VSShaderOutput &v0,VSShaderOutput &v1,VSShaderOutput &v2);
		void FillCommonTriangle(VSShaderOutput &v0,VSShaderOutput &v1,VSShaderOutput &v2);
		void FillLine(const VSShaderOutput &out0,const VSShaderOutput &out1);
		void FillLine(const VSShaderOutput &out0,const VSShaderOutput &out1,int nline);
		inline float GetZBuffer(int x,int y);
		inline void SetZBuffer(int x,int y,float f);
		void SetTexture(bmp*pbmp);
		
	private:
		DrawBase *m_pDrawImpl;
		unsigned int *m_pBackBuffer;
		int m_iPitch;
		int m_iWidth;
		int m_iHeight;
		RECT m_rcClip;
		Matrix m_matWorld;
		Matrix m_matView;
		Matrix m_matProject;
		Matrix m_matViewPort;
		VertexDeclaration* m_pVertexDecl;
		VertexBuffer *m_pDesVertexBuffer;
		IndexBuffer *m_pDesIndexBuffer;
		std::vector<RenderVertex> m_vecRenderBuffer;
		std::vector<VSShaderOutput> m_vecVSOutput;
		std::vector<UINT> m_vecIndexBuffer;
		float *z_buffer;
		void *m_pGameSource;
		IVertexShader *m_pVs;
		IPixelShader *m_pPs;
		CULLMODE m_cullmode;
		FILLMODE m_fillmode;
		int addCount;
		bmp* m_pTexture;
	};
}
#endif