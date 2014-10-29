#include "stdafx.h"
#include "application.h"
#include "game.h"
#include <iostream>
#include <windef.h>
using namespace SoftEngine;
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	Game theApp;
	int result=0;
	Application& yy_app=GetApplication();
	try {
		yy_app.Initial();
		result=yy_app.Run();
		yy_app.Exit();
	}
	catch(std::exception &e)
	{
		UNREFERENCED_PARAMETER(e);
		std::cout<<"catched in main"<<std::endl;
		system("pause");
	}
	return result;
}

