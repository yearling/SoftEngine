#ifndef __PARAMETER_H__
#define __PARAMETER_H__
#include "stdafx.h"
#include "Gamemath.h"
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
	};
}
#endif