// SecuritySystem.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "SecuritySystem.h"


int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE,
	LPTSTR,
	int)
{

	if (::FindWindow(0, t_t("Security System")) != 0)
	{
		df::msg(t_t("程序正在运行!"));
		return 0;
	}

	df::Init(hInstance);

	G::ReadMainIni();

	MainConnecter::InitVerifyKey();

	UCHAR sss[16] = {'1','2','3'};
	UCHAR sss2[16] = { '1', '2', '3' };
	MainConnecter::VerifyCrypt_.Encrypt(sss, sss, 16);
	MainConnecter::VerifyCrypt_.Encrypt(sss2, sss2, 16);

	G::MainListen();

	if (G::main.hide_window)
	{
	}
	else
	{
		Window::InitImage(IDB_BITMAP1);
		//MainForm().Run();
		NewWindow<MainForm>()->Run();
	}

	return 1;
}
