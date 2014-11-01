#include "StdAfx.h"
#include "FbxPaser.h"
#include <assert.h>
namespace SoftEngine
{
	FbxPaser::FbxPaser()
	{
		if(!FBXManager::GetInstance().GetInitialed())
			FBXManager::GetInstance().Init();
	}

	FbxPaser::FbxPaser(const string &file_name):file_name_(file_name)
	{
		if(!FBXManager::GetInstance().GetInitialed())
			FBXManager::GetInstance().Init();
	}

	bool FbxPaser::Load(string file_name)
	{
		if(file_name.empty())
		{
			file_name=file_name_;
		}
		else
			file_name_=file_name;
		if(file_name.empty())
			return false;
		assert(FBXManager::GetInstance().GetInitialed());
		scene_=FBXManager::GetInstance().CreateScence(file_name,"");
		if(scene_)
			if(Parse())
				return true;
		return false;
	}

	bool FbxPaser::Parse()
	{
		assert(scene_ &&" scene_ should not be empty");
		//设置顶点格式
		VERTEXELEMENT v_list[]={
			{0,DECLTYPE_FLOAT3,DECLUSAGE_POSITION,0},
			{12,DECLTYPE_FLOAT3,DECLUSAGE_NORMAL,0},
			{24,DECLTYPE_FLOAT4,DECLUSAGE_COLOR,0},
			{40,DECLTYPE_FLOAT2,DECLUSAGE_TEXCOORD,0},
			DECL_END()
		};
		decl_=device_->CreateVertexDeclaration(v_list);
		assert(decl_ && "create vertexelement error");
		FbxNode* lRootNode = scene_->GetRootNode();
		return false;
	}

	bool FbxPaser::Init(Device *d)
	{
		if(d)
		{
			device_=d;
			return true;
		}else
			return false;

	}

	void FbxPaser::ProcessNode(FbxNode* node)
	{
		assert(node && "node should not be empty");
		int num_attr=node->GetNodeAttributeCount();
		for(int i=0;i<num_attr;i++)
	}

}