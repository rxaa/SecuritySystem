// SecuritySystem.cpp : ����Ӧ�ó������ڵ㡣
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
		df::msg(t_t("������������!"));
		return 0;
	}

	df::Init(hInstance);

	G::ReadMainIni();

	MainConnecter::InitVerifyKey();

	G::MainListen();

	if (G::main.hide_window)
	{
		for (;;)
		{
			df::ThreadSleep(60 * 60 * 1000);
		}
	}
	else
	{
		Window::InitImage(IDB_BITMAP1);
		//MainForm().Run();
		NewWindow<FormMain>()->Run();
	}

	return 1;
}
