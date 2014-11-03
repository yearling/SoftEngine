#include "StdAfx.h"
#include "FBXManager.h"
#include "Device.h"
#include <assert.h>
#include <iosfwd>
#include <sstream>
namespace SoftEngine
{
	FBXManager::FBXManager(void):m_pManager(nullptr),m_pImporter(nullptr),
		m_pIoSetting(nullptr),m_bInitial(false)
	{
	}


	FBXManager::~FBXManager(void)
	{
		if(m_pImporter)
			m_pImporter->Destroy();
		if(m_pIoSetting)
			m_pIoSetting->Destroy();
		if(m_pManager)
			m_pManager->Destroy();
	}

	void FBXManager::Init()
	{
		m_pManager=FbxManager::Create();
		m_pIoSetting=FbxIOSettings::Create(m_pManager,IOSROOT);
		m_pManager->SetIOSettings(m_pIoSetting);
		m_pImporter=FbxImporter::Create(m_pManager,"");
		m_bInitial=true;
	}

	FbxScene* FBXManager::CreateScence(std::string file_path,std::string scene_name)
	{
		if(!GetInitialed())
			Init();
		assert(m_pManager&&m_pIoSetting&&m_pImporter&&"用之前要Init");
		if(!m_pImporter->Initialize(file_path.c_str(), -1, m_pIoSetting)) { 
			std::stringstream ss;	
			ss<<"Call to FbxImporter::Initialize() failed.\n"; 
			ss<<"file_path:"<<file_path<<"scene_name:"<<scene_name<<endl;
			ss<<"Error resturn :"<<m_pImporter->GetStatus().GetErrorString()<<std::endl;
			string tmp;
			ss>>tmp;
			cout<<tmp<<endl;
			return nullptr;
		}
		FbxScene *scene=FbxScene::Create(m_pManager,scene_name.c_str());
		if(scene==nullptr)
		{
			std::stringstream ss;
			ss<<"Create fbxscene failed!"<<std::endl;
			ss<<"file_path:"<<file_path<<"scene_name:"<<scene_name<<endl;
			string tmp;
			ss>>tmp;
			cout<<tmp<<endl;	
			return nullptr;
		}
		if(!m_pImporter->Import(scene))
		{
			std::stringstream ss;
			ss<<"import fbxscene failed!"<<std::endl;
			ss<<"file_path:"<<file_path<<"scene_name:"<<scene_name<<endl;
			string tmp;
			ss>>tmp;
			cout<<tmp<<endl;
			return nullptr;
		}
		if(scene)
		{
			FbxAxisSystem OriginSceneAxisSystem=scene->GetGlobalSettings().GetAxisSystem();
			FbxAxisSystem OurSystemAxisSystem=FbxAxisSystem::DirectX;
			if(OriginSceneAxisSystem!=OurSystemAxisSystem)
			{
				OurSystemAxisSystem.ConvertScene(scene);
			}
		}
		return scene;
	}

	SoftEngine::FBXManager FBXManager::fbxInstance;


	
}