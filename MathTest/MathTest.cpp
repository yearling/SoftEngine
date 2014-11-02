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
	D3DXQUATERNION q(2.4f,5.1f,8.3f,2.9f);
	D3DXQUATERNION p(2.4f,5.1f,8.3f,2.9f);
	D3DXQUATERNION m;
	D3DXQuaternionMultiply(&m,&q,&p);
	cout<<m.x<<"	"<<m.y<<"	"<<m.z<<"	"<<m.w<<endl;
	system("pause");
	return 0;
}

