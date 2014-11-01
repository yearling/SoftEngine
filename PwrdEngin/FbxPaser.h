#ifndef __FBXPASER_H__
#define  __FBXPASER_H__
#pragma once
#include "stdafx.h"
#include <fbxsdk.h>
#include "FBXManager.h"
#include "Device.h"
namespace SoftEngine
{
	class FbxPaser
	{
	public:
		FbxPaser();
		explicit FbxPaser(const string &file_name);
		bool Init(Device *d);
		~FbxPaser();
		bool Load(string file_name);
		VertexDeclaration *GetVertexDeclaration() const { return decl_;}
	private:
		FbxScene *scene_;
		Device *device_;
		string file_name_;
		bool Parse();
		void ProcessNode(FbxNode* node);
		VertexDeclaration *decl_;
	};
}
#endif

