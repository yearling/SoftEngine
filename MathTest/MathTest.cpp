// MathTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <d3dx9math.h>
#include <iomanip>
using namespace std;
void PrintMatrix(const D3DXMATRIX &m)
{
	for(int i=0;i<4;i++)
	{
		for(int j=0;j<4;j++)
		{
			cout<<std::setw(15)<<m.m[i][j];
		}
		cout<<endl;
	}
}
int _tmain(int argc, _TCHAR* argv[])
{
	D3DXMATRIX m;
	float width=800;
	float height=600;
	float aspert=width/height;
	D3DXMatrixPerspectiveFovLH(&m,D3DX_PI/4,aspert,1.0f,500.f);
	PrintMatrix(m);
	cout<<endl;
	D3DXVECTOR3 eye(100,242,365);
	D3DXVECTOR3 at(0,-20,155);
	D3DXVECTOR3 up(0,1,0);
	D3DXMatrixLookAtLH(&m,&eye,&at,&up);
	PrintMatrix(m);
	cout<<endl;
	at.x=-60;
	eye.y=79;
	D3DXMatrixLookAtLH(&m,&eye,&at,&up);
	PrintMatrix(m);
	/*
	-0.795432      -0.212791      -0.567458
	0       0.936332      -0.351115
	0.606043      -0.279288      -0.744789
	*/	
	D3DXVECTOR3 fx(-0.795432f,0.0f,0.606043f);
	D3DXVECTOR3 fz(-0.567458f,-0.351115f,-0.744789f);
	D3DXVECTOR3 vec;
	D3DXVec3Cross(&vec,&fz,&fx);
	std::cout<<vec.x<<"		"<<vec.y<<"		"<<vec.z<<std::endl;
	system("pause");
	return 0;
}

