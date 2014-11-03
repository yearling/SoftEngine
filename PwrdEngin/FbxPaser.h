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
		VertexDeclaration *GetVertexDeclaration() const { return pVertexDecl;}
		VertexBuffer *GetVertexBuffer() const {return pVertexBuffer;}
		IndexBuffer *GetIndexBuffer() const {return pIndexBuffer;}
		UINT GetFaceNumber() const {return uNumFaces;}
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
		FbxScene *pScene;
		Device *pDevice;
		string strFileName;
		VertexBuffer *pVertexBuffer;
		IndexBuffer *pIndexBuffer;
		VertexDeclaration *pVertexDecl;
		std::vector<FbxRenderData> vecParseRenderDataBuffer;//用来保存解析后的内容，再次load时，不用解析。
		std::vector<UINT> vecParseIndexBuffer;//用vector保存，然后放入index_buffer，可以不用，但是为了与上面保持一致
		UINT uNumFaces;
	};
}
#endif

