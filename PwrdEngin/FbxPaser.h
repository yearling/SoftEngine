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
	private:
		bool Parse();
		void ProcessNode(FbxNode* node);
		void ProcessMesh(FbxNode* node);
		void ProcessVertex(FbxMesh* mesh,int index,Vector3 &v);
		void ProcessColor();
		void ProcessUV(FbxMesh* mesh,int index,int tex_uv,int uvlayer,Vector2& v);
		void ProcessNormal(FbxMesh *mesh,int index,int vertex_counter,Vector3& v);
		bool FindSameRenderData(const FbxRenderData &data,UINT &pos);
	private:
		FbxScene *m_pScene;
		Device *m_pDevice;
		string m_strFileName;
		VertexBuffer *m_pVertexBuffer;
		IndexBuffer *m_pIndexBuffer;
		VertexDeclaration *m_pVertexDecl;
		std::vector<FbxRenderData> m_vecParseRenderDataBuffer;//�����������������ݣ��ٴ�loadʱ�����ý�����
		std::vector<UINT> m_vecParseIndexBuffer;//��vector���棬Ȼ�����index_buffer�����Բ��ã�����Ϊ�������汣��һ��
		UINT m_uNumFaces;
	};
}
#endif
