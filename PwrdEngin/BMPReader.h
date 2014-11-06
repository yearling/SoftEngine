#pragma once
#ifndef __BMPREADER_H__
#define __BMPREADER_H__
#include "stdafx.h"
class BMPReader
{
public:
	BMPReader(void);
	~BMPReader(void);
	static BMPReader &GetInstance() { return m_instance;}
	bool LoadBMP(std::string strPath);
	int GetPicWidth();
	int GetPicHeight();
private:
	static BMPReader m_instance;
};


#endif