#include "StdAfx.h"
#include "FbxPaser.h"
#include "Gamemath.h"
#include <assert.h>
#include <fbxsdk/scene/geometry/fbxgeometry.h>
namespace SoftEngine
{
	FbxPaser::FbxPaser()
	{
		if(!FBXManager::GetInstance().GetInitialed())
			FBXManager::GetInstance().Init();
		vertex_buffer_=nullptr;
		index_buffer_=nullptr;
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
		ProcessNode(lRootNode);
		return true;
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
	//递归处理子节点
	void FbxPaser::ProcessNode(FbxNode* node)
	{
		assert(node && "node should not be empty");
		int num_attr=node->GetNodeAttributeCount();
		for(int i=0;i<num_attr;i++)
			if(node->GetNodeAttributeByIndex(i))
			{
				switch(node->GetNodeAttribute()->GetAttributeType())
				{
				case FbxNodeAttribute::eMesh:
					ProcessMesh(node);
					break;
				case FbxNodeAttribute::eLight:
					break;
				case FbxNodeAttribute::eCamera:
					break;
				default:
					break;
				}
			}
		int num_children_nodes=node->GetChildCount();
		for(int i=0;i<num_children_nodes;i++)
			ProcessNode(node->GetChild(i));
	}

	void FbxPaser::ProcessMesh(FbxNode* node)
	{
		FbxMesh* mesh=node->GetMesh();
		assert(mesh->IsTriangleMesh());
		if(mesh==nullptr)
			return;
		int triangle_count=mesh->GetPolygonCount();
		num_faces_=triangle_count;
		int vertex_count=0;
		Vector3 position[3];
		Vector4 color[3];
		Vector3 normal[3];
		Vector2 uv[3][2];
		vertex_buffer_=device_->CreateVertexBuffer(triangle_count*3*sizeof(FbxRenderData));
		index_buffer_=device_->CreateIndexBuffer(triangle_count*3);
		assert(vertex_buffer_ && "fbx create VertexBuffer failed!");
		assert(index_buffer_ && "fbx create IndexBuffer failed!");
		
		for(int i=0;i<triangle_count;i++)
		{
			for(int j=0;j<3;j++)
			{
				int ctrl_point_index=mesh->GetPolygonVertex(i,j);
				ProcessVertex(mesh,ctrl_point_index,position[j]);
				for(int k=0;k<2;k++)
				ProcessUV(mesh,ctrl_point_index,mesh->GetTextureUVIndex(i,j),k,uv[j][k]);
				ProcessNormal(mesh,ctrl_point_index,vertex_count,normal[j]);
				vertex_count++;
			}
			for(int i=0;i<3;i++)
			{
				FbxRenderData da;
				da.position=position[i];
				da.normal=normal[i];
				da.color=color[i];
				da.uv=uv[i][0];
				parse_buffer_.push_back(da);
			}
		}
		void *v_buffer_data=vertex_buffer_->Lock();
		memcpy(v_buffer_data,&parse_buffer_[0],vertex_buffer_->GetSize());
		vertex_buffer_->UnLock();
		UINT *i_buffer_data=index_buffer_->Lock();
		for(UINT i=0;i<index_buffer_->GetSize();i++)
			i_buffer_data[i]=i;
	}

	void FbxPaser::ProcessVertex(FbxMesh* mesh,int index,Vector3 &v)
	{
		FbxVector4 * control_point=mesh->GetControlPoints();
		v.x=control_point[index][0];
		v.y=control_point[index][1];
		v.z=control_point[index][2];
	}

	void FbxPaser::ProcessUV(FbxMesh* mesh,int index,int tex_uv,int uvlayer,Vector2& v)
	{
		if(uvlayer>=2 || mesh->GetElementUVCount()<=uvlayer)
			return;
		FbxLayerElementUV* vertex_uv=mesh->GetElementUV(uvlayer);
		switch(vertex_uv->GetMappingMode())
		{
		case FbxLayerElement::eByControlPoint:
			switch(vertex_uv->GetReferenceMode())
			{
			case FbxLayerElement::eDirect:
				v.x=vertex_uv->GetDirectArray().GetAt(index)[0];
				v.y=vertex_uv->GetDirectArray().GetAt(index)[1];
				break;
			case FbxLayerElement::eIndexToDirect:
				{
					int id=vertex_uv->GetIndexArray().GetAt(index);
					v.x=vertex_uv->GetDirectArray().GetAt(id)[0];
					v.y=vertex_uv->GetDirectArray().GetAt(id)[1];
					break;
				}
			default:
				break;
			}
			break;
		case FbxLayerElement::eByPolygonVertex:
			{
				switch(vertex_uv->GetReferenceMode())
				{
				case FbxLayerElement::eDirect:
				case FbxLayerElement::eIndexToDirect:
					{
						v.x=vertex_uv->GetDirectArray().GetAt(tex_uv)[0];
						v.y=vertex_uv->GetDirectArray().GetAt(tex_uv)[1];
					}
					break;
				default:
					break;
				}
			}
			break;
		default:
			break;
		}

	}

	void FbxPaser::ProcessNormal(FbxMesh *mesh,int index,int vertex_counter,Vector3& v)
	{
		if(mesh->GetElementNormalCount()<1)
			return;
		FbxLayerElementNormal* layer_normal=mesh->GetElementNormal(0);
		switch(layer_normal->GetMappingMode())
		{
		case FbxLayerElement::eByControlPoint:
			switch(layer_normal->GetReferenceMode())
			{
			case FbxLayerElement::eDirect:
				{
					v.x=layer_normal->GetDirectArray().GetAt(index)[0];
					v.y=layer_normal->GetDirectArray().GetAt(index)[1];
					v.z=layer_normal->GetDirectArray().GetAt(index)[2];
				}
				break;
			case FbxLayerElement::eIndexToDirect:
				{
					int id=layer_normal->GetIndexArray().GetAt(index);
					v.x=layer_normal->GetDirectArray().GetAt(id)[0];
					v.y=layer_normal->GetDirectArray().GetAt(id)[1];
					v.z=layer_normal->GetDirectArray().GetAt(id)[2];
				}
				break;
			default:
				break;
			}
			break;
		case FbxLayerElement::eByPolygonVertex:
			switch(layer_normal->GetReferenceMode())
			{
			case FbxGeometryElement::eDirect:
				{
					v.x=layer_normal->GetDirectArray().GetAt(vertex_counter)[0];
					v.y=layer_normal->GetDirectArray().GetAt(vertex_counter)[1];
					v.z=layer_normal->GetDirectArray().GetAt(vertex_counter)[2];
				}
				break;
			case FbxGeometryElement::eIndexToDirect:
				{
					int id=layer_normal->GetIndexArray().GetAt(vertex_counter);
					v.x=layer_normal->GetDirectArray().GetAt(id)[0];
					v.y=layer_normal->GetDirectArray().GetAt(id)[1];
					v.z=layer_normal->GetDirectArray().GetAt(id)[2];
				}
				break;
			default:
				break;
			}
			break;
		}
	}

}