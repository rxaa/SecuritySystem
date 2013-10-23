// SecuritySystem.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "SecuritySystem.h"

#include "../../df/cmd.h"

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

	G::MainListen();

	df::cmd cm;

	COUT(cm.Exec(tcc_("dir c:\\ \r\n")));
	COUT(cm.Exec(tcc_("dir d:\\  \r\n")));
	COUT(cm.Exec(tcc_("\r\n \r\n")));

#ifdef _TEST
#	include "Test.h"
#else
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
#endif

	return 1;
}



//////////////////////////////////////////////////////////////////////////


MainModel G::main{ tcc_(""), false, defaultPort_ };

List < G::ConnectPtr> G::serverList_;
df::Lock G::listLock_;
