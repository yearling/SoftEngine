#include "StdAfx.h"
#include "FbxPaser.h"
#include "Gamemath.h"
#include <assert.h>
#include <fbxsdk/scene/geometry/fbxgeometry.h>
#include <algorithm>
namespace SoftEngine
{
	FbxPaser::FbxPaser()
	{
		if(!FBXManager::GetInstance().GetInitialed())
			FBXManager::GetInstance().Init();
		m_pVertexBuffer=nullptr;
		m_pIndexBuffer=nullptr;
		m_uNumFaces=0;
		m_uNumVertex=0;
	}

	FbxPaser::FbxPaser(const string &file_name):m_strFileName(file_name)
	{
		if(!FBXManager::GetInstance().GetInitialed())
			FBXManager::GetInstance().Init();
		m_uNumFaces=0;
		m_uNumVertex=0;
	}

	bool FbxPaser::Load(string file_name)
	{
		if(file_name.empty())
		{
			file_name=m_strFileName;
		}
		else
			m_strFileName=file_name;
		if(file_name.empty())
			return false;
		assert(FBXManager::GetInstance().GetInitialed());
		m_pScene=FBXManager::GetInstance().CreateScence(file_name,"");
		if(m_pScene)
			if(Parse())
				return true;
		return false;
	}

	bool FbxPaser::Parse()
	{
		assert(m_pScene &&" scene_ should not be empty");
		//设置顶点格式
		VERTEXELEMENT v_list[]={
			{0,DECLTYPE_FLOAT3,DECLUSAGE_POSITION,0},
			{12,DECLTYPE_FLOAT3,DECLUSAGE_NORMAL,0},
			{24,DECLTYPE_FLOAT4,DECLUSAGE_COLOR,0},
			{40,DECLTYPE_FLOAT2,DECLUSAGE_TEXCOORD,0},
			DECL_END()
		};
		m_pVertexDecl=m_pDevice->CreateVertexDeclaration(v_list);
		assert(m_pVertexDecl && "create vertexelement error");
		FbxNode* lRootNode = m_pScene->GetRootNode();
		ProcessNode(lRootNode);
		return true;
	}

	bool FbxPaser::Init(Device *d)
	{
		if(d)
		{
			m_pDevice=d;
			return true;
		}else
			return false;

	}
	//递归处理子节点
	void FbxPaser::ProcessNode(FbxNode* node)
	{
		assert(node && "node should not be empty");
	/*	FbxAMatrix & gTransform=node->EvaluateGlobalTransform();*/
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
		m_uNumFaces=triangle_count;
		int vertex_count=0;
		Vector3 position[3];
		Vector4 color[3];
		Vector3 normal[3];
		Vector2 uv[3][2];
		UINT   index_change[3];
		
		
		UINT index_cur=0;//用来计算index
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
			
			//法向取反,z轴取反
				for(int i=0;i<3;i++)
				{
					FbxRenderData da;
					memset(&da,0,sizeof(da));
					da.position=position[i];
					da.position.z=-da.position.z;
					da.normal=-normal[i];
					da.color=color[i];
					da.uv=uv[i][0];
					UINT tmp;
					if(FindSameRenderData(da,tmp))
					{
						index_change[i]=tmp;
					}
					else
					{
						m_vecParseRenderDataBuffer.push_back(da);
						index_change[i]=index_cur++;
					}
				}
				//反绕序
				m_vecParseIndexBuffer.push_back(index_change[2]);
				m_vecParseIndexBuffer.push_back(index_change[1]);
				m_vecParseIndexBuffer.push_back(index_change[0]);
		}
		m_uNumVertex=m_vecParseRenderDataBuffer.size();
		m_pVertexBuffer=m_pDevice->CreateVertexBuffer(m_vecParseRenderDataBuffer.size()*sizeof(FbxRenderData));
		m_pIndexBuffer=m_pDevice->CreateIndexBuffer(m_vecParseIndexBuffer.size());
		assert(m_pVertexBuffer && "fbx create VertexBuffer failed!");
		assert(m_pIndexBuffer && "fbx create IndexBuffer failed!");
		assert(m_vecParseIndexBuffer.size()==m_uNumFaces*3);
		void *v_buffer_data=m_pVertexBuffer->Lock();
		memcpy(v_buffer_data,&m_vecParseRenderDataBuffer[0],m_pVertexBuffer->GetSize());
		m_pVertexBuffer->UnLock();
		assert(m_pIndexBuffer->GetSize()==m_vecParseIndexBuffer.size() && "should not be error,check the parser");
		UINT *i_buffer_data=m_pIndexBuffer->Lock();
		memcpy(i_buffer_data,&m_vecParseIndexBuffer[0],sizeof(UINT)*m_pIndexBuffer->GetSize());
		m_pIndexBuffer->UnLock();
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

	bool FbxPaser::FindSameRenderData(const FbxRenderData &data,UINT &pos)
	{
		auto iter=std::find(m_vecParseRenderDataBuffer.begin(),m_vecParseRenderDataBuffer.end(),data);
		if(iter==m_vecParseRenderDataBuffer.end())
			return false;
		else
		{
#if defined(DEBUG) || defined( _DEGUB)
			//如果还有重复点，说明插入有问题
			auto test_iter=std::find(iter+1,m_vecParseRenderDataBuffer.end(),data);
			assert(test_iter==m_vecParseRenderDataBuffer.end());
#endif
			pos=std::distance(m_vecParseRenderDataBuffer.begin(),iter);
			return true;
		}
	}


	bool FbxRenderData::operator==(const FbxRenderData &data)
	{
		if(position==data.position && normal==data.normal && color==data.color && uv==data.uv)
			return true;
		else
		return false;
	}

}