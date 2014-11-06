#include "StdAfx.h"
#include "BMPReader.h"
#include <stdio.h>


BMPReader::BMPReader(void)
{
}


BMPReader::~BMPReader(void)
{
}
//一定要是2次幂的图
bmp* BMPReader::LoadBMP(std::string strPath)
{
	bmp* pBmp=new bmp;
	memset(pBmp,0,sizeof(pBmp));
	FILE *fp;
	if((fp=fopen(strPath.c_str(),"r"))==NULL)
	{
		delete pBmp;
		return nullptr;
	}
	fread(&pBmp->bmpHead,sizeof(BITMAPFILEHEADER),1,fp);
	fread(&pBmp->infoHead,sizeof(BITMAPINFOHEADER),1,fp);
	int offset=pBmp->bmpHead.bfOffBits;
	pBmp->width=pBmp->infoHead.biWidth;
	pBmp->height=pBmp->infoHead.biHeight;
	pBmp->imageSize=pBmp->infoHead.biSizeImage;
	pBmp->pixelPerBits=pBmp->infoHead.biBitCount;
	fseek(fp,offset,SEEK_SET);
	pBmp->buffer=new unsigned char[pBmp->imageSize];
	fread(pBmp->buffer,sizeof(unsigned char),pBmp->imageSize,fp);
	return pBmp;
}

BMPReader BMPReader::m_instance;

int bmp::GetColor(int x,int y)
{
	int offset=((height-1-y)*width+x)*3;
	unsigned char r=buffer[offset+2];
	unsigned char g=buffer[offset+1];
	unsigned char b=buffer[offset];
	return _RGB(r,g,b);
}
