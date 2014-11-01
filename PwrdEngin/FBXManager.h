#ifndef __FBXMANAGER_H__
#define  __FBXMANAGER_H__
#pragma once
#include "stdafx.h"
#include <fbxsdk.h>
#include <memory>
#include <string>
namespace SoftEngine
{
	class FBXManager
	{
	public:
		FBXManager(void);
		~FBXManager(void);
		void Init();
		static FBXManager& GetInstance(){ return instance_;}
		FbxScene* CreateScence(std::string file_path,std::string scene_name);
		bool GetInitialed() const{ return initialed_;}
	private:
		static FBXManager instance_;
		bool initialed_;
		FbxManager *manager_;
		FbxImporter *importer_;
		FbxIOSettings *io_setting_;
	};
	
}

#endif
