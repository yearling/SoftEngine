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
		static FBXManager& GetInstance(){ return fbxInstance;}
		FbxScene* CreateScence(std::string file_path,std::string scene_name);
		bool GetInitialed() const{ return m_bInitial;}
	private:
		static FBXManager fbxInstance;
		bool m_bInitial;
		FbxManager *m_pManager;
		FbxImporter *m_pImporter;
		FbxIOSettings *m_pIoSetting;
	};
	
}

#endif
