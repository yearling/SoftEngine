#pragma once
#ifndef __BMPREADER_H__
#define __BMPREADER_H__
#include "stdafx.h"
#include <wingdi.h>
struct bmp
{
	BITMAPFILEHEADER bmpHead;
	BITMAPINFOHEADER infoHead;
	unsigned char *buffer;
	int GetColor(int x,int y);
	int width;
	int height;
	int imageSize;
	int pixelPerBits;
};
class BMPReader
{
public:
	BMPReader(void);
	~BMPReader(void);
	static BMPReader &GetInstance() { return m_instance;}
	bmp* LoadBMP(std::string strPath);
private:
	static BMPReader m_instance;
};


#endif