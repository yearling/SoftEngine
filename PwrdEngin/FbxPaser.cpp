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
			{48,DECLTYPE_FLOAT3,DECLUSAGE_NORMAL,1},
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
		Vector2 uv[3];
		UINT   index_change[3];
		Vector3 tangant[3];
		
		UINT index_cur=0;//用来计算index
		for(int i=0;i<triangle_count;i++)
		{
			for(int j=0;j<3;j++)
			{
				int ctrl_point_index=mesh->GetPolygonVertex(i,j);
				ProcessVertex(mesh,ctrl_point_index,position[j]);
				for(int k=0;k<2;k++)
				ProcessUV(mesh,ctrl_point_index,mesh->GetTextureUVIndex(i,j),0,uv[j]);
				ProcessNormal(mesh,ctrl_point_index,vertex_count,normal[j]);
				ProcessColor(mesh,ctrl_point_index,vertex_count,color[j]);
				vertex_count++;
			}
			
			//法向取反,z轴取反
			for(int i=0;i<3;i++)
			{
				position[i].z=-position[i].z;
				//normal[i]=normal[i];
				//normal[i].z=-normal[i].z;
				uv[i].y=1.0f-uv[i].y;
			}
			//CaculateTangant(position,uv,tangant);
			for(int i=0;i<3;i++)
			{
				FbxRenderData da;
				memset(&da,0,sizeof(da));
				da.position=position[i];
				//da.normal=normal[i];
				da.color=color[i];
				da.uv=uv[i];
				//da.tangant=tangant[i];
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
		CaculateNormalAndTrangant();
		std::cout<<"uv from fbx finished!"<<endl;
		m_uNumVertex=m_vecParseRenderDataBuffer.size();
		m_pVertexBuffer=m_pDevice->CreateVertexBuffer(m_vecParseRenderDataBuffer.size()*sizeof(FbxRenderData));
		m_pIndexBuffer=m_pDevice->CreateIndexBuffer(m_vecParseIndexBuffer.size());
		assert(m_pVertexBuffer && "fbx create VertexBuffer failed!");
		assert(m_pIndexBuffer && "fbx create IndexBuffer failed!");
		//assert(m_vecParseIndexBuffer.size()==m_uNumFaces*3);
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

	void FbxPaser::ProcessColor(FbxMesh*mesh,int index,int vertex_counter,Vector4 &v)
	{
		if(mesh->GetElementVertexColorCount()<1)
			return;
		FbxLayerElementVertexColor *pVertexColor=mesh->GetElementVertexColor(0);
		switch(pVertexColor->GetMappingMode())
		{
		case FbxLayerElement::eByControlPoint:
			{
				switch(pVertexColor->GetReferenceMode())
				{
				case FbxLayerElement::eDirect:
					{
						v.x=pVertexColor->GetDirectArray().GetAt(index).mRed;
						v.y=pVertexColor->GetDirectArray().GetAt(index).mGreen;
						v.z=pVertexColor->GetDirectArray().GetAt(index).mBlue;
						v.w=pVertexColor->GetDirectArray().GetAt(index).mAlpha;
						break;
					}
				case FbxLayerElement::eIndexToDirect:
					{
						int pos=pVertexColor->GetIndexArray().GetAt(index);
						v.x=pVertexColor->GetDirectArray().GetAt(pos).mRed;
						v.y=pVertexColor->GetDirectArray().GetAt(pos).mGreen;
						v.z=pVertexColor->GetDirectArray().GetAt(pos).mBlue;
						v.w=pVertexColor->GetDirectArray().GetAt(pos).mAlpha;
						break;
					}
				default:
					break;
				}
			}
			break;
		case FbxLayerElement::eByPolygonVertex:
			{
				switch(pVertexColor->GetReferenceMode())
				{
				case FbxLayerElement::eDirect:
					{
						v.x=pVertexColor->GetDirectArray().GetAt(vertex_counter).mRed;
						v.y=pVertexColor->GetDirectArray().GetAt(vertex_counter).mGreen;
						v.z=pVertexColor->GetDirectArray().GetAt(vertex_counter).mBlue;
						v.w=pVertexColor->GetDirectArray().GetAt(vertex_counter).mAlpha;
						break;
					}
				case FbxLayerElement::eIndexToDirect:
					{
						int pos=pVertexColor->GetIndexArray().GetAt(vertex_counter);
						v.x=pVertexColor->GetDirectArray().GetAt(pos).mRed;
						v.y=pVertexColor->GetDirectArray().GetAt(pos).mGreen;
						v.z=pVertexColor->GetDirectArray().GetAt(pos).mBlue;
						v.w=pVertexColor->GetDirectArray().GetAt(pos).mAlpha;
						break;
					}
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
		if(position==data.position/* && normal==data.normal && color==data.color */&& uv==data.uv/*&&tangant==data.tangant*/)
			return true;
		else
		return false;
	}
	void FbxPaser::CaculateTangant(const Vector3 position[3],const Vector2 uv[3],Vector3 tangant[3])
	{
		CaculateTangant(position[0],position[1],position[2],uv[0],uv[1],uv[2],tangant[0]);
		CaculateTangant(position[1],position[2],position[0],uv[1],uv[2],uv[0],tangant[1]);
		CaculateTangant(position[2],position[0],position[1],uv[2],uv[0],uv[1],tangant[2]);
	}

	void FbxPaser::CaculateTangant(const Vector3& position0,const Vector3 position1,const Vector3 position2, const Vector2& uv0,const Vector2& uv1,const Vector2 &uv2,Vector3 &tangant)
	{
		Vector3 e1=position1-position0;
		Vector3 e2=position2-position0;
		float t1=uv1.x-uv0.x;
		float b1=uv1.y-uv0.y;
		float t2=uv2.x-uv0.x;
		float b2=uv2.y-uv0.y;
		float dertamin=1/(t1*b2-b1*t2);
		float tangX=b2*e1.x-b1*e2.x;
		float tangY=b2*e1.y-b1*e2.y;
		float tangZ=b2*e1.z-b1*e2.z;
		tangant=Vector3(dertamin*tangX,dertamin*tangY,dertamin*tangZ).Normalize();
	}

	void FbxPaser::CaculateNormalAndTrangant()
	{
		auto dwTriCount=m_uNumFaces;
		m_vecBase.clear();
		m_SameVertex.clear();
		std::vector<Matrix3x3> vTriangleBase;
		//first step: caculate tranengle space per triangle
		{
			for(unsigned int i=0;i<m_vecParseIndexBuffer.size();)
			{
				unsigned int index[3];
				for(int j=0;j<3;j++,i++)
				{
					index[j]=m_vecParseIndexBuffer[i];
				}
				Vector3 vEdges[3];
				Vector2 vUV[3];
				for(int j=0;j<3;j++)
				{
					vEdges[j]=m_vecParseRenderDataBuffer[index[j]].position;
					vUV[j]=m_vecParseRenderDataBuffer[index[j]].uv;
				}
				auto v10=vEdges[1]-vEdges[0];
				auto v20=vEdges[2]-vEdges[0];
				auto t1=vUV[1].x-vUV[0].x;
				auto b1=vUV[1].y-vUV[0].y;
				auto t2=vUV[2].x-vUV[0].x;
				auto b2=vUV[2].y-vUV[0].y;
				float deter=(t1*b2-b1*t2);
				if(isnan(deter))
					deter=0.0f;
				Vector3 u,v,n;
				n=(v10^v20).Normalize();
				if(deter!=0.0f)
				{
					float area=fabsf(deter);
					u=Vector3((b2*v10.x-b1*v20.x)/deter,
							  (b2*v10.y-b1*v20.y)/deter,
							  (b2*v10.z-b1*v20.z)/deter).Normalize();
					v=Vector3((t1*v20.x-t2*v10.x)/deter,
							   (t1*v20.y-t2*v10.y)/deter,
							   (t1*v20.z-t2*v10.z)/deter).Normalize();
					u*=area;
					v*=area;
				}
				else
				{
					u=Vector3(1.0f,0.0f,0.0f);
					v=Vector3(0.0f,1.0f,0.0f);
				}
				vTriangleBase.push_back(Matrix3x3(u,v,n));
			}
		}
		//caculate normals per vertex
		{
			for(unsigned int i=0;i<m_vecParseIndexBuffer.size();)
			{
				unsigned int index[3];
				Matrix3x3 matTriBase=vTriangleBase[i/3];
				for(int j=0;j<3;j++,i++)
				{
					index[j]=m_vecParseIndexBuffer[i];
				}
				Vector3 vEdges[3];
				for(int j=0;j<3;j++)
				{
					vEdges[j]=m_vecParseRenderDataBuffer[index[j]].position;
				}
				for(int j=0;j<3;j++)
				{
					float fWeight=GetAngle(vEdges[(j+2)%3]-vEdges[j],vEdges[(j+1)%3]-vEdges[j]);
					if(fWeight<=0.0f)
						fWeight=0.00001f;
					AddNormal(i-3+j,matTriBase.n*fWeight);
				}

			}
		}
		//uv to vertices
		{
			for(unsigned int i=0;i<m_vecParseIndexBuffer.size();)
			{
				unsigned int index[3];
				Matrix3x3 matTriBase=vTriangleBase[i/3];
				for(int j=0;j<3;j++,i++)
				{
					index[j]=m_vecParseIndexBuffer[i];
				}
				Vector3 vEdges[3];

				for(int j=0;j<3;j++)
				{
					vEdges[j]=m_vecParseRenderDataBuffer[index[j]].position;
				}
				for(int j=0;j<3;j++)
				{
					float fWeight=GetAngle(vEdges[(j+2)%3]-vEdges[j],vEdges[(j+1)%3]-vEdges[j]);
					Vector3 tmp=matTriBase.n;
					tmp.Normalize();
					UINT pos=AddUV(i-3+j,matTriBase.u*fWeight,matTriBase.v*fWeight,tmp);
					m_vecAssign.push_back(pos);
				}
			}
		}
		//normarize
		{
			for(auto iter=m_vecBase.begin();iter!=m_vecBase.end();++iter)
			{
				Matrix3x3 &mat=*iter;
				Vector3 u,v,n;
				Normalize(&n,&mat.n);
				u=mat.u-n*(n*mat.u);
				v=mat.v-n*(n*mat.v);
				mat.u=u.Normalize();
				mat.v=v.Normalize();
				mat.n=n.Normalize();
			}
		}
		for(int i=0;i<m_vecParseIndexBuffer.size();i++)
		{
			Matrix3x3 tmp=m_vecBase[m_vecAssign[i]];
			auto &data=m_vecParseRenderDataBuffer[m_vecParseIndexBuffer[i]];
			data.normal=tmp.n;
			data.tangant=tmp.u;
		}
	}

	void FbxPaser::AddNormal(UINT index,const Vector3 &v)
	{
		auto iter=m_SameVertex.find(index);
		UINT finalIndex;
		if(iter!=m_SameVertex.end())
		{
			finalIndex=iter->second;
		}
		else
		{
			Matrix3x3 base(Vector3(0,0,0),Vector3(0,0,0),Vector3(0,0,0));
			finalIndex=m_vecBase.size();
			m_SameVertex.insert(std::make_pair(index,finalIndex));
			m_vecBase.push_back(base);
		}
		Matrix3x3 & base=m_vecBase[finalIndex];
		base.n=base.n+v;
	}

	UINT FbxPaser::AddUV(UINT index,const Vector3 &u,const Vector3 &v,const Vector3 &n)
	{
		auto iter=m_SameVertex.lower_bound(index);
		assert(iter!=m_SameVertex.end());
		Vector3 vNormal=m_vecBase[iter->second].n;
		auto iter_end=m_SameVertex.upper_bound(index);
		UINT  dwBaseUVIndex=INFINITE;
		bool bParity=((u^v)*n>0.0f);
		for(;iter!=iter_end;++iter)
		{
			Matrix3x3 & matFount=m_vecBase[iter->second];
			if(!(matFount.u.x==0.0f&&matFount.u.y==0.0f &&matFount.u.z==0.0f))
			{
				bool bParityRef=((matFount.u ^matFount.v)*matFount.n>0.0f);
				bool bParityCheck=(bParityRef==bParity);
				if(!bParityCheck)
					continue;
				Vector3 fuv=matFount.u+matFount.v;
				Vector3 vLShape=fuv-n*(n*fuv);
				bool bLShapeCheck=(u+v)*vLShape>0.0f;
				if(!bLShapeCheck)
					continue;
			}
			dwBaseUVIndex=iter->second;
			break;
		}
		if(dwBaseUVIndex==INFINITE)
		{
			Matrix3x3 base(Vector3(0,0,0),Vector3(0,0,0),Vector3(0,0,0));
			dwBaseUVIndex=m_vecBase.size();
			m_SameVertex.insert(std::make_pair(index,dwBaseUVIndex));
			m_vecBase.push_back(base);
		}
		Matrix3x3 & BaseUV=m_vecBase[dwBaseUVIndex];
		BaseUV.u=BaseUV.u+u;
		BaseUV.v=BaseUV.v+v;
		return dwBaseUVIndex;
	}


}