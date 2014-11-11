#ifndef __PARAMETER_H__
#define __PARAMETER_H__
#include "stdafx.h"
#include "Gamemath.h"
#include "Device.h"
namespace SoftEngine
{

	struct SeftData
	{
		Matrix world;
		Matrix view;
		Matrix project;
		Matrix viewPort;
		UINT ambient;
		UINT direct;
		TextureSampler *m_pSamper;	
		Vector3 vLightPosition;
		Vector3 vDirect;
		float fAmbientScalar;
		float fDirectScalar; 
	};
}
#endif