#include "StdAfx.h"
#include "FBXManager.h"
#include "Device.h"
#include <assert.h>
#include <iosfwd>
#include <sstream>
namespace SoftEngine
{
	FBXManager::FBXManager(void):manager_(nullptr),importer_(nullptr),
		io_setting_(nullptr),initialed_(false)
	{
	}


	FBXManager::~FBXManager(void)
	{
		if(importer_)
			importer_->Destroy();
		if(io_setting_)
			io_setting_->Destroy();
		if(manager_)
			manager_->Destroy();
	}

	void FBXManager::Init()
	{
		manager_=FbxManager::Create();
		io_setting_=FbxIOSettings::Create(manager_,IOSROOT);
		manager_->SetIOSettings(io_setting_);
		importer_=FbxImporter::Create(manager_,"");
		initialed_=true;
	}

	FbxScene* FBXManager::CreateScence(std::string file_path,std::string scene_name)
	{
		if(!GetInitialed())
			Init();
		assert(manager_&&io_setting_&&importer_&&"用之前要Init");
		if(!importer_->Initialize(file_path.c_str(), -1, io_setting_)) { 
			std::stringstream ss;	
			ss<<"Call to FbxImporter::Initialize() failed.\n"; 
			ss<<"file_path:"<<file_path<<"scene_name:"<<scene_name<<endl;
			ss<<"Error resturn :"<<importer_->GetStatus().GetErrorString()<<std::endl;
			string tmp;
			ss>>tmp;
			cout<<tmp<<endl;
			return nullptr;
		}
		FbxScene *scene=FbxScene::Create(manager_,scene_name.c_str());
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
		if(!importer_->Import(scene))
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

	SoftEngine::FBXManager FBXManager::instance_;


	
}