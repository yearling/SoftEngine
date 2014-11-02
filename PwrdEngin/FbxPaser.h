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
		VertexDeclaration *GetVertexDeclaration() const { return decl_;}
		VertexBuffer *GetVertexBuffer() const {return vertex_buffer_;}
		IndexBuffer *GetIndexBuffer() const {return index_buffer_;}
		UINT GetFaceNumber() const {return num_faces_;}
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
		FbxScene *scene_;
		Device *device_;
		string file_name_;
		VertexBuffer *vertex_buffer_;
		IndexBuffer *index_buffer_;
		VertexDeclaration *decl_;
		std::vector<FbxRenderData> parse_render_data_buffer_;//�����������������ݣ��ٴ�loadʱ�����ý�����
		std::vector<UINT> parse_index_buffer_;//��vector���棬Ȼ�����index_buffer�����Բ��ã�����Ϊ�������汣��һ��
		UINT num_faces_;
	};
}
#endif

