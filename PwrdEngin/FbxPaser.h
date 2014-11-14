#ifndef __FBXPASER_H__
#define  __FBXPASER_H__
#pragma once
#include "stdafx.h"
#include <fbxsdk.h>
#include "FBXManager.h"
#include "Device.h"
namespace SoftEngine
{
	struct FbxRenderData
	{
		Vector3 position;
		Vector3 normal;
		Vector4 color;
		Vector2 uv;
		Vector3 tangant;
		bool operator==(const FbxRenderData &data);
	};
	class FbxPaser
	{
	public:
		FbxPaser();
		explicit FbxPaser(const string &file_name);
		bool Init(Device *d);
		~FbxPaser();
		bool Load(string file_name);
		VertexDeclaration *GetVertexDeclaration() const { return m_pVertexDecl;}
		VertexBuffer *GetVertexBuffer() const {return m_pVertexBuffer;}
		IndexBuffer *GetIndexBuffer() const {return m_pIndexBuffer;}
		UINT GetFaceNumber() const {return m_uNumFaces;}
		UINT GetVertexNumber() const {return m_uNumVertex;}
	private:
		bool Parse();
		void ProcessNode(FbxNode* node);
		void ProcessMesh(FbxNode* node);
		void ProcessVertex(FbxMesh* mesh,int index,Vector3 &v);
		void ProcessColor();
		void ProcessUV(FbxMesh* mesh,int index,int tex_uv,int uvlayer,Vector2& v);
		void ProcessNormal(FbxMesh *mesh,int index,int vertex_counter,Vector3& v);
		void ProcessColor(FbxMesh*mesh,int index,int vertex_counter,Vector4 &v);
		bool FindSameRenderData(const FbxRenderData &data,UINT &pos);
	private:
		void CaculateTangant(const Vector3 position[3],const Vector2 uv[3],Vector3 tangant[3]);
		void CaculateTangant(const Vector3& position0,const Vector3 position1,const Vector3 position2,
			const Vector2& uv0,const Vector2& uv1,const Vector2 &uv2,Vector3 &tangant);
		FbxScene *m_pScene;
		Device *m_pDevice;
		string m_strFileName;
		VertexBuffer *m_pVertexBuffer;
		IndexBuffer *m_pIndexBuffer;
		VertexDeclaration *m_pVertexDecl;
		std::vector<FbxRenderData> m_vecParseRenderDataBuffer;//用来保存解析后的内容，再次load时，不用解析。
		std::vector<UINT> m_vecParseIndexBuffer;//用vector保存，然后放入index_buffer，可以不用，但是为了与上面保持一致
		UINT m_uNumFaces;
		UINT m_uNumVertex;
		string m_strLightDirect;
	};
}
#endif

